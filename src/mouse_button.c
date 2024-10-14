#include "gd32f10x_rcu.h"
#include "gd32f10x_gpio.h"
#include "gd32f10x_timer.h"
#include "mouse_button.h"

#define MOUSE_LEFT_GPIO GPIOA
#define MOUSE_LEFT_PIN  GPIO_PIN_5

#define MOUSE_RIGHT_GPIO GPIOB
#define MOUSE_RIGHT_PIN  GPIO_PIN_4

#define MOUSE_CENTER_GPIO GPIOB
#define MOUSE_CENTER_PIN  GPIO_PIN_5

#define AUTO_CLICK_GPIO GPIOB
#define AUTO_CLICK_PIN GPIO_PIN_1

#define AUTO_PRESS_GPIO GPIOB
#define AUTO_PRESS_PIN GPIO_PIN_2

void MouseButton_Init(void) {
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    gpio_init(MOUSE_LEFT_GPIO, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, MOUSE_LEFT_PIN);
    gpio_init(MOUSE_RIGHT_GPIO, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, MOUSE_RIGHT_PIN);
    gpio_init(MOUSE_CENTER_GPIO, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, MOUSE_CENTER_PIN);
    gpio_init(AUTO_CLICK_GPIO, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, AUTO_CLICK_PIN);
    gpio_init(AUTO_PRESS_GPIO, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, AUTO_PRESS_PIN);
}

uint8_t MouseButton_IsPressed(MouseButtonEnum button) {
    switch (button) {
        case eMouseButtonLeft:
            return RESET == gpio_input_bit_get(MOUSE_LEFT_GPIO, MOUSE_LEFT_PIN);
        case eMouseButtonRight:
            return RESET == gpio_input_bit_get(MOUSE_RIGHT_GPIO, MOUSE_RIGHT_PIN);
        case eMouseButtonCenter:
            return RESET == gpio_input_bit_get(MOUSE_CENTER_GPIO, MOUSE_CENTER_PIN);
        case eMouseButtonAutoClick:
            return RESET == gpio_input_bit_get(AUTO_CLICK_GPIO, AUTO_CLICK_PIN);
        case eMouseButtonAutoPress:
            return RESET == gpio_input_bit_get(AUTO_PRESS_GPIO, AUTO_PRESS_PIN);
        default:
            return 0;
    }
}
