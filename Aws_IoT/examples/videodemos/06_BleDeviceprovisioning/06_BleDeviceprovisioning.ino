/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file 06_BleDeviceprovisioning.ino
 * \brief 06_BleDeviceprovisioning.ino file.
 *
 * The 06_BleDeviceprovisioning.ino main entry of the application.
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
#include "LibBle.h"
#include "TFT_eSPI.h"
#include "icons.h"
#include "Free_Fonts.h"
#include "LibWifi.h"
#include "LibFlash.h"

/* Macros ------------------------------------------------------------------*/

#define STR_SHADOW_KEY_LED "LED"
#define STR_SHADOW_KEY_COLOR "COLOR"

#define DESIRED_LED_STATE 0
#define DESIRED_COLOR "WHITE"

/* Variables -----------------------------------------------------------------*/

extern const char aws_root_ca_pem[];

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

#define gray 0x6B6D


typedef enum
{
  DISPLAY_STATUS_OFF,
  DISPLAY_STATUS_NO_CHANGE,
  DISPLAY_STATUS_TOGGLE,
  DISPLAY_STATUS_ON,
} displayStatusCodes_et;

static uint8_t eventWIFIState = DISPLAY_STATUS_OFF;
static uint8_t eventBLEState = DISPLAY_STATUS_OFF;
static uint8_t eventMQTTState = DISPLAY_STATUS_OFF;
bool printStatus = false;

LibSystem SYS;
LibBle BLE;
Aws AWS;
LibWifi WIFI;
LibFlash FLASH;

systemInitConfig_st sysConfig = {0};

void app_init();
void app_loop();

int32_t gDesiredLedState_s32 = 0, gReportedLedState_s32 = 0;
char gDesiredColorStr[12] = DESIRED_COLOR, gReportedColorStr[12] = DESIRED_COLOR;

void classicShadowUpdateCallBack(uint8_t elementIndex_u8, const char *pKeyStr, const void *pValue);
void statusShadowUpdateCallBack(uint8_t elementIndex_u8, const char *pKeyStr, const void *pValue);

#define CLASSIC_SHADOW 0
#define STATUS_SHADOW 1
#define MAX_TYPES_OF_SHADOWS 2

const awsShadowElement_st classicShadowElements[] = {
    /* Shaodw Value type          Key         value          needToPublish   shadowUpdateType*/
    {SHADOW_VALUE_TYPE_STRING, "fw_ver", {.pStr = APP_VERSION}, true, SHADOW_UPDATE_TYPE_ALL},
    {SHADOW_VALUE_TYPE_INT, "LED", {.val_i32 = 0}, true, SHADOW_UPDATE_TYPE_ALL},
    {SHADOW_VALUE_TYPE_STRING, "COLOR", {.pStr = DESIRED_COLOR}, true, SHADOW_UPDATE_TYPE_ALL}};

const awsShadowElement_st statusShadowElements[] = {
    /* Shaodw Value type          Key     value     needToPublish shadowUpdateType*/
    {SHADOW_VALUE_TYPE_FLOAT, "s01", {.val_f32 = 3.1}, true, SHADOW_UPDATE_TYPE_ALL},
    {SHADOW_VALUE_TYPE_FLOAT, "s02", {.val_f32 = 3.1}, true, SHADOW_UPDATE_TYPE_ALL},
};

shadowConfigTable_st shadowTable[MAX_TYPES_OF_SHADOWS] = {
    {
      ptrShadowName : NULL, // NUll for classis shadow
      maxElementCount_u8 : (sizeof(classicShadowElements) / sizeof(classicShadowElements[0])),
      callbackHandler : classicShadowUpdateCallBack,
      pShadowElementsTable : classicShadowElements,
    },

    {
      ptrShadowName : "STATUS", // Named shadow
      maxElementCount_u8 : (sizeof(statusShadowElements) / sizeof(statusShadowElements[0])),
      callbackHandler : statusShadowUpdateCallBack,
      pShadowElementsTable : statusShadowElements,
    },
};

