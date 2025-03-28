#include "../include/imxrt_hw.h"
#include "../include/libs/core_pins.h"
#include "../include/sgtl5000.h"
#include "../include/wire_c.h"
#include <stdlib.h>

//TODO separate file
#define DMAMEM __attribute__ ((section(".dmabuffers"), used))

#define I2C_ADDR 0x0A

// TODO is 32bit int faster than 16 bit?
uint32_t sgtl5000_read(uint16_t reg) {
  Wire_begin_transmission(I2C_ADDR);
  Wire_write(reg >> 8);
  Wire_write(reg & 0xFF);
  if (Wire_end_transmission(false) != 0) {
    return 0;
  }
  if (Wire_request_from(I2C_ADDR, 2, true) < 2) {
    return 0;
  }
  uint32_t val = Wire_read() << 8;
  val |= Wire_read();
  return val;
}

bool sgtl5000_write(uint16_t reg, uint16_t val) {
  Wire_begin_transmission(I2C_ADDR);
  Wire_write(reg >> 8);
  Wire_write(reg & 0xFF);
  Wire_write(val >> 8);
  Wire_write(val & 0xFF);
  if (Wire_end_transmission(true) == 0) {
    return true;
  }
  return false;
}

bool sgtl5000_setup(void) {
  Wire_begin();
  delay(5);
  bool r = sgtl5000_write(CHIP_ANA_POWER, ANA_POWER_REFTOP_POWERUP |
                                              ANA_POWER_ADC_MONO |
                                              ANA_POWER_DAC_MONO);
  if (!r)
    return false;
  sgtl5000_write(CHIP_LINREG_CTRL, LINREG_VDDC_VOLTAGE(0x0C /*1.0V*/) |
                                       LINREG_VDDC_FROM_VDDIO |
                                       LINREG_VDDC_SOURCE_OVERRIDE);
  sgtl5000_write(CHIP_REF_CTRL, REF_VAG_VOLTAGE(0x1F /*1.575V*/) |
                                    REF_BIAS_CTRL(0x1 /*+12.5%*/));
  sgtl5000_write(CHIP_LINE_OUT_CTRL,
                 LINE_OUT_CTRL_VAG_VOLTAGE(0x22 /*1.65V*/) |
                     LINE_OUT_CTRL_OUT_CURRENT(0x3C /*0.54mA*/));
  sgtl5000_write(CHIP_SHORT_CTRL, 0x4446); // fuck it, I cba to work this out
  sgtl5000_write(CHIP_ANA_CTRL, 0x0137);   // enable zero cross detectors

  sgtl5000_write(CHIP_ANA_POWER,
                 ANA_POWER_CAPLESS_HEADPHONE_POWERUP | ANA_POWER_DAC_POWERUP |
                     ANA_POWER_HEADPHONE_POWERUP | ANA_POWER_DAC_MONO |
                     ANA_POWER_VAG_POWERUP | ANA_POWER_REFTOP_POWERUP);
  sgtl5000_write(CHIP_DIG_POWER, DIG_POWER_DAC | DIG_POWER_I2S_IN);
  delay(400);
  sgtl5000_write(CHIP_CLK_CTRL, 0x0004); // 44.1 kHz, 256*Fs
  sgtl5000_write(CHIP_I2S_CTRL, 0x0030); // SCLK=64*Fs, 16bit, I2S format
  // default signal routing is ok?
  sgtl5000_write(CHIP_SSS_CTRL, 0x0010);    // ADC->I2S, I2S->DAC
  sgtl5000_write(CHIP_ADCDAC_CTRL, 0x0000); // disable dac mute
  sgtl5000_write(CHIP_DAC_VOL, 0x3C3C);     // digital gain, 0dB
  sgtl5000_write(CHIP_ANA_HP_CTRL, 0x7F7F); // set volume (lowest level)
  sgtl5000_write(CHIP_ANA_CTRL, 0x0036);    // enable zero cross detectors
  return true;
}

const static int AUDIO_SAMPLE_RATE_EXACT = 44100.0f;

