 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibWifiTypes.h
 * \brief WifiTypes library header file.
 *
 * 
 * This file defines the structure and enums used by the WiFi library.
 * 
 * The libraries have been tested on the ESP32 modules. 
 * Buildstorm explicitly denies responsibility for any hardware failures 
 * arising from the use of these libraries, whether directly or indirectly. 
 * 
 * EULA LICENSE:
 * This library is licensed under end user license EULA agreement.
 * The EULA is available at https://buildstorm.com/eula/
 * For any support contact us at hello@buildstorm.com
 * 
 */

#ifndef __LIB_WIFI_TYPES_H__
#define __LIB_WIFI_TYPES_H__

/* Includes ------------------------------------------------------------------*/

#include "LibConfig.h"

/* Macros -----------------------------------------------------------------*/

#define ENABLE_WIFI_VERIFICATION TRUE

/**
 * @enum wifiStates_et
 * An enum to represnet WiFi states
 */
typedef enum
{
    STATE_WIFI_IDLE,                    /*!< Idle state */
    STATE_WIFI_SCAN,                    /*!< Scanning for SSIDs */
    STATE_WIFI_START,                   /*!< Start WiFi */
    STATE_WIFI_START_SERVER,            /*!< Start WiFi, AP Server */
    STATE_WIFI_DISCONNECTED,            /*!< WiFi disconnected */
    STATE_WIFI_CONNECTED,               /*!< WiFi connected */
    STATE_WIFI_SERVER_STA_CONNECTED,    /*!< WiFi connected */
    STATE_WIFI_SERVER_STA_DISCONNECTED, /*!< WiFi connected */
    STATE_WIFI_MAX,                     /*!< Total number of states */
} wifiStates_et;

/**
 * @brief WiFi credentials
 */
typedef struct
{
    char ssid[LENGTH_WIFI_CRED_SSID];         /*!< WiFi SSID */
    char password[LENGTH_WIFI_CRED_PASSWORD]; /*!< WiFi Password */
} wifiCred_st;

#endif //__LIB_WIFI_TYPES_H__