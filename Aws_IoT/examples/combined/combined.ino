/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file combined.ino
 * \brief main entry of the application.
 */

#include "app_config.h"
#include "LibSystem.h"
#include "LibAws.h"
#include "LibBle.h"
#include "LibJson.h"
#include "LibUtils.h"

/* Macros ------------------------------------------------------------------*/

#define STR_SHADOW_KEY_LED "LED"
#define STR_SHADOW_KEY_COLOR "COLOR"

#define DESIRED_LED_STATE 0
#define DESIRED_COLOR "RED"

#define CLASSIC_SHADOW 0
#define MAX_TYPES_OF_SHADOWS 1

/* Objects -----------------------------------------------------------------*/
LibSystem SYS;
LibBle BLE;
Aws AWS;
LibJson JSN;
LibUtils Utils;

/* Variables -----------------------------------------------------------------*/
extern const char aws_root_ca_pem[];
extern const char certificate_pem_crt[];
extern const char private_pem_key[];

int32_t gDesiredLedState_s32 = 0, gReportedLedState_s32 = 0;
char gDesiredColorStr[12] = DESIRED_COLOR, gReportedColorStr[12] = DESIRED_COLOR;

static char gJobIdStr[LENGTH_JOB_ID] = {0};
static char gJobDocumentStr[LENGTH_JOB_DOCUMENT] = {0};
static bool gJobReceived_b8 = false;

const awsShadowElement_st classicShadowElements[] = {
    /* Shaodw Value type          Key         value          needToPublish   shadowUpdateType*/
    {SHADOW_VALUE_TYPE_STRING, "fw_ver", {.pStr = APP_VERSION}, true, SHADOW_UPDATE_TYPE_ALL},
    {SHADOW_VALUE_TYPE_INT, "LED", {.val_i32 = 0}, true, SHADOW_UPDATE_TYPE_ALL},
    {SHADOW_VALUE_TYPE_STRING, "COLOR", {.pStr = "RED"}, true, SHADOW_UPDATE_TYPE_ALL}};

void classicShadowUpdateCallBack(uint8_t elementIndex_u8, const char *pKeyStr, const void *pValue);

shadowConfigTable_st shadowTable[MAX_TYPES_OF_SHADOWS] =
    {
        {
            ptrShadowName : NULL, // NUll for classis shadow
            maxElementCount_u8 : (sizeof(classicShadowElements) / sizeof(classicShadowElements[0])),
            callbackHandler : classicShadowUpdateCallBack,
            pShadowElementsTable : classicShadowElements,
        }};

/* Forward Function DEclaration -----------------------------------------------------------------*/
void app_init();
void app_loop();

void setup()
{
    Serial.begin(115200);
    app_init();
    pinMode(LED0_PIN, OUTPUT);
}

void loop()
{
    app_loop();
    delay(100);
}

