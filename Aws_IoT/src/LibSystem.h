 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibSystem.h
 * \brief System library header file.
 *
 * The system library initializes the system by managing core operations, 
 * including WiFi, BLE, MQTT, and HTTP communication for data transfers. 
 * It also manages flash storage operations, error handling, device logs,
 * and supports OTA firmware updates.
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

#ifndef __LIB_SYSTEM_H__
#define __LIB_SYSTEM_H__

#include "LibSystemTypes.h"

class LibSystem
{

private:
public:
    bool init(systemInitConfig_st *s_pConfig);
    void start();
    void restart(uint32_t restartDelay_u32);
    bool isWaitingForRestart();
    systemMode_et getMode();
    bool isDeviceRegistered();
    const char *getDeviceName();
    const char *getMacAddr();
    uint8_t getOtaPercentage();
};

#endif //__LIB_SYSTEM_H__
