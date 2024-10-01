#ifndef __STANDARD_HID_CORE_H
#define __STANDARD_HID_CORE_H

#include "usbd_enum.h"
#include "usb_hid.h"

#define USB_HID_CONFIG_DESC_LEN          0x22U
#define USB_HID_REPORT_DESC_LEN          50U

typedef struct
{
    uint32_t protocol;
    uint32_t idle_state;
    uint8_t data[HID_IN_PACKET];
    __IO uint8_t prev_transfer_complete;
} standard_hid_handler;

typedef struct
{
    void (*hid_itf_config) (void);
    void (*hid_itf_data_process) (usb_dev *udev);
} hid_fop_handler;

extern usb_desc hid_desc;
extern usb_class hid_class;

/* function declarations */
/* register HID interface operation functions */
uint8_t hid_itfop_register (usb_dev *udev, hid_fop_handler *hid_fop);
/* send HID report */
uint8_t hid_report_send (usb_dev *udev, uint8_t *report, uint16_t len);

#endif /* __STANDARD_HID_CORE_H */
