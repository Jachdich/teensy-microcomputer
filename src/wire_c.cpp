#include "../include/wire_c.h"
#include "../include/libs/Wire.h"

void Wire_begin() {
		Wire.begin();
}

void Wire_begin_transmission(uint8_t addr) {
		Wire.beginTransmission(addr);
}

uint8_t Wire_end_transmission(bool sendStop) {
		return Wire.endTransmission(sendStop);
}

void Wire_write(uint8_t data) {
		Wire.write(data);
}

uint8_t Wire_read() {
		return Wire.read();
}

uint8_t Wire_request_from(uint8_t address, uint8_t quantity, uint8_t send_stop) {
		return Wire.requestFrom(address, quantity, send_stop);
}
