/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file 03_Shadow.ino
 * \brief 03_Shadow.ino file.
 *
 * The 03_Shadow.ino main entry of the application.
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

#define STR_SHADOW_KEY_LED "LED"
#define STR_SHADOW_KEY_COLOR "COLOR"

#define DESIRED_LED_STATE 0
#define DESIRED_COLOR "RED"

/* Variables -----------------------------------------------------------------*/
extern const char aws_root_ca_pem[];
extern const char certificate_pem_crt[];
extern const char private_pem_key[];

LibSystem SYS;
Aws AWS;

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

void setup();
void loop();

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
    {SHADOW_VALUE_TYPE_STRING, "COLOR", {.pStr = "RED"}, true, SHADOW_UPDATE_TYPE_ALL}};

const awsShadowElement_st statusShadowElements[] = {
    /* Shaodw Value type          Key     value     needToPublish shadowUpdateType*/
    {SHADOW_VALUE_TYPE_FLOAT, "s01", {.val_f32 = 3.1}, true, SHADOW_UPDATE_TYPE_ALL},
    {SHADOW_VALUE_TYPE_FLOAT, "s02", {.val_f32 = 3.1}, true, SHADOW_UPDATE_TYPE_ALL},
};

shadowConfigTable_st shadowTable[MAX_TYPES_OF_SHADOWS] =
    {
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
  delay(200);
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
    if ((strcmp((char *)pValue, "WHITE") == 0) ||
        (strcmp((char *)pValue, "RED") == 0) ||
        (strcmp((char *)pValue, "GREEN") == 0) ||
        (strcmp((char *)pValue, "BLUE") == 0))
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

  systemInitConfig_st sysConfig = {0};

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

  if (SYS.init(&sysConfig))
  {
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
  static bool isUpdated = true;

  switch (SYS.getMode())
  {

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
    break;

  default:
    break;
  }
  if (isUpdated == true || printStatus == true)
  {
    icon_status();
    shadowScreen(gReportedLedState_s32, gReportedColorStr);
    isUpdated = false;
    printStatus = false;
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
  sprite.setFreeFont(FF5);
  sprite.setTextSize(1);
  sprite.setCursor(10, 20);
  sprite.print("AWS IoT");
  sprite.drawRect(2, 142, 316, 26, TFT_WHITE);
  sprite.setTextColor(TFT_WHITE);
  sprite.drawString("Shadow Update", 170, 152);
  sprite.drawRect(0, 0, 320, 170, gray);
}

void icon_status()
{

  if (eventBLEState == DISPLAY_STATUS_ON)
  {
    sprite.pushImage(230, 2, 25, 25, bt_icon);
  }
  else
  {
    sprite.pushImage(230, 2, 25, 25, btDiscon_icon);
  }
  if (eventWIFIState == DISPLAY_STATUS_ON)
  {
    sprite.pushImage(290, 2, 25, 25, wifi_icon);
  }
  else
  {
    sprite.pushImage(290, 2, 25, 25, wifiDiscon_icon);
  }
  if (eventMQTTState == DISPLAY_STATUS_ON)
  {
    sprite.pushImage(260, 4, 25, 25, awsCon_icon);
  }
  else
  {
    sprite.pushImage(260, 4, 25, 25, awsDiscon_icon);
  }
}

void shadowScreen(int32_t ledState, char *txtColor)
{
  sprite.fillSprite(TFT_BLACK);
  sprite.drawRect(0, 0, 320, 170, gray);
  sprite.setFreeFont(FF5);
  int color = stringToColor(txtColor);
  sprite.setTextColor(color);
  sprite.drawString("Shadow Status", 170, 60);
  sprite.drawString("LED  :", 40, 90);
  sprite.drawString("COLOR:", 40, 115);
  sprite.drawString(String(ledState), 120, 90);
  sprite.drawString(txtColor, 120, 115);
  drawHeaderFooter();
  icon_status();
  sprite.pushSprite(0, 0);
  delay(1000);
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