#include "../include/sgtl5000.h"
#include "../include/wire_c.h"
#include "../include/libs/Arduino.h"

#define ANA_POWER_LINEOUT_POWERUP           (1 << 0)
#define ANA_POWER_ADC_POWERUP               (1 << 1)
#define ANA_POWER_CAPLESS_HEADPHONE_POWERUP (1 << 2)
#define ANA_POWER_LINEOUT_POWERUP           (1 << 3)
#define ANA_POWER_DAC_POWERUP               (1 << 4)
#define ANA_POWER_REFTOP_POWERUP            (1 << 5)
#define ANA_POWER_ADC_MONO                  (1 << 6)
#define ANA_POWER_VAG_POWERUP               (1 << 7)
#define ANA_POWER_VCOAMP_POWERUP            (1 << 8)
#define ANA_POWER_LINREG_D_POWERUP          (1 << 9)
#define ANA_POWER_PLL_POWERUP               (1 << 10)
#define ANA_POWER_VDDC_CHRGPMP_POWERUP      (1 << 11)
#define ANA_POWER_STARTUP_POWERUP           (1 << 12)
#define ANA_POWER_LINREG_SIMPLE_POWERUP     (1 << 13)
#define ANA_POWER_DAC_MONO                  (1 << 14)

void sgtl5000_setup(void) {
    Wire_begin();
    delay(5);
    sgtl5000_write(CHIP_ANA_POWER, 0x4060);
}

void audio_test(void) {
    setup_sgtl5000();
    while (1) {
		    
    }
}

