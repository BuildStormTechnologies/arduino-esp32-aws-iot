/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file app_config.h
 * \brief app_config header file.
 *
 * The app_config Contains macros for the application.
 *
 */

#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define TEST_WIFI_SSID "Your SSID"
#define TEST_WIFI_PASSWORD "wifi password"
#define AWS_IOT_THING_NAME "thing name"
#define AWS_IOT_MQTT_HOST "Your host"
#define AWS_IOT_MQTT_PORT 8883
#define APP_VERSION "1.0.0"

#define LED0_PIN 2

#ifdef __cplusplus
}
#endif

#endif //_APP_CONFIG_H_
