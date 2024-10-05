#include "paw3205.h"
#include "gd32f10x_gpio.h"

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

#define __CLOCK_HZ (96*1000*1000)
#define __DELAY_TICKS(us) ((us) * __CLOCK_HZ / 1000000)
#define DELAY_US(us) do { \
    uint32_t ticks = __DELAY_TICKS(us) >> 1; \
    while (ticks--); \
} while (0)
#define DELAY_MS(ms) do { \
    uint32_t t = ms; \
    while (t--) \
        DELAY_US(1000); \
} while (0)

static void Paw3205Init(void) {
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(SENSOR_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SCLK_PIN);
    gpio_init(SENSOR_GPIO, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SDIO_PIN);
    gpio_init(SENSOR_GPIO, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, MOTION_PIN);

    DELAY_MS(50);
    Paw3205_TrySync();
    Paw3205_ResetChip();
    DELAY_MS(5);

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
}

static void Paw3205Write(uint8_t data) {
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
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        gpio_bit_set(SENSOR_GPIO, SCLK_PIN);
        data <<= 1;
        if (gpio_input_bit_get(SENSOR_GPIO, SDIO_PIN)) {
            data |= 0x01;
        }
        gpio_bit_reset(SENSOR_GPIO, SCLK_PIN);
    }
    gpio_bit_set(SENSOR_GPIO, SCLK_PIN);
    return data;
}

static bool Paw3205WriteReg(Paw3205AddressEnum address, uint8_t data) {
    uint32_t tries = 16;
    do {
        Paw3205Write(0x80 | address);
        Paw3205Write(data);
    } while (Paw3205ReadReg(address) == data && (tries--));

    if (tries == 0) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

static uint8_t Paw3205ReadReg(Paw3205AddressEnum address) {
    Paw3205Write(0x7f & address);
    DELAY_US(3);
    uint8_t data = Paw3205Read();
    DELAY_US(1);
    return data;
}

static void Paw3205Sync(void) {
    gpio_bit_reset(SENSOR_GPIO, SCLK_PIN);
    DELAY_US(1);
    gpio_bit_set(SENSOR_GPIO, SCLK_PIN);
    DELAY_US(1700);
}

// --------------------------------------------------------------------------------
// public
// --------------------------------------------------------------------------------
void Paw3205_Init(void) {
    Paw3205Init();
}

void Paw3205_GetMotion(MotionStruct* motion) {
    Paw3205_TrySync();
    motion->dx = Paw3205ReadReg(ePaw3205_DeltaX);
    motion->dy = Paw3205ReadReg(ePaw3205_DeltaY);
    *(uint8_t*)&motion->motionStatus = Paw3205ReadReg(ePaw3205_Motion);
}

bool Paw3205_TrySync(void) {
    uint32_t trys = 16;
    uint16_t id = Paw3205ReadReg(ePAW3205_ProductID0);
    if (id == 0x30) {
        return TRUE;
    }

    while (trys--) {
        Paw3205Sync();
        id = Paw3205ReadReg(ePAW3205_ProductID0);
        if (id == 0x30) {
            return TRUE;
        }
    }
    return FALSE;
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

bool Paw3205_HasMotion(void) {
    if (gpio_input_bit_get(SENSOR_GPIO, MOTION_PIN) == RESET)
        return TRUE;
    return FALSE;
}

void Paw3205_SetCPI(Paw3205CPIEnum cpi) {
    Paw3205WriteReg(ePaw3205_WriteProtect, 0x5a);	//Unlock WP3
    uint8_t val = Paw3205ReadReg(ePaw3205_Config);
    ((Paw3205ConfigRegStruct*)&val)->cpi = cpi;
    Paw3205WriteReg(ePaw3205_Config, val);
    Paw3205WriteReg(ePaw3205_WriteProtect, 0x00);	//Lock WP
}
