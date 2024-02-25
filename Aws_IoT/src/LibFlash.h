 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibFlash.h
 * \brief Flash library header file.
 *
 * The Flash library handles read, write and erase of the device configuration and data. 
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

#ifndef __LIB_FLASH_H__
#define __LIB_FLASH_H__

#include "LibFlashTypes.h"

class LibFlash
{

private:
public:
    bool isBusy();
    bool read(void *pData, uint16_t size_u16);
    bool write(void *pData, uint16_t size_u16);
    bool erase();
};

#endif //__LIB_FLASH_H__
