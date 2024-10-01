#include "mouse_usb.h"
#include "mouse_fop_handler.h"

usb_dev usb_hid;

static void ClockConfig(void);
static void GpioConfig(void);
static void NvicConfig(void);

int main(void) {
    ClockConfig();
    GpioConfig();

    hid_itfop_register(&usb_hid, &gMouseFopHandler);
    usbd_init(&usb_hid, &hid_desc, &hid_class);

    NvicConfig();
    usbd_connect(&usb_hid);

    while(usb_hid.cur_status != USBD_CONFIGURED) {
    }

    while (1) {
        gMouseFopHandler.hid_itf_data_process(&usb_hid);
    }
}
