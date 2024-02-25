 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibAwsTypes.h
 * \brief LibAwsTypes library header file.
 *
 * 
 * This file defines the structure and enums used by the Aws library.
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

#ifndef __LIB_AWS_TYPES_H__
#define __LIB_AWS_TYPES_H__

#include "LibConfig.h"
#include "LibMsgTypes.h"

/**
 * @enum awsIotStates_et
 * An enum that represents states of AWS module.
 * It will be in one of the following modes.
 */
typedef enum
{
    STATE_AWS_IDLE,             /*!< AWS idle state */
    STATE_AWS_INIT,             /*!< AWS init state */
    STATE_AWS_START,            /*!< AWS start state */
    STATE_AWS_PROVISIONING,     /*!< AWS device provisioning in progress */
    STATE_AWS_CONNECTED,        /*!< Device is connected to AWS */
    STATE_AWS_DISCONNECTED,     /*!< Device is disconnected from AWS */
    STATE_AWS_SUSPENDED,        /*!< AWS module is in suspended state */
    STATE_AWS_RESUME,           /*!< Resumes AWS module from suspended state */
    STATE_AWS_PROVISION_FAILED, /*!< AWS device provisioning has failed */
    STATE_AWS_MAX               /*!< Total number of AWS states */
} awsIotStates_et;

/**
 * @brief Subscribe message callback function type. The application should
 * define the callback function
 */
typedef void (*awsSubscribeCallback_t)(const char *pTopic, const char *pPayload);

/**
 * @brief AWS configuration structure used by the library.
 */
typedef struct
{
    char *pHostNameStr;                              /*!< AWS IoT Endpoint */
    char *pRootCaStr;                                /*!< Root CA certificate string */
    char *pThingCertStr;                             /*!< Thing ceritificate string */
    char *pThingPrivateKeyStr;                       /*!< Thing Private key string */
    char *pClaimCertStr;                             /*!< Claim certificate string */
    char *pClaimPrivateKeyStr;                       /*!< Claim Private key string */
    char *pClaimTemplateStr;                         /*!< Provisioning template name */
    const char *pThingNameStr;                       /*!< AWS IoT thing name */
    uint16_t port_u16;                               /*!< AWS IoT port number */
    uint8_t maxPubMsgToStore_u8;                     /*!< Number of publish messaged stored in Ringbuffer */
    uint8_t maxSubMsgToStore_u8;                     /*!< Number of Subscribe messages stored in Ringbuffer */
    uint8_t maxSubscribeTopics_u8;                   /*!< Number of Subscribe topics supported */
    uint8_t maxJobs_u8;                              /*!< Number of jobs supported */
    awsSubscribeCallback_t subscribeCallbackHandler; /*!< Callback function for subscribe messages */
} mqttClientConfig_st;

/**
 * @enum shadowValueType_et
 * An enum that represents all possible shadow value types.
 */
typedef enum
{
    SHADOW_VALUE_TYPE_INT,    /*!< Shadow value type as integer */
    SHADOW_VALUE_TYPE_FLOAT,  /*!< Shadow value type as float */
    SHADOW_VALUE_TYPE_STRING, /*!< Shadow value type as string */
    SHADOW_VALUE_TYPE_MAX,    /*!< Total number of shadow value types */
} shadowValueType_et;

/**
 * @enum shadowUpdateType_et
 * An enum that represents all possible shadow update types.
 */
typedef enum
{
    SHADOW_UPDATE_TYPE_DESIRED,  /*!< Update Desired state */
    SHADOW_UPDATE_TYPE_REPORTED, /*!< Update Reported state */
    SHADOW_UPDATE_TYPE_ALL,      /*!< Update both Desired & Reported states */
    SHADOW_UPDATE_TYPE_MAX,      /*!< Total number for update types */
} shadowUpdateType_et;

/**
 * @struct value_st
 * This is a shadow value type.
 */
typedef struct
{
    int32_t val_i32;  /*!< Value of type int32_t */
    float val_f32;    /*!< Value of type float */
    const char *pStr; /*!< Value of type string */
} value_st;

/**
 * @brief AWS Shadow structure used by the library to update classic shadow.
 * This will update the shadow for one element without needing to register
 * No callbacks will be sent represent only one shadow element within a thing shadow.
 */
typedef struct
{
    char keyStr[LENGTH_AWS_SHADOW_KEY]; /*!< A key of the shadow element */
    value_st s_value;                   /*!< A value of the shadow element */
    shadowValueType_et valType_e;       /*!< A type of value required for this shadow element */
} awsThingShadow_st;

/**
 * @brief Shadow update callback function type. The application should
 * define the callback function and intialize it in the @ref awsShadowElement_st
 * configuration.
 */
typedef void (*awsShadowUpdateCallBack_t)(uint8_t elementIndex_u8, const char *pKeyStr, const void *pValue);

/**
 * @brief AWS Shadow structure used by the library to handle shadow updates.
 * You should initialize the callback handler and register this awsShadowElement_st
 * by calling @ref SHADOW_deltaRegister function to receive callbacks
 * whenever the shadow element is updated.
 *
 * This structure represent only one shadow element within a thing shadow.
 */
typedef struct
{
    const shadowValueType_et valType_e;           /*!< A type of value required for this shadow element */
    const char *keyStr;                           /*!< A key of the shadow element */
    const value_st s_value;                       /*!< A value of the shadow element */
    const uint8_t needToPublish_b8;               /*!< Flag to indicate data needs to be published */
    const shadowUpdateType_et shadowUpdateType_e; /*!< Flag to indicate update type desired/reported */
} awsShadowElement_st;

typedef struct
{
    const char *ptrShadowName;
    const uint8_t maxElementCount_u8;
    const awsShadowUpdateCallBack_t callbackHandler; /*!< Callback handler to be called when shadow element is updated */
    const awsShadowElement_st *pShadowElementsTable;
} shadowConfigTable_st;

typedef enum
{
    JOB_STATUS_IN_PROGRESS,
    JOB_STATUS_SUCCESSED,
    JOB_STATUS_FAILED,
    JOB_STATUS_REJECTED,
    JOB_STATUS_MAX,
} jobsStatus_et;

typedef struct
{
    uint32_t queuedAt_u32;                 /*!< Timestamp when job was queued */
    uint32_t currentTimeStamp_u32;         /*!< Current timestamp */
    char idStr[LENGTH_JOB_ID];             /*!< Job Id */
    char documentStr[LENGTH_JOB_DOCUMENT]; /*!< Job document */
} job_st;

/**
 * @brief Job callback handler type. The application should provide
 * this handler to perform actions based on the Job created on AWS IoT.
 */
typedef jobsStatus_et (*jobCallback_t)(const job_st *ps_job);

#endif //__LIB_AWS_TYPES_H__