#include "paw3205.h"
#include "gd32f10x_gpio.h"
#include "delay.h"
#include "uart_printf.h"

#define SENSOR_GPIO GPIOB
#define SCLK_PIN GPIO_PIN_8
#define SDIO_PIN GPIO_PIN_9
#define MOTION_PIN GPIO_PIN_0

typedef enum {
    ePAW3205_ProductID0 = 0x00,
    ePAW3205_ProductID1 = 0x01,
    ePaw3205_Motion = 0x02,
    ePaw3205_DeltaX = 0x03,
    ePaw3205_DeltaY = 0x04,
    ePaw3205_OpMode = 0x05,
    ePaw3205_Config = 0x06,
    ePaw3205_ImgQuality = 0x07,
    ePaw3205_OpState = 0x08,
    ePaw3205_WriteProtect = 0x09,
    ePaw3205_SleepSetting1 = 0x0A,
    ePaw3205_EnterTime = 0x0B,
    ePaw3205_SleepSetting2 = 0x0C,
    ePaw3205_ImgThreshold = 0x0D,
    ePaw3205_ImgRecognition = 0x0E,
} Paw3205AddressEnum;

static void Paw3205Init(void);
static void Paw3205Write(uint8_t data);
static uint8_t Paw3205Read(void);
static bool Paw3205WriteReg(Paw3205AddressEnum address, uint8_t data);
static uint8_t Paw3205ReadReg(Paw3205AddressEnum address);
static void Paw3205Sync(void);

static void Paw3205Init(void) {
    Delay_Init();

    MY_LOG_INFO("paw3205 gpio init\n");
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(SENSOR_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SCLK_PIN);
    gpio_init(SENSOR_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SDIO_PIN);
    gpio_init(SENSOR_GPIO, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, MOTION_PIN);
    gpio_bit_set(SENSOR_GPIO, SCLK_PIN);
    gpio_bit_set(SENSOR_GPIO, SDIO_PIN);

    Delay_Ms(50);
    MY_LOG_INFO("paw3205 try sync\n");
    Paw3205_TrySync();
    MY_LOG_INFO("paw3205 reset\n");
    Paw3205_ResetChip();
    Delay_Ms(5);

    MY_LOG_INFO("paw3205 init\n");
    Paw3205WriteReg(ePaw3205_WriteProtect, 0x5a);	//Unlock WP
    Paw3205WriteReg(0x28, 0xb4);
    Paw3205WriteReg(0x29, 0x46);
    Paw3205WriteReg(0x2a, 0x96);
    Paw3205WriteReg(0x2b, 0x8c);
    Paw3205WriteReg(0x2c, 0x6e);
    Paw3205WriteReg(0x2d, 0x64);
    Paw3205WriteReg(0x38, 0x5f);
    Paw3205WriteReg(0x39, 0x0f);
    Paw3205WriteReg(0x3a, 0x32);
    Paw3205WriteReg(0x3b, 0x47);
    Paw3205WriteReg(0x42, 0x10);
    Paw3205WriteReg(0x54, 0x2e);
    Paw3205WriteReg(0x55, 0xf2);
    Paw3205WriteReg(0x61, 0xf4);
    Paw3205WriteReg(0x63, 0x70);
    Paw3205WriteReg(0x75, 0x52);
    Paw3205WriteReg(0x76, 0x41);
    Paw3205WriteReg(0x77, 0xed);
    Paw3205WriteReg(0x78, 0x23);
    Paw3205WriteReg(0x79, 0x46);
    Paw3205WriteReg(0x7a, 0xe5);
    Paw3205WriteReg(0x7c, 0x48);
    Paw3205WriteReg(0x7d, 0x80);
    Paw3205WriteReg(0x7e, 0x77);

    Paw3205Write(0xff);		//bank1
    Paw3205Write(0x01);
    Paw3205WriteReg(0x0b, 0x00);
    Paw3205Write(0xff);		//bank0
    Paw3205Write(0x00);

    Paw3205WriteReg(ePaw3205_WriteProtect, 0x00);	//Lock WP

    for (uint8_t addr = 0x2; addr < 0x7; addr++)
        Paw3205ReadReg(addr);
    
    MY_LOG_INFO("paw3205 init done\n");
}

