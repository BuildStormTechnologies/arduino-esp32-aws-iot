
 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibAws.h
 * \brief LibAws library header file.
 *
 * 
 *  This library handles the AWS platform's core features:
 *   - Device Provisioning
 *   - Shadows
 *   - Jobs
 *   - OTA 
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

#ifndef __LIB_AWS_H__
#define __LIB_AWS_H__

#include "LibAwsTypes.h"

class Aws
{

private:
public:
    bool isConnected();
    bool subscribe(char *pTopicStr, uint8_t qos_e);
    bool publish(mqttMsg_st *ps_msg);
    uint8_t available();
    bool read(mqttMsg_st *ps_msg);
    const char *getThingName();

    bool shadowRegister(const shadowConfigTable_st *pShadowtable, uint8_t maxShadows);
    bool shadowUpdate(uint8_t shadowIndex, const char *pKeyStr, void *pValue, shadowUpdateType_et updateType_e);
    bool shadowDocumentUpdate(uint8_t shadowIndex, awsThingShadow_st as_thingShadow[], uint8_t maxKeys_u8, shadowUpdateType_et updateType_e);

    bool jobRegister(const char *pJobActionStr, uint8_t timeoutMin_u8, jobCallback_t callbackHandler);
    void jobUpdateStatus(char *pJobIdStr, jobsStatus_et status_e);
};

#endif //__LIB_AWS_H__
