#include "../include/wire_c.h"
#include "../include/libs/Wire.h"

void Wire_begin(uint8_t addr) {
		Wire.begin(addr);
}

void Wire_begin_transmission(uint8_t addr) {
		Wire.beginTransmission(addr);
}

void Wire_write(int data) {
		Wire.write(data);
}

int Wire_read() {
		return Wire.read();
}
