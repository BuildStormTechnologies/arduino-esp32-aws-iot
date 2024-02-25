/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file 01_ProvisioningByClaim.ino
 * \brief 01_ProvisioningByClaim.ino file.
 *
 * The 01_ProvisioningByClaim.ino main entry of the application.
 *
 * The file have been tested on the ESP32 modules.
 * Buildstorm explicitly denies responsibility for any hardware failures
 * arising from the use of these file, whether directly or indirectly.
 * Please note that files are subject to change without prior notice.
 *
 * EULA LICENSE:
 * This file is licensed under end user license EULA agreement.
 * The EULA is available at https://buildstorm.com/eula/
 * For any support contact us at hello@buildstorm.com
 *
 */

#include "app_config.h"
#include "LibSystem.h"
#include "LibAws.h"
#include "TFT_eSPI.h"
#include "icons.h"
#include "Free_Fonts.h"

extern const char aws_root_ca_pem[];
extern const char certificate_pem_crt[];
extern const char private_pem_key[];

LibSystem SYS;
Aws AWS;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

#define gray 0x6B6D

typedef enum {
  DISPLAY_STATUS_OFF,
  DISPLAY_STATUS_NO_CHANGE,
  DISPLAY_STATUS_TOGGLE,
  DISPLAY_STATUS_ON,
} displayStatusCodes_et;

static uint8_t eventWIFIState = DISPLAY_STATUS_OFF;
static uint8_t eventBLEState = DISPLAY_STATUS_OFF;
static uint8_t eventMQTTState = DISPLAY_STATUS_OFF;
bool printStatus = false;

mqttMsg_st s_mqttMsg = { 0 };

void app_init();
void app_loop();

void setup() {
  Serial.begin(115200);
  app_init();
  disp_init();
}

void loop() {
  app_loop();
  delay(100);
}

void app_init() {
  systemInitConfig_st sysConfig = { 0 };

  sysConfig.systemEventCallbackHandler = app_eventsCallBackHandler;
  sysConfig.pAppVersionStr = (char *)APP_VERSION;
  sysConfig.pWifiSsidStr = (char *)TEST_WIFI_SSID;
  sysConfig.pWifiPwdStr = (char *)TEST_WIFI_PASSWORD;

  sysConfig.s_mqttClientConfig.maxPubMsgToStore_u8 = 5;
  sysConfig.s_mqttClientConfig.maxSubMsgToStore_u8 = 6;
  sysConfig.s_mqttClientConfig.maxSubscribeTopics_u8 = 5;
  sysConfig.s_mqttClientConfig.pHostNameStr = (char *)AWS_IOT_MQTT_HOST;
  sysConfig.s_mqttClientConfig.port_u16 = AWS_IOT_MQTT_PORT;
  sysConfig.s_mqttClientConfig.pRootCaStr = (char *)aws_root_ca_pem;
  sysConfig.s_mqttClientConfig.pClaimCertStr = (char *)certificate_pem_crt;
  sysConfig.s_mqttClientConfig.pClaimPrivateKeyStr = (char *)private_pem_key;
  sysConfig.s_mqttClientConfig.pClaimTemplateStr = (char *)AWS_PROVISION_TEMPLATE_NAME;

    if (SYS.init(&sysConfig))
    {
        SYS.start();
        if (SYS.getMode() == SYSTEM_MODE_NORMAL)
        {
            AWS.subscribe((char *)TEST_AWS_TOPIC_SUBSCRIBE, QOS0_AT_MOST_ONCE);
        }    
    }
    else
    {
        while (1)
        {
            printf("\n System Init failed, verify the init config ....");
            delay(5000);
        }
    }
}

void app_loop() {
  static uint32_t nextPubTime = 0;
  static uint32_t count_u32 = 1;

  switch (SYS.getMode()) {
    case SYSTEM_MODE_DEVICE_CONFIG:
      if (millis() > nextPubTime) {
        nextPubTime = millis() + 2000;

        if (SYS.isDeviceRegistered()) 
        {
          // printf("\nDevice successfully provisioned");
          if(EVENT_MQTT_CONNECTED)
          {
            eventMQTTState = DISPLAY_STATUS_ON;
          }
          provisionedScreen();
          delay(2000);
          rebootingScreen();
          delay(2000);
        } else 
        {
          // printf("\nDevice is not provisioned");
          provisioningScreen();
        }
      }
      break;

    case SYSTEM_MODE_NORMAL:
      if (AWS.isConnected()) {
        if (AWS.read(&s_mqttMsg)) {
          printf("\n Received: [%s]:%s", s_mqttMsg.topicStr, s_mqttMsg.payloadStr);
        }
        if ((millis() > nextPubTime)) {

          s_mqttMsg.payloadLen_u16 = sprintf(s_mqttMsg.payloadStr, "{\"Hello from device\": %d}", count_u32++);
          strcpy(s_mqttMsg.topicStr, TEST_AWS_TOPIC_PUBLISH);
          s_mqttMsg.qos_e = QOS0_AT_MOST_ONCE;
          AWS.publish(&s_mqttMsg);
          nextPubTime = millis() + 5000;
          printf("\n %s\n", s_mqttMsg.payloadStr);
          afterProvisioned(s_mqttMsg.payloadStr);
        }
      }
      afterProvisioned(s_mqttMsg.payloadStr);
      break;

    case SYSTEM_MODE_OTA:
      break;

    default:
      break;
  }
}

