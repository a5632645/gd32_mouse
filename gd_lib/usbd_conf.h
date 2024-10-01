#ifndef __USBD_CONF_H
#define __USBD_CONF_H

#include "gd32f10x.h"

/* define if low power mode is enabled; it allows entering the device into DEEP_SLEEP mode
   following USB suspend event and wakes up after the USB wakeup event is received. */
// #define USBD_LOWPWR_MODE_ENABLE

/* USB feature -- Self Powered */
/* #define USBD_SELF_POWERED */

/* link power mode support */
/* #define LPM_ENABLED */

#define USBD_CFG_MAX_NUM                   1
#define USBD_ITF_MAX_NUM                   1

#define USBD_HID_INTERFACE                 0

/* endpoint count used by the HID device */
#define EP_COUNT                           (2)

#define USB_STRING_COUNT                   4

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_TX_ADDR                        0x20
#define EP0_RX_ADDR                        0x40

#define INT_TX_ADDR                        0x50
#define INT_RX_ADDR                        0x60

#define HID_IN_EP                          EP_IN(1)

#define HID_IN_PACKET                      4


/* base address of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                      (0x0000)

#define USB_PULLUP                         GPIOD
#define USB_PULLUP_PIN                     GPIO_PIN_13
#define RCU_AHBPeriph_GPIO_PULLUP          RCU_GPIOD

#endif /* __USBD_CONF_H */
