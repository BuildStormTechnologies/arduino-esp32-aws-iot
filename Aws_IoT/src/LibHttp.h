 /**
 * \copyright Copyright (c) 2019-2024, Buildstorm Pvt Ltd
 *
 * \file LibHttp.h
 * \brief Http library header file.
 *
 * The Http library handles http requests.
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

#ifndef __LIB_HTTP_H__
#define __LIB_HTTP_H__

#include "LibHttpTypes.h"
#include "LibMsgTypes.h"

class LibHttp
{

private:
public:
    bool start(httpConfig_st *ps_config);
    uint16_t availableToRead();
    bool read(packet_st *ps_packet);
    void close();
    const char *getStateString(httpStates_et state_e);
};

#endif //__LIB_HTTP_H__