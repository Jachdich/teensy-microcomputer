#include "../include/sgtl5000.h"
#include "../include/wire_c.h"
#include "../include/libs/core_pins.h"

#define I2C_ADDR 0x0A

uint32_t sgtl5000_read(uint32_t reg) {
    uint32_t val = 0;
    Wire_begin_transmission(I2C_ADDR);
    Wire_write(reg >> 8);
    Wire_write(reg & 0xFF);
    Wire_end_transmission(false);
    // TODO finish
}

void sgtl5000_setup(void) {
    Wire_begin();
    delay(5);
    sgtl5000_write(CHIP_ANA_POWER, ANA_POWER_REFTOP_POWERUP | ANA_POWER_ADC_MONO | ANA_POWER_DAC_MONO);
}

void audio_test(void) {
    setup_sgtl5000();
    while (1) {
		    
    }
}

