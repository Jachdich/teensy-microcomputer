#ifndef _WIRE_C_H
#define _WIRE_C_H
#include "../include/libs/Wire.h"
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
void Wire_begin();
void Wire_begin_transmission(uint8_t addr);
uint8_t Wire_end_transmission(bool);
void Wire_write(uint8_t data);
uint8_t Wire_read();
uint8_t Wire_request_from(uint8_t address, uint8_t quantity, bool send_stop);
#ifdef __cplusplus
}
#endif
#endif