void setup()
{
  Serial.begin(115200);
  app_init();
  disp_init();
  pinMode(LED0_PIN, OUTPUT);
}

void loop()
{
  app_loop();
  delay(100);
}

void classicShadowUpdateCallBack(uint8_t elementIndex_u8, const char *pKeyStr, const void *pValue)
{
  printf("\nClassic shadow: %s", pKeyStr);
  if (elementIndex_u8 == 1) // 0-FV, 1-LED, 2-COLOR
  {
    gDesiredLedState_s32 = *(uint8_t *)pValue;
    printf("\nDelta update desired Led : %d", gDesiredLedState_s32);
  }
  else if (elementIndex_u8 == 2) // 0-FV, 1-LED, 2-COLOR
  {
    if ((strcmp((char *)pValue, "WHITE") == 0) || (strcmp((char *)pValue, "RED") == 0) || (strcmp((char *)pValue, "GREEN") == 0) || (strcmp((char *)pValue, "BLUE") == 0))
    {
      strcpy(gDesiredColorStr, (char *)pValue);
      printf("\ngDesiredColorStr:%s gReportedColorStr:%s", gDesiredColorStr, gReportedColorStr);
    }
  }
}

void statusShadowUpdateCallBack(uint8_t elementIndex_u8, const char *pKeyStr, const void *pValue)
{
  printf("\nStatus shadow: %s", pKeyStr);
  AWS.shadowUpdate(STATUS_SHADOW, (char *)pKeyStr, (void *)pValue, SHADOW_UPDATE_TYPE_REPORTED); // index 1->Status
}

