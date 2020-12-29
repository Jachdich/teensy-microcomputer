#include <keypad.h>
#include <Arduino.h>

unsigned char keypadPins[4][3] = {
{30, 29, 27},
{28, 33, 31},
{36, 32, 38},
{35, 34, 37}, 
};

char calcKeypad[4][3] {
	"789",
	"456",
	"123",
	"0  "
};

unsigned char keypad[4][3];

void setupKeypad() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			pinMode(keypadPins[i][j], INPUT_PULLUP);
		}
	}
}

bool readKeypad() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			bool lvl = (keypad[i][j] & 0x1);
			//bool chg = (keypad[i][j] & 0x1 << 1) >> 1;
			bool newLvl = !digitalReadFast(keypadPins[i][j]);
			bool rising = (lvl != newLvl) && newLvl;
			bool falling = (lvl != newLvl) && (!newLvl);
			keypad[i][j] = falling << 2 | rising << 1 | newLvl;
		}
	}
    if (keypad[0][2] == 0b001) {
        return true;
    }
    return false;
}
