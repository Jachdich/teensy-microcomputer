#include "../include/libs/imxrt.h"
#include <stdbool.h>
#include <stdint.h>

int main() {
   // Configure GPIO B0_03 (PIN 13) for output
    IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_03 = 5;
    IOMUXC_SW_PAD_CTL_PAD_GPIO_B0_03 = IOMUXC_PAD_DSE(7);
    IOMUXC_GPR_GPR27 = 0xFFFFFFFF;
    GPIO7_GDIR |= (1 << 3);

    for(;;) {
        volatile unsigned int i = 0;

        // Set PIN 13 HIGH
        GPIO7_DR_SET = (1 << 3);

        // Poor man's delay
        while(i < 10000000) {
            i++;
        }

        i = 0;

        // Set PIN 13 LOW
        GPIO7_DR_CLEAR = (1 << 3);

        // Poor man's delay
        while(i < 10000000) {
            i++;
        }
    }    
    // while(1);
}

// #define portControlRegister(pin) ((digital_pin_to_info_PGM[(pin)].pad))
// #define portConfigRegister(pin)  ((digital_pin_to_info_PGM[(pin)].mux))
// void i2c_begin() {
//     // use 24 MHz clock
//     CCM_CSCDR2 = (CCM_CSCDR2 & ~CCM_CSCDR2_LPI2C_CLK_PODF(63)) | CCM_CSCDR2_LPI2C_CLK_SEL;
//     hardware.clock_gate_register |= hardware.clock_gate_mask;
//     port->MCR = LPI2C_MCR_RST;
//     setClock(100000);

//     // Setup SDA register
//     *(portControlRegister(hardware.sda_pins[sda_pin_index_].pin)) = PINCONFIG;
//     *(portConfigRegister(hardware.sda_pins[sda_pin_index_].pin)) = hardware.sda_pins[sda_pin_index_].mux_val;
//     if (hardware.sda_pins[sda_pin_index_].select_input_register) {
//         *(hardware.sda_pins[sda_pin_index_].select_input_register) =  hardware.sda_pins[sda_pin_index_].select_val;
//     }

//     // setup SCL register
//     *(portControlRegister(hardware.scl_pins[scl_pin_index_].pin)) = PINCONFIG;
//     *(portConfigRegister(hardware.scl_pins[scl_pin_index_].pin)) = hardware.scl_pins[scl_pin_index_].mux_val;
//     if (hardware.scl_pins[scl_pin_index_].select_input_register) {
//         *(hardware.scl_pins[scl_pin_index_].select_input_register) =  hardware.scl_pins[scl_pin_index_].select_val;
//     }
// }