void app_init()
{
  

  sysConfig.systemEventCallbackHandler = app_eventsCallBackHandler;
  sysConfig.pAppVersionStr = (char *)APP_VERSION;

  sysConfig.s_mqttClientConfig.maxPubMsgToStore_u8 = 5;
  sysConfig.s_mqttClientConfig.maxSubMsgToStore_u8 = 6;
  sysConfig.s_mqttClientConfig.maxSubscribeTopics_u8 = 5;
  sysConfig.s_mqttClientConfig.pRootCaStr = (char *)aws_root_ca_pem;

  if (SYS.init(&sysConfig))
  {
    BLE.start();
    SYS.start();

    if (SYS.getMode() == SYSTEM_MODE_NORMAL)
    {
      AWS.shadowRegister(shadowTable, MAX_TYPES_OF_SHADOWS);
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

void app_loop()
{
  static uint32_t nextPubTime = 0;
  static bool isUpdated = true;
  static int i=0;

  switch (SYS.getMode())
  {
  case SYSTEM_MODE_DEVICE_CONFIG:
    if (millis() > nextPubTime)
    {
      nextPubTime = millis();

      if (SYS.isDeviceRegistered())
      {
        printf("\nDevice successfully provisioned");
        provisionedScreen();
        delay(500);
        rebootingScreen();
        delay(2000);
      }
      else
      {
        printf("\nDevice is not provisioned");
        if (!eventBLEState)
        {
          advertisingScreen();
        }
        else if(EVENT_BLE_CONNECTED)
        {
          if(i==0)
          {
          appConnectedScreen();
          i=1;
          }
        }
        if(WIFI.getState() == STATE_WIFI_SCAN)
        {
          wifiScanningScreen();
        }
        else if(WIFI.getState() == STATE_WIFI_START)
        {
          wifiConnScreen();
        }
      }
    }
    printStatus = false;
    break;

  case SYSTEM_MODE_NORMAL:
    if (gDesiredLedState_s32 != gReportedLedState_s32)
    {
      gReportedLedState_s32 = gDesiredLedState_s32;
      digitalWrite(LED0_PIN, gDesiredLedState_s32);
      printf("\ngDesiredLedState_s32:%d gReportedLedState_s32:%d", gDesiredLedState_s32, gReportedLedState_s32);
      AWS.shadowUpdate(CLASSIC_SHADOW, STR_SHADOW_KEY_LED, &gReportedLedState_s32, SHADOW_UPDATE_TYPE_REPORTED);
      isUpdated = true;
    }
    else if (strcmp(gDesiredColorStr, gReportedColorStr) != 0)
    {
      strcpy(gReportedColorStr, gDesiredColorStr);
      printf("\ngDesiredColorStr:%s gReportedColorStr:%s", gDesiredColorStr, gReportedColorStr);
      AWS.shadowUpdate(CLASSIC_SHADOW, STR_SHADOW_KEY_COLOR, gReportedColorStr, SHADOW_UPDATE_TYPE_REPORTED);
      isUpdated = true;
    }
      // if (isUpdated == true || printStatus == true)
      // {
      //   aicon_status();
      //   shadowScreen(gReportedLedState_s32, gReportedColorStr);
      //   isUpdated = false;
      //   printStatus = false;
      // }
        shadowScreen(gReportedLedState_s32, gReportedColorStr);
        isUpdated = false;
        printStatus = false;
    break;

  default:
    break;
  }
  

}

void app_eventsCallBackHandler(systemEvents_et event_e)
{
  switch (event_e)
  {
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

  case EVENT_BLE_CONNECTED:
    eventBLEState = DISPLAY_STATUS_ON;
    printStatus = true;
    printf("\nEVENT_BLE_CONNECTED");
    break;
  case EVENT_BLE_AUTHENTICATED:
    printf("\nEVENT_BLE_AUTHENTICATED");
    break;
  case EVENT_BLE_DISCONNECTED:
    eventBLEState = DISPLAY_STATUS_OFF;
    printStatus = true;
    printf("\nEVENT_BLE_DISCONNECTED");
    break;

  default:
    break;
  }
}

void disp_init()
{
  tft.init();
  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);

  sprite.createSprite(320, 170);
  sprite.setSwapBytes(true);
  sprite.setTextColor(TFT_WHITE);
  sprite.setTextDatum(4);
  sprite.fillSprite(TFT_BLACK);
}

void drawHeaderFooter()
{
  sprite.setTextColor(TFT_SKYBLUE);
  sprite.setFreeFont(FF1);
  sprite.setTextSize(1);
  sprite.setCursor(10, 20);
  sprite.print("AWS IoT");
  sprite.drawRect(5, 137, 310, 26, TFT_WHITE);
  sprite.setTextColor(TFT_WHITE);
  sprite.drawString("BLE Provisioning", 170, 148);
  sprite.drawRect(0, 0, 320, 170, gray);
}

void bicon_status()
{
  static bool blinkState = false;

  if (!eventBLEState)
  {
    if (blinkState)
    {
      sprite.pushImage(290, 2, 25, 25, btDiscon_icon);
    }
    else
    {
      sprite.drawRect(290, 2, 25, 25, TFT_BLACK);
    }
  }
  else
  {
    sprite.pushImage(290, 2, 25, 25, bt_icon);
  }

  blinkState = !blinkState;
}

void aicon_status()
{
  static bool blinkState = false;

  if (!eventBLEState)
  {
    sprite.pushImage(290, 2, 25, 25, btDiscon_icon);
  }
  else
  {
    sprite.pushImage(290, 2, 25, 25, bt_icon);
  }

  if (!eventWIFIState)
  {
    if (blinkState)
    {
      sprite.pushImage(260, 2, 25, 25, wifiDiscon_icon);
    }
    else
    {
      sprite.drawRect(260, 2, 25, 25, TFT_BLACK);
    }
  }
  else
  {
    sprite.pushImage(260, 2, 25, 25, wifi_icon);
  }

  if (eventWIFIState==DISPLAY_STATUS_ON && !eventMQTTState)
  {
    if (blinkState)
    {
      sprite.pushImage(230, 4, 25, 25, awsDiscon_icon);
    }
    else
    {
      sprite.drawRect(230, 2, 25, 25, TFT_BLACK);
    }
  }
  else
  {
    sprite.pushImage(230, 4, 25, 25, awsDiscon_icon);
  }
  if(eventMQTTState==DISPLAY_STATUS_ON)
  {
   sprite.pushImage(230, 4, 25, 25, awsCon_icon);
  }

  blinkState = !blinkState;
}

void provisioningScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF1);
  sprite.drawString("Provisioning...", 170, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void provisionedScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF1);
  sprite.drawString("Registration Successful", 170, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void appConnectedScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF1);
  sprite.drawString("App Connected", 170, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void wifiScanningScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF1);
  sprite.drawString("Wifi Scanning", 170, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void wifiConnScreen()
{
  static wifiCred_st  s_wifiCred = {0};
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF1);
  sprite.drawString("Connected to Wifi", 170, 60);
  // sprite.drawString(String(sysConfig.pWifiSsidStr), 170, 100);
  // sprite.drawString((FLASH_getDeviceConfig(DEVICE_CONFIG_WIFI, (void *)&s_wifiCred)), 170, 100);
  sprite.drawString(String(WIFI.getSSID()), 170, 90);

  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void rebootingScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF1);
  sprite.drawString("Rebooting...", 170, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void advertisingScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF1);
  sprite.drawString("Advertising", 160, 55);
  // int deviceNameLength = sprite.textWidth(SYS.getDeviceName());
  // sprite.drawRect(90,70, deviceNameLength+10, 26, TFT_WHITE);
  sprite.drawString(String(SYS.getDeviceName()), 160, 80);
  drawHeaderFooter();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void openScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  if (SYS.isDeviceRegistered())
  {
    provsScreen();
  }
  aicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void provsScreen()
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF1);
  sprite.drawString("Provision Successful", 170, 80);
  thingVersion();
  bicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void thingVersion()
{
  sprite.setTextColor(TFT_SKYBLUE);
  sprite.setFreeFont(FF1);
  sprite.setTextSize(1);
  sprite.setCursor(10, 20);
  sprite.print(AWS.getThingName());
  int deviceNameLength = sprite.textWidth(AWS.getThingName());
  sprite.setFreeFont(FF0);
  // sprite.setCursor(deviceNameLength + 15, 15);
  // sprite.print(APP_VERSION);
  sprite.drawRect(5, 137, 310, 26, TFT_WHITE);
  sprite.setFreeFont(FF1);
  sprite.setTextColor(TFT_WHITE);
  sprite.drawString("Buildstorm Technologies", 170, 148);
  sprite.drawRect(0, 0, 320, 170, gray);
}

void afterProvisioned(String message)
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF1);
  sprite.drawString(message, 160, 80);
  thingVersion();
  aicon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
}

void shadowScreen(int32_t ledState, char *txtColor)
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF1);
  int color = stringToColor(txtColor);
  sprite.setTextColor(color);
  sprite.drawString("fw_ver :", 50, 60);
  sprite.drawString("LED    :", 50, 90);
  sprite.drawString("COLOR  :", 50, 115);
  sprite.drawString(String(APP_VERSION), 145, 60);
  sprite.drawString(String(ledState), 140, 90);
  sprite.drawString(txtColor, 140, 115);
  thingVersion();
  aicon_status();
  sprite.pushSprite(0, 0);
  delay(500);
}

int32_t stringToColor(char *colorStr)
{
  int32_t tftColor = {0};

  if (strcmp(colorStr, "RED") == 0)
  {
    tftColor = TFT_RED;
  }
  else if (strcmp(colorStr, "GREEN") == 0)
  {
    tftColor = TFT_DARKGREEN;
  }
  else if (strcmp(colorStr, "BLUE") == 0)
  {
    tftColor = TFT_BLUE;
  }
  else
  {
    tftColor = TFT_WHITE;
  }

  return tftColor;
}

