/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file 05_OTA.ino
 * \brief 05_OTA.ino file.
 *
 * The 05_OTA.ino main entry of the application.
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

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "app_config.h"
#include "LibSystem.h"
#include "LibAws.h"
#include "TFT_eSPI.h"
#include "icons.h"
#include "Free_Fonts.h"

/* Macros ------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
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

//progress bar variables
int progress = 0;
int w = 300;
int h = 18;
int x = 12;
int y = 110;
int blocks = 0;

void setup();
void loop();
void loopTask(void *);

void setup() {
  Serial.begin(115200);
  disp_init();
  app_init();
}

void app_task(void *param)
{
  while (1)
  {
    // if (SYS.getMode() == SYSTEM_MODE_NORMAL)
    // {
    //   OTAScreen();
    // }
    // if (SYS.getMode() == SYSTEM_MODE_OTA)
    // {
    //   if (SYS.getOtaPercentage() == 100)
    //   {
    //     rebootScreen();
    //   }
    //   else if (SYS.getOtaPercentage() == 0)
    //   {
    //     OTARequestScreen();
    //   }
    //   else
    //   {
    //     progressbar(SYS.getOtaPercentage());
    //     //        progress++;
    //   }
    // }
    if (printStatus == true)
    {
      icon_status();
      printStatus = false;
    }

    vTaskDelay(10);
  }
}

void loop() {
  app_loop();
  delay(200);
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
  sysConfig.s_mqttClientConfig.pThingCertStr = (char *)certificate_pem_crt;
  sysConfig.s_mqttClientConfig.pThingPrivateKeyStr = (char *)private_pem_key;

  if (SYS.init(&sysConfig)) {
    SYS.start();
  } 
  else 
  {
    while (1)
     {
      printf("\n System Init failed, verify the init config ....");
      delay(5000);
     }
  }
  uint8_t err = xTaskCreate(&app_task, "app_task", TASK_APP_STACK_SIZE, NULL, TASK_APP_PRIORITY, NULL);
  if (pdPASS != err) 
  {
    printf("\n Error 0x%X in creating app_task \n restarting system\n\r\n\r", err);
  }
}

void app_loop() {

  switch (SYS.getMode()) 
  {

  case SYSTEM_MODE_NORMAL:
    OTAScreen();
    if (SYS.getOtaPercentage() == 100) {
        rebootScreen();
        delay(2000);
      }
    break;

  case SYSTEM_MODE_OTA:

    if (SYS.getMode() == SYSTEM_MODE_OTA) {
      if (SYS.getOtaPercentage() == 100) {
        rebootScreen();
        delay(2000);
      } else if (SYS.getOtaPercentage() == 0) {
        OTARequestScreen();
      } else {
        progressbar(SYS.getOtaPercentage());
        //        progress++;
      }
    }

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
  sprite.drawString("Buildstorm Technologies", 170, 152);
  sprite.drawRect(0, 0, 320, 170, gray);
}

void icon_status() {

  if (eventBLEState == DISPLAY_STATUS_ON) {
    sprite.pushImage(230, 2, 25, 25, bt_icon);
  } else {
    sprite.pushImage(230, 2, 25, 25, btDiscon_icon);
  }
  if (eventWIFIState == DISPLAY_STATUS_ON) {
    sprite.pushImage(260, 2, 25, 25, wifi_icon);
  } else {
    sprite.pushImage(260, 2, 25, 25, wifiDiscon_icon);
  }
  if (eventMQTTState == DISPLAY_STATUS_ON) {
    sprite.pushImage(290, 4, 25, 25, awsCon_icon);
  } else {
    sprite.pushImage(290, 4, 25, 25, awsDiscon_icon);
  }
}

void progressbar(int progress)
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170,   gray);
  sprite.setFreeFont(&Orbitron_Light_24);
  //  sprite.drawString("Progress:", 75, 16);
  //  sprite.setFreeFont(&Orbitron_Light_32);

  sprite.drawString(String(progress) + "%", 170, 80);

  int blockWidth = w / 100;
  blocks = progress;
  sprite.drawRoundRect(x, y, w, h, 3, TFT_WHITE);

  for (int i = 0; i < blocks; i++) {
    sprite.fillRect(x + 2 + i * blockWidth, y + 4, blockWidth - 1.5, 10, TFT_GREEN);
  }
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(300);
}

void rebootScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  sprite.drawString("Rebooting...", 170, 80);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void OTAScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170, gray);
  sprite.setTextColor (TFT_WHITE);
  sprite.setFreeFont(FF5);
  sprite.drawString("OTA UPDATES", 170, 80);
  sprite.drawString(APP_VERSION, 170, 100);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void OTARequestScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect  (  0, 0, 320, 170, gray);
  sprite.setTextColor (TFT_WHITE);
  sprite.setFreeFont(FF5);
  sprite.drawString("OTA Requested", 170, 80);
  //  sprite.drawString(APP_VERSION, 170, 100);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}
