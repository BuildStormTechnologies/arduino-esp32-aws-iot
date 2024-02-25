 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibHttpTypes.h
 * \brief HttpTypes library header file.
 *
 * 
 * This file defines the structure and enums used by the HTTP library. 
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

#ifndef __LIB_HTTP_TYPES_H__
#define __LIB_HTTP_TYPES_H__

#include "LibJsonTypes.h"

#define HTTP_TIMEOUT 10000
#define MAX_HTTP_HEADERS 3
#define HTTP_GET_REQ_RETRY_LIMIT 3

/**
 * @enum httpReq_et
 * An enum that represents HTTP requests supported by the library.
 */
typedef enum
{
    HTTP_REQUEST_GET = 0,     /*!< HTTP GET Method */
    HTTP_REQUEST_POST,        /*!< HTTP POST Method */
    HTTP_REQUEST_PUT,         /*!< HTTP PUT Method */
    HTTP_REQUEST_PATCH,       /*!< HTTP PATCH Method */
    HTTP_REQUEST_DELETE,      /*!< HTTP DELETE Method */
    HTTP_REQUEST_HEAD,        /*!< HTTP HEAD Method */
    HTTP_REQUEST_NOTIFY,      /*!< HTTP NOTIFY Method */
    HTTP_REQUEST_SUBSCRIBE,   /*!< HTTP SUBSCRIBE Method */
    HTTP_REQUEST_UNSUBSCRIBE, /*!< HTTP UNSUBSCRIBE Method */
    HTTP_REQUEST_OPTIONS,     /*!< HTTP OPTIONS Method */
    HTTP_REQUEST_MAX,
} httpReq_et;

/**
 * @enum httpStates_et
 * An enum that represents HTTP states of the library.
 */
typedef enum
{
    STATE_HTTP_IDLE,                   /*!< HTTP idle state */
    STATE_HTTP_START,                  /*!< HTTP start state */
    STATE_HTTP_SET_REQUEST_HEADER,     /*!< Set HTTP request header */
    STATE_HTTP_SET_RESPONSE_HEADER,    /*!< Set HTTP response header */
    STATE_HTTP_CONNECT,                /*!< Initiate connection */
    STATE_HTTP_SET_URL,                /*!< Set the URL for HTTP request*/
    STATE_HTTP_GET_REQUEST,            /*!< HTTP idle state */
    STATE_HTTP_POST_REQUEST,           /*!< Send HTTP POST request */
    STATE_HTTP_POST_DATA,              /*!< Send HTTP POST data */
    STATE_HTTP_WAIT_FOR_POST_RESPONSE, /*!< Wait for HTTP response */
    STATE_HTTP_READ,                   /*!< Read HTTP response */
    STATE_HTTP_WAIT_FOR_FILE_HEADER,   /*!< Wait for HTTP file header */
    STATE_HTTP_DOWNLOAD_FILE,          /*!< File download in progress */
    STATE_HTTP_DOWNLOAD_COMPLETE,      /*!< File download completed */
    STATE_HTTP_RETRY,                  /*!< Retry last operation */
    STATE_HTTP_TIMEOUT,                /*!< Timeout occured */
    STATE_HTTP_FAILED,                 /*!< HTTP operation failed */
    STATE_HTTP_CLOSE,                  /*!< Close HTTP connection */
    STATE_HTTP_MAX                     /*!< Total number of HTTP states */
} httpStates_et;

/**
 * @brief An HTTP Configuration structure.
 */
typedef struct
{
    httpReq_et reqType_e;                         /*!< HTTP request type */
    tagStructure_st as_headers[MAX_HTTP_HEADERS]; /*!< HTTP headers */
    char *pUrlStr;                                /*!< HTTP request URL */
    char *pPayloadStr;                            /*!< HTTP payload as string */
    bool header_b8;
} httpConfig_st;

#endif //__LIB_HTTP_TYPES_H__