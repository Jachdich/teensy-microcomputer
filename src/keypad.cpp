#include "../include/keypad.h" 
#include "../include/libs/Arduino.h"

const unsigned char keypad_pins[NUM_KEYS] = {
    27, 31, 38, 26,
    29, 28, 32, 34,
    30, 33, 36, 35,
    22, 2
};



char calcKeypad[4][3] {
    "789",
    "456",
    "123",
    "0  "
};

void setup_keypad() {
    for (int i = 0; i < NUM_KEYS; i++) {
        pinMode(keypad_pins[i], INPUT_PULLUP);
    }
}

void read_keypad(Keypad *pad) {
    for (int i = 0; i < NUM_KEYS; i++) {
        bool lvl = (pad->buttons[i] & 0x1);
        int elapsed = millis() - pad->times[i];
        bool is_long = elapsed > 400;
        bool new_lvl = !digitalReadFast(keypad_pins[i]);
        bool rising = (lvl != new_lvl) && new_lvl;
        bool falling = (lvl != new_lvl) && (!new_lvl);
        if (rising) {
            pad->times[i] = millis();
        }
        pad->buttons[i] = is_long << 3 | falling << 2 | rising << 1 | new_lvl;
    }
}

void keypad_debug(Keypad *pad) {
    for (int i = 0; i < NUM_KEYS; i++) {
        print(pad->buttons[i] & 0x1 ? '1' : '0');
    }
    print('\n');
}