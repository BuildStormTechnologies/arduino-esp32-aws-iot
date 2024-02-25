/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file 02_ThingPubSub.ino
 * \brief main entry of the application.
 */

#include "app_config.h"
#include "LibSystem.h"
#include "LibAws.h"

extern const char aws_root_ca_pem[];
extern const char certificate_pem_crt[];
extern const char private_pem_key[];

LibSystem SYS;
Aws AWS;

void app_init();
void app_loop();

void setup()
{
    Serial.begin(115200);
    app_init();
}

void loop()
{
    app_loop();
    delay(200);
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
    
    sysConfig.s_mqttClientConfig.pHostNameStr = (char *)AWS_IOT_MQTT_HOST;
    sysConfig.s_mqttClientConfig.pThingNameStr = AWS_IOT_THING_NAME;
    sysConfig.s_mqttClientConfig.port_u16 = AWS_IOT_MQTT_PORT;
    sysConfig.s_mqttClientConfig.pRootCaStr = (char *)aws_root_ca_pem;
    sysConfig.s_mqttClientConfig.pThingCertStr = (char *)certificate_pem_crt;
    sysConfig.s_mqttClientConfig.pThingPrivateKeyStr = (char *)private_pem_key;

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

void app_loop()
{

    static uint32_t nextPubTime = 0;
    static uint32_t count_u32 = 100;

    if (SYS.getMode() == SYSTEM_MODE_NORMAL)
    {
        if (AWS.isConnected())
        {
            mqttMsg_st s_mqttMsg = {0};
            if (AWS.read(&s_mqttMsg))
            {
                printf("\n Received: [%s]:%s", s_mqttMsg.topicStr, s_mqttMsg.payloadStr);
            }
            if ((millis() > nextPubTime))
            {

                s_mqttMsg.payloadLen_u16 = sprintf(s_mqttMsg.payloadStr, "{\"Count\": %d}", count_u32++);
                strcpy(s_mqttMsg.topicStr, TEST_AWS_TOPIC_PUBLISH);
                s_mqttMsg.qos_e = QOS0_AT_MOST_ONCE;
                AWS.publish(&s_mqttMsg);
                nextPubTime = millis() + 5000;
                printf("\n %s\n", s_mqttMsg.payloadStr);
            }
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
