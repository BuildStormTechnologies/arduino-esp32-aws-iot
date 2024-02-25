 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibWifi.h
 * \brief Wifi library header file.
 *
 * 
 * The Wifi library handles the Wifi operations.
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

#ifndef __LIB_WIFI_H__
#define __LIB_WIFI_H__

#include "LibWifiTypes.h"

class LibWifi
{

private:
public:
    bool isConnected();
    char *getIP();
    uint8_t getSignalStrength();
    int8_t getRssi();
    const char *getStateString();
    wifiStates_et getState();
	const char *getSSID();
};

#endif //__LIB_WIFI_H__