void configure_i2s(void) {
  CCM_CCGR5 |= CCM_CCGR5_SAI1(CCM_CCGR_ON);

  // if either transmitter or receiver is enabled, do nothing
  if ((I2S1_TCSR & I2S_TCSR_TE) != 0 || (I2S1_RCSR & I2S_RCSR_RE) != 0) {
    // if (!only_bclk) // if previous transmitter/receiver only activated BCLK,
    // activate the other clock pins now
    // {
    //   CORE_PIN23_CONFIG = 3;  //1:MCLK
    //   CORE_PIN20_CONFIG = 3;  //1:RX_SYNC (LRCLK)
    // }
    return;
  }

  // PLL:
  int fs = AUDIO_SAMPLE_RATE_EXACT;
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  int n1 = 4; // SAI prescaler 4 => (n1*n2) = multiple of 4
  int n2 = 1 + (24000000 * 27) / (fs * 256 * n1);

  double C = ((double)fs * 256 * n1 * n2) / 24000000;
  int c0 = C;
  int c2 = 10000;
  int c1 = C * c2 - (c0 * c2);
  set_audioClock(c0, c1, c2, false);

  // clear SAI1_CLK register locations
  CCM_CSCMR1 =
      (CCM_CSCMR1 & ~(CCM_CSCMR1_SAI1_CLK_SEL_MASK)) |
      CCM_CSCMR1_SAI1_CLK_SEL(2); // &0x03 // (0,1,2): PLL3PFD0, PLL5, PLL4
  CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK |
                               CCM_CS1CDR_SAI1_CLK_PODF_MASK)) |
               CCM_CS1CDR_SAI1_CLK_PRED(n1 - 1)    // &0x07
               | CCM_CS1CDR_SAI1_CLK_PODF(n2 - 1); // &0x3f

  // Select MCLK
  IOMUXC_GPR_GPR1 =
      (IOMUXC_GPR_GPR1 & ~(IOMUXC_GPR_GPR1_SAI1_MCLK1_SEL_MASK)) |
      (IOMUXC_GPR_GPR1_SAI1_MCLK_DIR | IOMUXC_GPR_GPR1_SAI1_MCLK1_SEL(0));

  // if (!only_bclk)
  // {
  // CORE_PIN23_CONFIG = 3;  //1:MCLK
  // CORE_PIN20_CONFIG = 3;  //1:RX_SYNC  (LRCLK)
  // }
  CORE_PIN21_CONFIG = 3; // 1:RX_BCLK

  int rsync = 0;
  int tsync = 1;

  I2S1_TMR = 0;
  // I2S1_TCSR = (1<<25); //Reset
  I2S1_TCR1 = I2S_TCR1_RFW(1);
  I2S1_TCR2 = I2S_TCR2_SYNC(tsync) | I2S_TCR2_BCP // sync=0; tx is async;
              | (I2S_TCR2_BCD | I2S_TCR2_DIV((1)) | I2S_TCR2_MSEL(1));
  I2S1_TCR3 = I2S_TCR3_TCE;
  I2S1_TCR4 = I2S_TCR4_FRSZ((2 - 1)) | I2S_TCR4_SYWD((32 - 1)) | I2S_TCR4_MF |
              I2S_TCR4_FSD | I2S_TCR4_FSE | I2S_TCR4_FSP;
  I2S1_TCR5 =
      I2S_TCR5_WNW((32 - 1)) | I2S_TCR5_W0W((32 - 1)) | I2S_TCR5_FBT((32 - 1));

  I2S1_RMR = 0;
  // I2S1_RCSR = (1<<25); //Reset
  I2S1_RCR1 = I2S_RCR1_RFW(1);
  I2S1_RCR2 = I2S_RCR2_SYNC(rsync) | I2S_RCR2_BCP // sync=0; rx is async;
              | (I2S_RCR2_BCD | I2S_RCR2_DIV((1)) | I2S_RCR2_MSEL(1));
  I2S1_RCR3 = I2S_RCR3_RCE;
  I2S1_RCR4 = I2S_RCR4_FRSZ((2 - 1)) | I2S_RCR4_SYWD((32 - 1)) | I2S_RCR4_MF |
              I2S_RCR4_FSE | I2S_RCR4_FSP | I2S_RCR4_FSD;
  I2S1_RCR5 =
      I2S_RCR5_WNW((32 - 1)) | I2S_RCR5_W0W((32 - 1)) | I2S_RCR5_FBT((32 - 1));
}

typedef struct audio_block_struct {
	uint8_t  ref_count;
	uint8_t  reserved1;
	uint16_t memory_pool_index;
	int16_t  data[AUDIO_BLOCK_SAMPLES];
} audio_block_t;


audio_block_t *block_left_1st = NULL;
audio_block_t *block_right_1st = NULL;
audio_block_t *block_left_2nd = NULL;
audio_block_t *block_right_2nd = NULL;
uint16_t  block_left_offset = 0;
uint16_t  block_right_offset = 0;
bool update_responsibility = false;
DMAChannel dma(false);
DMAMEM __attribute__((aligned(32))) static uint32_t i2s_tx_buffer[AUDIO_BLOCK_SAMPLES];

void begin_i2s(void) {
  dma.begin(true); // Allocate the DMA channel first

  block_left_1st = NULL;
  block_right_1st = NULL;

  config_i2s();

  CORE_PIN7_CONFIG = 3; // 1:TX_DATA0
  dma.TCD->SADDR = i2s_tx_buffer;
  dma.TCD->SOFF = 2;
  dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
  dma.TCD->NBYTES_MLNO = 2;
  dma.TCD->SLAST = -sizeof(i2s_tx_buffer);
  dma.TCD->DOFF = 0;
  dma.TCD->CITER_ELINKNO = sizeof(i2s_tx_buffer) / 2;
  dma.TCD->DLASTSGA = 0;
  dma.TCD->BITER_ELINKNO = sizeof(i2s_tx_buffer) / 2;
  dma.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
  dma.TCD->DADDR = (void *)((uint32_t)&I2S1_TDR0 + 2);
  dma.triggerAtHardwareEvent(DMAMUX_SOURCE_SAI1_TX);
  dma.enable();

  I2S1_RCSR |= I2S_RCSR_RE | I2S_RCSR_BCE;
  I2S1_TCSR = I2S_TCSR_TE | I2S_TCSR_BCE | I2S_TCSR_FRDE;
  update_responsibility = update_setup();
  dma.attachInterrupt(isr);
}

void audio_test(void) {
  sgtl5000_setup();
  while (1) {
  }
}
