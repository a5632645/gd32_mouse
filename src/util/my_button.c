#include "my_button.h"

static void ButtonStateTick(ButtonStateEnum* state, uint8_t press) {
    switch (*state) {
    case eButtonState_Idel:
        if (press)
            *state = eButtonState_Click;
        break;
    case eButtonState_Click:
        if (press)
            *state = eButtonState_Press;
        else
            *state = eButtonState_Release;
        break;
    case eButtonState_Press:
        if (!press)
            *state = eButtonState_Release;
        break;
    case eButtonState_Release:
        if (press)
            *state = eButtonState_Click;
        else
            *state = eButtonState_Idel;
        break;
    default:
        break;
    }
}

void MyButton_Tick(MyButtonStruct* state, uint8_t press, uint32_t escape) {
    ButtonStateTick(&state->state, press);

    switch (state->state) {
    case eButtonState_Click:
        state->pressTime = escape;
        break;
    case eButtonState_Press:
        state->pressTime += escape;
        break;
    default:
        break;
    }
}
