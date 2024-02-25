/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file 04_Jobs.ino
 * \brief main entry of the application.
 */

#include "app_config.h"
#include "LibSystem.h"
#include "LibAws.h"
#include "LibJson.h"
#include "LibUtils.h"

LibSystem SYS;
Aws AWS;
LibJson JSN;
LibUtils Utils;

extern const char aws_root_ca_pem[];
extern const char certificate_pem_crt[];
extern const char private_pem_key[];

static char gJobIdStr[LENGTH_JOB_ID] = {0};
static char gJobDocumentStr[LENGTH_JOB_DOCUMENT] = {0};
static bool gJobReceived_b8 = 0;

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
    sysConfig.s_mqttClientConfig.pRootCaStr = (char *)aws_root_ca_pem;
    sysConfig.s_mqttClientConfig.pThingCertStr = (char *)certificate_pem_crt;
    sysConfig.s_mqttClientConfig.pThingPrivateKeyStr = (char *)private_pem_key;

    if (SYS.init(&sysConfig))
    {
        SYS.start();

        if (SYS.getMode() == SYSTEM_MODE_NORMAL)
        {
            if (AWS.jobRegister("blink", 0, app_jobHandlerLed))
            {
                printf("\nblink job reg success");
            }
            else
            {
                printf("\nblink job reg failed");
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
    switch (SYS.getMode())
    {

    case SYSTEM_MODE_NORMAL:
        if (AWS.isConnected())
        {
            app_jobCheck();
        }
        break;

    default:
        break;
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

void app_eventsCallBackHandler(systemEvents_et event_e)
{
    switch (event_e)
    {
    case EVENT_WIFI_CONNECTED:
        printf("\nEVENT_WIFI_CONNECTED");
        break;
    case EVENT_WIFI_DISCONNECTED:
        printf("\nEVENT_WIFI_DISCONNECTED");
        break;

    case EVENT_MQTT_CONNECTED:
        printf("\nEVENT_AWS_CONNECTED");
        break;
    case EVENT_MQTT_DISCONNECTED:
        printf("\nEVENT_AWS_DISCONNECTED");
        break;

    default:
        break;
    }
}
