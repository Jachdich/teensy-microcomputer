#include "../include/wire_c.h"
#include "../include/libs/Wire.h"

void Wire_begin() {
		Wire.begin();
}

void Wire_begin_transmission(uint8_t addr) {
		Wire.beginTransmission(addr);
}

void Wire_write(uint8_t data) {
		Wire.write(data);
}

uint8_t Wire_read() {
		return Wire.read();
}