typedef struct {
    uint32_t : 4;
    uint32_t md : 2;
    uint32_t ctrl : 2;
    uint32_t : 24;
} GpioCtrlStruct;
#define SENSOR_CTRL ((volatile GpioCtrlStruct*)(&(GPIO_CTL1(SENSOR_GPIO))))

static void Paw3205Write(uint8_t data) {
    SENSOR_CTRL->md = 0b11;
    SENSOR_CTRL->ctrl = 0b00;
    for (int i = 0; i < 8; i++) {
        gpio_bit_reset(SENSOR_GPIO, SCLK_PIN);
        if (data & 0x80) {
            gpio_bit_set(SENSOR_GPIO, SDIO_PIN);
        } else {
            gpio_bit_reset(SENSOR_GPIO, SDIO_PIN);
        }
        data <<= 1;
        gpio_bit_set(SENSOR_GPIO, SCLK_PIN);
    }
}

static uint8_t Paw3205Read(void) {
    SENSOR_CTRL->md = 0b00;
    SENSOR_CTRL->ctrl = 0b01;
    Delay_Us(5);
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        gpio_bit_reset(SENSOR_GPIO, SCLK_PIN);
        data <<= 1;
        gpio_bit_set(SENSOR_GPIO, SCLK_PIN);
        if (SET == gpio_input_bit_get(SENSOR_GPIO, SDIO_PIN)) {
            data |= 0x01;
        }
    }
    SENSOR_CTRL->md = 0b11;
    SENSOR_CTRL->ctrl = 0b00;
    return data;
}

static bool Paw3205WriteReg(Paw3205AddressEnum address, uint8_t data) {
    uint32_t tries = 16;
    do {
        Paw3205Write(0x80 | address);
        Paw3205Write(data);
    } while (Paw3205ReadReg(address) == data && (tries--));

    if (tries == 0) {
        UartPrintf_Puts("Paw3205 write error\n");
        UartPrintf_Puts("[");
        UartPrintf_PrintHex(address, 0);
        UartPrintf_Puts("] ");
        UartPrintf_PrintHex(data, 1);
        return FALSE;
    }
    else {
        return TRUE;
    }
}

static uint8_t Paw3205ReadReg(Paw3205AddressEnum address) {
    Paw3205Write(0x7f & address);
    uint8_t data = Paw3205Read();
    return data;
}

static void Paw3205Sync(void) {
    gpio_bit_reset(SENSOR_GPIO, SCLK_PIN);
    Delay_Us(2);
    gpio_bit_set(SENSOR_GPIO, SCLK_PIN);
    Delay_Us(1700);
}

// --------------------------------------------------------------------------------
// public
// --------------------------------------------------------------------------------
void Paw3205_Init(void) {
    Paw3205Init();
}

int8_t Paw3205_NumConvert(uint8_t code) {
    if (code & 0x80) {
        // negative
        --code;
        code = (~code) & 0x7f;
        return -code;
    }
    else {
        // positive
        return code;
    }
}


uint8_t Paw3205_GetMotion(MotionStruct* motion) {
    uint8_t sync = Paw3205_TrySync();
    motion->dx = Paw3205ReadReg(ePaw3205_DeltaX);
    motion->dy = Paw3205ReadReg(ePaw3205_DeltaY);
    *(uint8_t*)&motion->motionStatus = Paw3205ReadReg(ePaw3205_Motion);
    return sync;
}

