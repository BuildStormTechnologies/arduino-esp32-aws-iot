 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibJsonTypes.h
 * \brief JsonTypes library header file.
 *
 * 
 * This file defines the tags, structure and enums used by the JSON library. 
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

#ifndef __LIB_JSON_TYPES_H__
#define __LIB_JSON_TYPES_H__

#include <stdbool.h>
#include <stdint.h>

#define LENGTH_KEY_SIZE 32 // Max 3-char keyStr ex:"typ", "dat", "cmd", "sts" etc

/**
 * @brief A structure to represent key-value pairs in a JSON string.
 */
typedef struct
{
    char *keyStr;    /*!< A key */
    char *pValueStr; /*!< Value associated with the key */
} tagStructure_st;

#define MAX_JSON_ARRAY_OBJS 10
#define MAX_JSON_ARRAY_INTEGERS 10

/**
 * @brief A structure to represent array of JSON objects.
 */
typedef struct
{
    uint8_t numOfJosnObjs_u8;                /*!< Number of JSON objects in array */
    char jsonObjs[MAX_JSON_ARRAY_OBJS][150]; /*!< Array of objects */
} jsonArray_st;

/**
 * @brief A structure to represent array of integers
 */
typedef struct
{
    uint32_t arrayElements[MAX_JSON_ARRAY_INTEGERS]; /*!< Array of integaers */
} jsonArrayList_st;

#endif //__LIB_JSON_TYPES_H__