void app_init()
{
    systemInitConfig_st sysConfig = {0};

    sysConfig.systemEventCallbackHandler = app_eventsCallBackHandler;
    sysConfig.pAppVersionStr = (char *)APP_VERSION;
  
    /* Wifi Configuration*/
    sysConfig.pWifiSsidStr = (char *)TEST_WIFI_SSID;
    sysConfig.pWifiPwdStr = (char *)TEST_WIFI_PASSWORD;

    /* Aws Configuration */
    sysConfig.s_mqttClientConfig.maxPubMsgToStore_u8 = 5;
    sysConfig.s_mqttClientConfig.maxSubMsgToStore_u8 = 6;
    sysConfig.s_mqttClientConfig.maxSubscribeTopics_u8 = 5;
    sysConfig.s_mqttClientConfig.maxJobs_u8 = 2;

    sysConfig.s_mqttClientConfig.pThingNameStr = AWS_IOT_THING_NAME;
    sysConfig.s_mqttClientConfig.pHostNameStr = (char *)AWS_IOT_MQTT_HOST;
    sysConfig.s_mqttClientConfig.port_u16 = AWS_IOT_MQTT_PORT;
    sysConfig.s_mqttClientConfig.pClaimTemplateStr = (char *)AWS_PROVISION_TEMPLATE_NAME;
    sysConfig.s_mqttClientConfig.pRootCaStr = (char *)aws_root_ca_pem;
    sysConfig.s_mqttClientConfig.pClaimCertStr = (char *)certificate_pem_crt;
    sysConfig.s_mqttClientConfig.pClaimPrivateKeyStr = (char *)private_pem_key;

    // sysConfig.s_mqttClientConfig.pThingCertStr = (char *)certificate_pem_crt;
    // sysConfig.s_mqttClientConfig.pThingPrivateKeyStr = (char *)private_pem_key;

    if (SYS.init(&sysConfig))
    {
        BLE.start();
        SYS.start();

        if (SYS.getMode() == SYSTEM_MODE_NORMAL)
        {
            AWS.subscribe((char *)TEST_AWS_TOPIC_SUBSCRIBE, QOS0_AT_MOST_ONCE);
            if (AWS.shadowRegister(shadowTable, MAX_TYPES_OF_SHADOWS))
            {
                printf("\n shadow reg success");
            }
            else
            {
                printf("\n shadow reg failed");
            }

            if (AWS.jobRegister("blink", 0, app_jobHandlerLed))
            {
                printf("\n blink job reg success");
            }
            else
            {
                printf("\n blink job reg failed");
            }
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
    static uint32_t count_u32 = 10;

    switch (SYS.getMode())
    {
    case SYSTEM_MODE_DEVICE_CONFIG:
        if (millis() > nextPubTime)
        {
            nextPubTime = millis() + 5000;

            if (SYS.isDeviceRegistered())
            {
                printf("\n Device successfully provisioned");
            }
            else
            {
                printf("\n Device is not provisioned");
            }
        }
        break;

    case SYSTEM_MODE_NORMAL:
        if (AWS.isConnected())
        {
            mqttMsg_st s_mqttMsg = {0};
            if (AWS.read(&s_mqttMsg))
            {
                printf("\n Received: [%s]:%s", s_mqttMsg.topicStr, s_mqttMsg.payloadStr);
            }

            if ((millis() > nextPubTime) && count_u32)
            {
                s_mqttMsg.payloadLen_u16 = sprintf(s_mqttMsg.payloadStr, "{\"counter\": %d}", count_u32--);
                strcpy(s_mqttMsg.topicStr, TEST_AWS_TOPIC_PUBLISH);
                s_mqttMsg.qos_e = QOS0_AT_MOST_ONCE;
                AWS.publish(&s_mqttMsg);
                nextPubTime = millis() + 5000;
                printf("\n %s\n", s_mqttMsg.payloadStr);
            }

            if (gDesiredLedState_s32 != gReportedLedState_s32)
            {
                gReportedLedState_s32 = gDesiredLedState_s32;
                digitalWrite(LED0_PIN, gDesiredLedState_s32);
                printf("\n gDesiredLedState_s32:%d gReportedLedState_s32:%d", gDesiredLedState_s32, gReportedLedState_s32);
                AWS.shadowUpdate(CLASSIC_SHADOW, STR_SHADOW_KEY_LED, &gReportedLedState_s32, SHADOW_UPDATE_TYPE_REPORTED);
            }
            else if (strcmp(gDesiredColorStr, gReportedColorStr) != 0)
            {
                strcpy(gReportedColorStr, gDesiredColorStr);
                printf("\n gDesiredColorStr:%s gReportedColorStr:%s", gDesiredColorStr, gReportedColorStr);
                AWS.shadowUpdate(CLASSIC_SHADOW, STR_SHADOW_KEY_COLOR, gReportedColorStr, SHADOW_UPDATE_TYPE_REPORTED);
            }

            app_jobCheck();
        }
        break;

    case SYSTEM_MODE_OTA:
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
        printf("\n EVENT_WIFI_CONNECTED");
        break;
    case EVENT_WIFI_DISCONNECTED:
        printf("\n EVENT_WIFI_DISCONNECTED");
        break;

    case EVENT_MQTT_CONNECTED:
        printf("\n EVENT_AWS_CONNECTED");
        break;
    case EVENT_MQTT_DISCONNECTED:
        printf("\n EVENT_AWS_DISCONNECTED");
        break;

    case EVENT_BLE_CONNECTED:
        printf("\n EVENT_BLE_CONNECTED");
        break;
    case EVENT_BLE_AUTHENTICATED:
        printf("\n EVENT_BLE_AUTHENTICATED");
        break;
    case EVENT_BLE_DISCONNECTED:
        printf("\n EVENT_BLE_DISCONNECTED");
        break;

    default:
        break;
    }
}

void classicShadowUpdateCallBack(uint8_t elementIndex_u8, const char *pKeyStr, const void *pValue)
{
    printf("\n Classic shadow: %s", pKeyStr);
    if (elementIndex_u8 == 1) // 0-FV, 1-LED, 2-COLOR
    {
        gDesiredLedState_s32 = *(uint8_t *)pValue;
        printf("\n Delta update desired Led : %d", gDesiredLedState_s32);
    }
    else if (elementIndex_u8 == 2) // 0-FV, 1-LED, 2-COLOR
    {
        if ((strcmp((char *)pValue, "WHITE") == 0) ||
            (strcmp((char *)pValue, "RED") == 0) ||
            (strcmp((char *)pValue, "GREEN") == 0) ||
            (strcmp((char *)pValue, "BLUE") == 0))
        {
            strcpy(gDesiredColorStr, (char *)pValue);
            printf("\n gDesiredColorStr:%s gReportedColorStr:%s", gDesiredColorStr, gReportedColorStr);
        }
    }
}

jobsStatus_et app_jobHandlerLed(const job_st *ps_job)
{
    printf("\n [app_jobHandlerLed] %s : %s", ps_job->idStr, ps_job->documentStr);

    // copy gJobIdStr & gJobDocumentStr
    strcpy(gJobIdStr, ps_job->idStr);
    strcpy(gJobDocumentStr, ps_job->documentStr);

    // handle the operation in application task
    gJobReceived_b8 = true;

    return JOB_STATUS_IN_PROGRESS;
}

void app_jobCheck()
{
    // job received? perform operation
    if (gJobReceived_b8)
    {
        printf("\n app_jobCheck job received");
        gJobReceived_b8 = 0;

        uint32_t countValue_u32 = 0;
        uint32_t onTime_u32 = 0;
        uint32_t offTime_u32 = 0;
        char countStr[8] = {0};
        char onTimeStr[8] = {0};
        char offTimeStr[8] = {0};

#define MAX_KEYS 3
        // variable for extracting json key-value pair
        tagStructure_st jobKeyValuePair[MAX_KEYS] = {
            {(char *)"count", countStr},
            {(char *)"onTime", onTimeStr},
            {(char *)"offTime", offTimeStr}};

        // parse job document to initialize count value
        if (JSN.extractString(gJobDocumentStr, jobKeyValuePair, MAX_KEYS, 0))
        {
            countValue_u32 = Utils.getNumI32(countStr);
            onTime_u32 = Utils.getNumI32(onTimeStr);
            offTime_u32 = Utils.getNumI32(offTimeStr);

            printf("\n blink %d times => ON:%dms OFF:%dms", countValue_u32, onTime_u32, offTime_u32);
            countValue_u32 = countValue_u32 & 0x0F; // Limit to 15 loops
            for (uint32_t i = 0; i < countValue_u32; i++)
            {
                if (onTime_u32)
                {
                    digitalWrite(LED0_PIN, HIGH);
                    delay(onTime_u32);
                }
                if (offTime_u32)
                {
                    digitalWrite(LED0_PIN, LOW);
                    delay(offTime_u32);
                }
            }

            AWS.jobUpdateStatus(gJobIdStr, JOB_STATUS_SUCCESSED);
        }
        else
        {
            printf("\n Error: Invalid job document: %s", gJobDocumentStr);
            AWS.jobUpdateStatus(gJobIdStr, JOB_STATUS_FAILED);
        }
    }
}