uint8_t Paw3205_TrySync(void) {
    uint32_t trys = 32;
    uint16_t id = Paw3205ReadReg(ePAW3205_ProductID0);

    if ((id & 0x30) == 0x30) {
        return 1;
    }
    while (trys--) {
        Paw3205Sync();
        id = Paw3205ReadReg(ePAW3205_ProductID0);
        if ((id & 0x30) == 0x30) {
            return 1;
        }
    }
    Paw3205_ResetChip();
    return 0;
}

typedef struct {
    uint8_t cpi : 3;
    uint8_t powerDown : 1;
    uint8_t zero : 2;
    uint8_t motswk : 1;
    uint8_t reset : 1;
}Paw3205ConfigRegStruct;
void Paw3205_ResetChip(void) {
    Paw3205WriteReg(ePaw3205_WriteProtect, 0x5a);	//Unlock WP3
    uint8_t val = Paw3205ReadReg(ePaw3205_Config);
    ((Paw3205ConfigRegStruct*)&val)->reset = 1;
    Paw3205WriteReg(ePaw3205_Config, val);
    Paw3205WriteReg(ePaw3205_WriteProtect, 0x00);	//Lock WP
}

uint8_t Paw3205_HasMotion(void) {
    if (gpio_input_bit_get(SENSOR_GPIO, MOTION_PIN) == RESET)
        return 1;
    return 0;
}

void Paw3205_SetCPI(Paw3205CPIEnum cpi) {
    Paw3205WriteReg(ePaw3205_WriteProtect, 0x5a);	//Unlock WP3
    uint8_t val = Paw3205ReadReg(ePaw3205_Config);
    ((Paw3205ConfigRegStruct*)&val)->cpi = cpi;
    Paw3205WriteReg(ePaw3205_Config, val);
    Paw3205WriteReg(ePaw3205_WriteProtect, 0x00);	//Lock WP
}

void Paw3205_DumpReg(void) {
    UartPrintf_Puts("Paw3205 dump:\n");
    uint8_t d = 0;
    d = Paw3205ReadReg(ePAW3205_ProductID0);
    UartPrintf_Puts("pid0:");UartPrintf_PrintNum(d, 1);
    d = Paw3205ReadReg(ePAW3205_ProductID1);
    UartPrintf_Puts("pid1:");UartPrintf_PrintNum(d, 1);
    d = Paw3205ReadReg(ePaw3205_Motion);
    UartPrintf_Puts("mot:");UartPrintf_PrintNum(d, 1);
    d = Paw3205ReadReg(ePaw3205_DeltaX);
    UartPrintf_Puts("dx:");UartPrintf_PrintNum((int8_t)d, 1);
    d = Paw3205ReadReg(ePaw3205_DeltaY);
    UartPrintf_Puts("dy:");UartPrintf_PrintNum((int8_t)d, 1);
    d = Paw3205ReadReg(ePaw3205_OpMode);
    UartPrintf_Puts("op:");UartPrintf_PrintNum(d, 1);
    d = Paw3205ReadReg(ePaw3205_Config);
    UartPrintf_Puts("config:");UartPrintf_PrintNum(d, 1);
    d = Paw3205ReadReg(ePaw3205_ImgQuality);
    UartPrintf_Puts("iq:");UartPrintf_PrintNum(d, 1);
    d = Paw3205ReadReg(ePaw3205_OpState);
    UartPrintf_Puts("os:");UartPrintf_PrintNum(d, 1);
    d = Paw3205ReadReg(ePaw3205_WriteProtect);
    UartPrintf_Puts("wp:");UartPrintf_PrintNum(d, 1);
    d = Paw3205ReadReg(ePaw3205_SleepSetting1);
    UartPrintf_Puts("s1:");UartPrintf_PrintNum(d, 1);
    d = Paw3205ReadReg(ePaw3205_SleepSetting2);
    UartPrintf_Puts("s2:");UartPrintf_PrintNum(d, 1);
    UartPrintf_Puts("\n");
}