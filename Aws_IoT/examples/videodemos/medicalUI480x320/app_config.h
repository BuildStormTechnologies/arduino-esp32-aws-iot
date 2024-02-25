/*******************************************************************************
 * Copyright (c) 2021-2024, Buildstorm Pvt Ltd
 *
 * Filename:     app_config.h
 *
 * Description:
 *               Contains macros for the application
 *******************************************************************************/

#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define TASK_APP_PRIORITY 4
#define TASK_APP_STACK_SIZE (2 * 1024) // reduce it to 4096

#define APP_VERSION "v0.1.0"

#define TEST_WIFI_SSID "Your SSID"
#define TEST_WIFI_PASSWORD "wifi password"

#define AWS_THING_NAME "348518B83A28"
#define AWS_IOT_MQTT_HOST "Your host"
#define AWS_IOT_MQTT_PORT 8883
#define AWS_PROVISION_TEMPLATE_NAME "your provisioning template"
#define APP_VERSION "1.0.0"

#define TEST_AWS_TOPIC_PUBLISH "testPub/ESP32"
#define TEST_AWS_TOPIC_SUBSCRIBE "testSub/ESP32"


#ifdef __cplusplus
}
#endif

#endif
