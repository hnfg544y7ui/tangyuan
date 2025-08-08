#include "generic/typedef.h"
#include "audio_dac.h"
#include "audio_adc.h"

/*音频模块寄存器跟踪*/
void audio_adda_dump(void) //打印所有的dac,adc寄存器
{
    printf("JL_WL_AUD CON0:%x", JL_WL_AUD->CON0);
    printf("AUD_CON:%x", JL_AUDIO->AUD_CON);
    printf("DAC_CON:%x", JL_AUDIO->DAC_CON);
    printf("ADC_CON:%x", JL_AUDIO->ADC_CON);
    printf("ADC_CON1:%x", JL_AUDIO->ADC_CON1);
    printf("DAC_TM0:%x", JL_AUDIO->DAC_TM0);
    //printf("DAC_DTB:%x", JL_AUDIO->DAC_DTB);
    printf("DAA_CON 0:%x 	1:%x,	2:%x,	3:%x    4:%x,7:%x\n", JL_ADDA->DAA_CON0, JL_ADDA->DAA_CON1, JL_ADDA->DAA_CON2, JL_ADDA->DAA_CON3, JL_ADDA->DAA_CON4, JL_ADDA->DAA_CON7);
    printf("ADA_CON 0:%x	1:%x	2:%x	3:%x	4:%x	5:%x  6:%x 7:%x 8:%x\n", JL_ADDA->ADA_CON0, JL_ADDA->ADA_CON1, JL_ADDA->ADA_CON2, JL_ADDA->ADA_CON3, JL_ADDA->ADA_CON4, JL_ADDA->ADA_CON5, JL_ADDA->ADA_CON6, JL_ADDA->ADA_CON7, JL_ADDA->ADA_CON8);
    printf("ADDA_CON 0:%x	1:%x\n", JL_ADDA->ADDA_CON0, JL_ADDA->ADDA_CON1);
}

/*音频模块配置跟踪*/
void audio_config_dump()
{
    u8 dac_bit_width = ((JL_AUDIO->DAC_CON & BIT(20)) ? 24 : 16);
    u8 adc_bit_width = ((JL_AUDIO->ADC_CON & BIT(20)) ? 24 : 16);
    int dac_dgain_max = 8192;
    int dac_again_max = 7;
    int mic_gain_max = 14;
    u8 dac_dcc = (JL_AUDIO->DAC_CON2 >> 12) & 0xF;
    u8 mic0_dcc = JL_AUDIO->ADC_CON1 & 0xF;
    u8 mic1_dcc = (JL_AUDIO->ADC_CON1 >> 4) & 0xF;
    u8 mic2_dcc = (JL_AUDIO->ADC_CON1 >> 8) & 0xF;
    u8 mic3_dcc = (JL_AUDIO->ADC_CON1 >> 12) & 0xF;
    u8 dac_again_fl = JL_ADDA->DAA_CON1 & 0xF;
    u8 dac_again_fr = (JL_ADDA->DAA_CON1 >> 4) & 0xF;
    u8 dac_again_rl = (JL_ADDA->DAA_CON1 >> 8) & 0xF;
    u8 dac_again_rr = (JL_ADDA->DAA_CON1 >> 12) & 0xF;
    u32 dac_dgain_fl = JL_AUDIO->DAC_VL0 & 0xFFFF;
    u32 dac_dgain_fr = (JL_AUDIO->DAC_VL0 >> 16) & 0xFFFF;
    u32 dac_dgain_rl = JL_AUDIO->DAC_VL1 & 0xFFFF;
    u32 dac_dgain_rr = (JL_AUDIO->DAC_VL1 >> 16) & 0xFFFF;
    u8 mic0_0_6 = (JL_ADDA->ADA_CON0 >> 26) & 0x1;
    u8 mic1_0_6 = (JL_ADDA->ADA_CON1 >> 26) & 0x1;
    u8 mic2_0_6 = (JL_ADDA->ADA_CON2 >> 26) & 0x1;
    u8 mic3_0_6 = (JL_ADDA->ADA_CON3 >> 26) & 0x1;
    u8 mic0_gain = (JL_ADDA->ADA_CON0 >> 21) & 0x1F;
    u8 mic1_gain = (JL_ADDA->ADA_CON1 >> 21) & 0x1F;
    u8 mic2_gain = (JL_ADDA->ADA_CON2 >> 21) & 0x1F;
    u8 mic3_gain = (JL_ADDA->ADA_CON3 >> 21) & 0x1F;

    printf("[ADC]BitWidth:%d,DCC:%d,%d,%d,%d\n", adc_bit_width, mic0_dcc, mic1_dcc, mic2_dcc, mic3_dcc);
    printf("[ADC]Gain(Max:%d):%d,%d,%d,%d,6dB_Boost:%d,%d,%d,%d,\n", mic_gain_max, mic0_gain, mic1_gain, mic2_gain, mic3_gain, \
           mic0_0_6, mic1_0_6, mic2_0_6, mic3_0_6);

    printf("[DAC]BitWidth:%d,DCC:%d\n", dac_bit_width, dac_dcc);
    printf("[DAC]AGain(Max:%d):%d,%d,%d,%d,DGain(Max:%d):%d,%d,%d,%d\n", dac_again_max, dac_again_fl, dac_again_fr,  \
           dac_again_rl, dac_again_rr,  \
           dac_dgain_max, dac_dgain_fl, dac_dgain_fr,  \
           dac_dgain_rl, dac_dgain_rr);
}