void app_eventsCallBackHandler(systemEvents_et event_e) {
  switch (event_e) {
    case EVENT_WIFI_CONNECTED:
      eventWIFIState = DISPLAY_STATUS_ON;
      printStatus = true;
      printf("\nEVENT_WIFI_CONNECTED");
      break;
    case EVENT_WIFI_DISCONNECTED:
      eventWIFIState = DISPLAY_STATUS_OFF;
      printStatus = true;
      printf("\nEVENT_WIFI_DISCONNECTED");
      break;


    case EVENT_MQTT_CONNECTED:
      eventMQTTState = DISPLAY_STATUS_ON;
      printStatus = true;
      printf("\nEVENT_AWS_CONNECTED");
      break;
    case EVENT_MQTT_DISCONNECTED:
      eventMQTTState = DISPLAY_STATUS_OFF;
      printStatus = true;
      printf("\nEVENT_AWS_DISCONNECTED");
      break;

    default:
      break;
  }
}

void disp_init() {
  tft.init();
  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);

  sprite.createSprite(320, 170);
  sprite.setSwapBytes(true);
  sprite.setTextColor(TFT_WHITE);
  sprite.setTextDatum(4);
  sprite.fillSprite(TFT_BLACK);
}


void drawHeaderFooter() {
  sprite.setTextColor(TFT_SKYBLUE);
  sprite.setFreeFont(FF5);
  sprite.setTextSize(1);
  sprite.setCursor(10, 20);
  sprite.print("AWS IoT");
  sprite.drawRect(2, 142, 316, 26, TFT_WHITE);
  sprite.setTextColor(TFT_WHITE);
  sprite.drawString("Provision By Claim", 170, 152);
  sprite.drawRect(0, 0, 320, 170, gray);
}

void icon_status()
{
  static bool blinkState = false;

  if (!eventWIFIState)
  {
    if (blinkState)
    {
      sprite.pushImage(290, 2, 25, 25, wifiDiscon_icon);
    }
    else
    {
      sprite.drawRect(290, 2, 25, 25, TFT_BLACK);
    }
  }
  else
  {
    sprite.pushImage(290, 2, 25, 25, wifi_icon);
  }

  if (eventWIFIState==DISPLAY_STATUS_ON && !eventMQTTState)
  {
    if (blinkState)
    {
      sprite.pushImage(260, 4, 25, 25, awsDiscon_icon);
    }
    else
    {
      sprite.drawRect(260, 2, 25, 25, TFT_BLACK);
    }
  }
  else
  {
    sprite.pushImage(260, 4, 25, 25, awsDiscon_icon);
  }
  if(eventMQTTState==DISPLAY_STATUS_ON)
  {
   sprite.pushImage(260, 4, 25, 25, awsCon_icon);
  }

  blinkState = !blinkState;
}

void provisioningScreen() {
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Provisioning...", 170, 80);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void provisionedScreen() {
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Provisioned...", 170, 80);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void rebootingScreen() {
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Rebooting...", 170, 80);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void openScreen() {
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  if (SYS.isDeviceRegistered()) 
  {
    provsScreen();
  }
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void provsScreen() {
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Provision Successful", 170, 80);
  thingVersion();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}


void thingVersion() {
  sprite.setTextColor(TFT_SKYBLUE);
  sprite.setFreeFont(FF5);
  sprite.setTextSize(1);
  sprite.setCursor(10, 20);
  sprite.print(AWS.getThingName());
  int deviceNameLength = sprite.textWidth(AWS.getThingName());
  sprite.setFreeFont(FF0);
  sprite.setCursor(deviceNameLength + 15, 15);
  sprite.print(APP_VERSION);
  sprite.drawRect(2, 142, 316, 26, TFT_WHITE);
  sprite.setFreeFont(FF5);
  sprite.setTextColor(TFT_WHITE);
  sprite.drawString("Buildstorm Technologies", 170, 152);
  sprite.drawRect(0, 0, 320, 170, gray);
}

void afterProvisioned(String message) {
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
   sprite.drawString(message, 160, 80);
  thingVersion();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}
