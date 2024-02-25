 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibBleTypes.h
 * \brief BleTypes library header file.
 *
 * This file defines the structure and enums used by the BLE library.
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

#ifndef __LIB_BLE_TYPES_H__
#define __LIB_BLE_TYPES_H__

/* Includes ------------------------------------------------------------------*/

#include "LibConfig.h"

#define MAX_BLE_USER_PACKETS 2
#define SIZE_BLE_USER_PACKET_NAME 4
#define SIZE_BLE_USER_PACKET_DATA 128

typedef enum
{
    STATE_BLE_IDLE,
    STATE_BLE_DISCONNECTED,
    STATE_BLE_CONNECTED,
    STATE_BLE_AUTHENTICATED,
    STATE_BLE_MAX
} bleState_et;

typedef enum
{
    BLE_OKAY,
    BLE_BUFF_EMPTY,
    BLE_BUFF_EXCEEDS_LIMIT,
    BLE_WRITE_FULL,
    BLE_WRITE_FAILED,
    BLE_DISCONNECTED,
    BLE_ERROR_MAX,
} bleWrtStatus_et;

typedef enum
{
    BLE_RESP_SUCCESS,
    BLE_RESP_IN_PROGRESS,
    BLE_RESP_CONFIG_DONE,
    BLE_RESP_ACCESS_DENIED,
    BLE_RESP_TIMEOUT,
    BLE_RESP_MISSING_ELEMENTS,
    BLE_RESP_WIFI_WRONG_PWD,
    BLE_RESP_WIFI_WRONG_SSID,
    BLE_RESP_INVALID_RQST,
    BLE_RESP_MISSING_PACKETS,
    BLE_RESP_CRC_FAILED,
    BLE_RESP_DEVICE_BUSY,
    BLE_RESP_FAILED_UNKNOWN_REASON,
    BLE_RESP_DEV_REG_RQST = 55,
    BLE_RESP_INVALID_JSON = 99,
    BLE_NO_RESPONSE = 101,
} bleResponseCodes_et;

typedef void (*bleCallBack_t)(const char *pPacketStr, const char *pDataStr);

#endif //__LIB_BLE_TYPES_H__