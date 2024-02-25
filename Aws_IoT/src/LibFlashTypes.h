 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibFlashTypes.h
 * \brief FlashTypes library header file.
 *
 * This file defines the structure and enums used by the Flash library.
 * 
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

#ifndef __LIB_FLASH_TYPES_H__
#define __LIB_FLASH_TYPES_H__

/*---------------------------structures-------------------------------------*/
/**
 * @enum flashStatus_t
 * An enum to represent the status of flash operations.
 */
typedef enum
{
    FLASH_STATUS_OK,              /*!< Status ok */
    FLASH_STATUS_BUSY,            /*!< Status busy */
    FLASH_STATUS_LOAD_DEFAULT,    /*!< Load default configuration values */
    FLASH_STATUS_READ_FAIL,       /*!< Read failed */
    FLASH_STATUS_WRITE_FAIL,      /*!< Write failed */
    FLASH_STATUS_INVALID_DATASET, /*!< Data is invalid */
    FLASH_STATUS_MAX              /*!< Total number of status for Flash library */
} flashStatus_t;



/**
 * @enum deviceConfigParams_et
 * An enum to represent Device Configuration parameters.
 */
typedef enum
{
    DEVICE_CONFIG_HOST,       /*!< AWS IoT Host */
    DEVICE_CONFIG_PORT,       /*!< AWS IoT Port number */
    DEVICE_CONFIG_WIFI,       /*!< WIFI configuration parameter */
    DEVICE_CONFIG_THING_NAME, /*!< Thing name parameter */
    DEVICE_CONFIG_CLEAR,      /*!< Clear configurations */
    DEVICE_CONFIG_MAX         /*!< Total number of available configuration parameters */
} deviceConfigParams_et;

/**
 * @enum deviceCertType_et
 * An enum to represent Device Certificate type.
 */
typedef enum
{
    FLASH_CERT_CERTIFICATE_PEM, /*!< Device certificate */
    FLASH_CERT_PRIVATE_KEY,     /*!< Device private key */
    FLAH_CERT_MAX               /*!< Total number of device certificate types */
} deviceCertType_et;

/**
 * @enum flashDataSet_et
 * An enum to represent available Flash Datasets.
 */
typedef enum
{
    FLASH_DATASET_DEVICE_CONFIG,       /*!< Device configuration dataset */
    FLASH_DATASET_DEVICE_CERTIFICATES, /*!< Device certificate dataset */
    FLASH_DATASET_DEVICE_PRIVATE_KEY,  /*!< Device private key dataset */
    FLASH_DATASET_APP,                 /*!< Application dataset */
    FLASH_DATASET_MAX,                 /*!< Total number of available datasets */
} flashDataSet_et;

#endif //__LIB_FLASH_TYPES_H__
