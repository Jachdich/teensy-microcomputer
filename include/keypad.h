#ifndef __KEYPAD_H
#define __KEYPAD_H
#include <stdint.h>

#define NUM_KEYS 14

struct Keypad {
    uint8_t buttons[NUM_KEYS];
    uint32_t times[NUM_KEYS];
};
typedef struct Keypad Keypad;

void read_keypad(Keypad *pad);
void setup_keypad();
void keypad_debug(Keypad *pad);

void print(char n);
#endif
