 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibJson.h
 * \brief Json library header file.
 *
 * The JSON library provides the API's form JSON parsing
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

#ifndef __LIB_JSON_H__
#define __LIB_JSON_H__

#include "LibJsonTypes.h"

class LibJson
{

private:
public:
    uint8_t extractString(const char *pJsonStr, const tagStructure_st tags[], uint8_t maxKeys_u8, uint8_t clearvalueFlag_u8);
    uint8_t extractArrayOfIntegers(char *pJsonStr, jsonArrayList_st *jsnArrStr);
    uint8_t extractArrayObjs(char *pJsonStr, jsonArray_st *jsnArrPtr);
};

#endif //__LIB_JSON_H__
