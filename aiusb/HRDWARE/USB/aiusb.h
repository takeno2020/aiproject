#ifndef AI_USB_H_
#define AI_USB_H_

#include <aitype.h>

#define    AIUSB_GET_STATUS           0
#define    AIUSB_CLEAR_FEATURE        1
#define    AIUSB_SET_FEATURE          3
#define    AIUSB_SET_ADDRESS          5
#define    AIUSB_GET_DESCRIPTOR       6
#define    AIUSB_SET_DESCRIPTOR       7
#define    AIUSB_GET_CONFIGURATION    8
#define    AIUSB_SET_CONFIGURATION    9
#define    AIUSB_GET_INTERFACE        10
#define    AIUSB_SET_INTERFACE        11
#define    AIUSB_SYNCH_FRAME          12

#define    AIUSB_DEVICE_DESCRIPTOR           1
#define    AIUSB_CONFIGURATION_DESCRIPTOR    2
#define    AIUSB_STRING_DESCRIPTOR           3
#define    AIUSB_INTERFACE_DESCRIPTOR        4
#define    AIUSB_ENDPOINT_DESCRIPTOR         5
#define    AIUSB_REPORT_DESCRIPTOR           0x22

extern uint8 ai_usb_ep1_in_is_busy;

void ai_usb_disconnect(void);
void ai_usb_connect(void);
void ai_usb_bus_suspend(void);
void ai_usb_bus_reset(void);
void ai_usb_ep0_out(void);
void ai_usb_ep0_in(void);
void ai_usb_ep1_out(void);
void ai_usb_ep1_in(void);
void ai_usb_ep2_out(void);
void ai_usb_ep2_in(void);

#endif /* AI_USB_H_ */