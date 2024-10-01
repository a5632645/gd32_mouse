#include "mouse_fop_handler.h"

typedef struct {
    struct {
        uint8_t left : 1;
        uint8_t right : 1;
        uint8_t center : 1;
        uint8_t reserve : 1; // 始终为1
        uint8_t dxSign : 1;
        uint8_t dySign : 1;
        uint8_t dxOverflow : 1;
        uint8_t dyOverflow : 1;
    } bits;

    uint8_t dx;
    uint8_t dy;
    int8_t wheel; 
} MoudeReportStruct;

static void HidItfConfig(void);
static void HidItfDataProcess(usb_dev* udev);

hid_fop_handler gMouseFopHandler = {
    .hid_itf_config = HidItfConfig,
    .hid_itf_data_process = HidItfDataProcess
};

static void HidItfConfig(void) {
    // TODO: 配置和鼠标有关的硬件啥的
}

// 不是哥们，怎么是死循环一直调用这玩意啊
static void HidItfDataProcess(usb_dev* udev) {
    standard_hid_handler *hid = (standard_hid_handler *)udev->class_data[USBD_HID_INTERFACE];
    if (hid->prev_transfer_complete == 0)
        return;

    MoudeReportStruct* report = (MoudeReportStruct*)hid->data;
    report->bits.reserve = 1;
    // TODO: 发送报文
    
    hid_report_send(udev, hid->data, HID_IN_PACKET);
}