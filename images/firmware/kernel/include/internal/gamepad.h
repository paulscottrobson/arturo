/**
 * @file       gamepad.h
 *
 * @brief      Header file, Gamepad code, internal
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#pragma once

#define CTL_DEVICE_TYPE_ID(vid,pid)             (((vid) << 16)|(pid))               // 32 bit device type.
#define CTL_HARDWARE_ID(dev_addr,instance)      (((dev_addr << 8)|(instance)))      // 16 bit composite identifier

#define CTLM_REGISTER       (0)
#define CTLM_UPDATE         (1)

struct _CTL_MessageData {
    const uint8_t *report;                                                          // USB report data
    uint8_t len;                                                                    // Length of report data.
};

void CTLInitialise(void);
void CTLAddController(uint8_t dev_addr,uint8_t instance,uint16_t vid,uint16_t pid);
void CTLUpdateController(uint8_t dev_addr,uint8_t instance,uint8_t const *report,uint8_t len);
int  CTLSendMessage(int command,uint16_t hwid,struct _CTL_MessageData *msg);
int  CTLDispatchMessage(int command,CTLState *cs,struct _CTL_MessageData *msg);
void CTLAnnounceDevice(CTLState *cs,const char *name);


