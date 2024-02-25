/**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibBle.h
 * \brief BLE library header file.
 *
 * The Ble library handles Ble core operations.
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

#ifndef __LIB_BLE_H__
#define __LIB_BLE_H__

#include "LibBleTypes.h"

class LibBle
{

private:
public:
    bool start();
    bool isConnected();
    void disconnect();
    bool packetRegister(char *pPacketStr, bleCallBack_t callbackHandler);
    bleWrtStatus_et write(char *pPacketStr, char *pDataStr, uint8_t code_u8);
    bleWrtStatus_et ack(char *pPacketStr, uint8_t code_u8);
    const char *getStateString();
};

#endif //__LIB_BLE_H__
