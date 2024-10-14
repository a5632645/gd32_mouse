#include "mouse_fop_handler.h"
#include "mouse_usb.h"
#include <string.h>

usb_dev usb_hid;

static void HidItfConfig(void);
static void HidItfDataProcess(usb_dev* udev);

hid_fop_handler gMouseFopHandler = {
    .hid_itf_config = HidItfConfig,
    .hid_itf_data_process = HidItfDataProcess
};

static void HidItfConfig(void) {
    standard_hid_handler *hid = (standard_hid_handler*)usb_hid.class_data[USBD_HID_INTERFACE];
}

static void HidItfDataProcess(usb_dev* udev) {
    standard_hid_handler *hid = (standard_hid_handler *)udev->class_data[USBD_HID_INTERFACE];
    if (hid->prev_transfer_complete == 0)
        return;

    MoudeReportStruct* report = (MoudeReportStruct*)hid->data;
    report->bits.reserve = 1;
    
    hid_report_send(udev, hid->data, HID_IN_PACKET);
}

// USB 引脚初始化函数
static void usb_gpio_init(void) {
    // // 使能 GPIOA 时钟（根据具体的引脚调整）
    // rcu_periph_clock_enable(RCU_GPIOA);
    // rcu_periph_clock_enable(RCU_AF);

    // // 配置 PA11 为 USB DM
    // gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    
    // // 配置 PA12 为 USB DP
    // gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
}

void MouseUsb_Init(void) {
    rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV2);
    rcu_periph_clock_enable(RCU_USBD);

    usb_gpio_init();

    hid_itfop_register(&usb_hid, &gMouseFopHandler);
    usbd_init(&usb_hid, &hid_desc, &hid_class);

    nvic_irq_enable(USBD_LP_CAN0_RX0_IRQn, 1, 0);
    nvic_irq_enable((uint8_t)USBD_WKUP_IRQn, 0U, 0U);

    usbd_connect(&usb_hid);
    while(usb_hid.cur_status != USBD_CONFIGURED) {
    }
}

uint8_t MouseUsb_IsReady(void) {
    standard_hid_handler *hid = (standard_hid_handler *)usb_hid.class_data[USBD_HID_INTERFACE];
    return hid->prev_transfer_complete == 1;
}

void MouseUsb_Send(MoudeReportStruct* report) {
    report->bits.reserve = 1;
    hid_report_send(&usb_hid, (uint8_t*)report, HID_IN_PACKET);
}

void MouseUsb_ResetReport(MoudeReportStruct* report) {
    memset(report, 0, sizeof(MoudeReportStruct));
    report->bits.reserve = 1;
}
