#ifndef _WIRE_C_H
#define _WIRE_C_H
#include "../include/libs/Wire.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void Wire_begin(uint8_t addr);
void Wire_begin_transmission(uint8_t addr);
void Wire_write(int data);
int Wire_read();
#ifdef __cplusplus
}
#endif
#endif