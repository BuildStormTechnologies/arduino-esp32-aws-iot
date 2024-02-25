 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibUtils.h
 * \brief Utils library header file.
 *
 * 
 * The  Utils library handles the utilities for data extraction and hex dumps. 
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

#ifndef __LIB_UTILS_H__
#define __LIB_UTILS_H__

class LibUtils
{

private:
public:
    uint32_t getNumU32(const char *pStr);
    int32_t getNumI32(const char *pStr);
    float getFloat(const char *pStr, uint8_t decPoints);
    bool isValidString(packet_st *ps_packet);
    void hexDump(uint8_t *buffPtr, uint16_t dumpSize, uint8_t charsPerLine, bool ascii);
};

#endif //__LIB_UTILS_H__