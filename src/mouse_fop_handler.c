#include "mouse_fop_handler.h"
#include "mouse_usb.h"
#include <string.h>

usb_dev usb_hid;
MoudeReportStruct* gMouseReport;

static void HidItfConfig(void);
static void HidItfDataProcess(usb_dev* udev);

hid_fop_handler gMouseFopHandler = {
    .hid_itf_config = HidItfConfig,
    .hid_itf_data_process = HidItfDataProcess
};

static void HidItfConfig(void) {
    standard_hid_handler *hid = (standard_hid_handler*)usb_hid.class_data[USBD_HID_INTERFACE];
    gMouseReport = (MoudeReportStruct*)hid->data;
}

static void HidItfDataProcess(usb_dev* udev) {
    standard_hid_handler *hid = (standard_hid_handler *)udev->class_data[USBD_HID_INTERFACE];
    if (hid->prev_transfer_complete == 0)
        return;

    MoudeReportStruct* report = (MoudeReportStruct*)hid->data;
    report->bits.reserve = 1;
    
    hid_report_send(udev, hid->data, HID_IN_PACKET);
}

void MouseUsb_Init(void) {
    rcu_periph_clock_enable(RCU_USBD);
    rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV2);

    hid_itfop_register(&usb_hid, &gMouseFopHandler);
    usbd_init(&usb_hid, &hid_desc, &hid_class);

    nvic_irq_enable(USBD_LP_CAN0_RX0_IRQn, 0, 0);

    usbd_connect(&usb_hid);
    while(usb_hid.cur_status != USBD_CONFIGURED) {
    }
}

uint8_t MouseUsb_IsReady(void) {
    standard_hid_handler *hid = (standard_hid_handler *)usb_hid.class_data[USBD_HID_INTERFACE];
    return hid->prev_transfer_complete == 1;
}

void MouseUsb_Send() {
    gMouseReport->bits.reserve = 0;
    hid_report_send(&usb_hid, (uint8_t*)gMouseReport, HID_IN_PACKET);
}

void MouseUsb_ResetReport(void) {
    memset(gMouseReport, 0, sizeof(MoudeReportStruct));
    gMouseReport->bits.reserve = 1;
}