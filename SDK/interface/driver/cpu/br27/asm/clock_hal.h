#ifndef  __CLOCK_HAL_H__
#define  __CLOCK_HAL_H__




#include "typedef.h"

enum CLK_OUT_SOURCE0 {
    CLK_OUT_SRC0_RTC_OSC,
    CLK_OUT_SRC0_LRC_CLK,
    CLK_OUT_SRC0_STD_12M,
    CLK_OUT_SRC0_STD_24M,
    CLK_OUT_SRC0_STD_48M,
    CLK_OUT_SRC0_SSC_12M,
    CLK_OUT_SRC0_SSC_24M,
    CLK_OUT_SRC0_SSC_48M,
    CLK_OUT_SRC0_BTOSC_24M,
    CLK_OUT_SRC0_BTOSC_48M,
    CLK_OUT_SRC0_SRC_CLK,
    CLK_OUT_SRC0_HSB_CLK,
    CLK_OUT_SRC0_LSB_CLK,
    CLK_OUT_SRC0_PLL_96M,
    CLK_OUT_SRC0_PRR3_RCLK,
    CLK_OUT_SRC0_RC_16M,
    CLK_OUT_SRC0_XOSC_FSCK,
    CLK_OUT_SRC0_IMD_CLK,
    CLK_OUT_SRC0_PSRAM_CLK,
    CLK_OUT_SRC0_ALINK0_CLK,
    CLK_OUT_SRC0_ALINK1_CLK,
    CLK_OUT_SRC0_RF_CKO75M,
    CLK_OUT_SRC0_DCDC_DCK,
    CLK_OUT_SRC0_FMTX_IO_OUT,
    CLK_OUT_SRC0_RING_CLK
};
enum CLK_OUT_SOURCE1 {
    CLK_OUT_SRC1_RTC_OSC = 0,
    CLK_OUT_SRC1_LRC_CLK,
    CLK_OUT_SRC1_BTOSC_24M,
    CLK_OUT_SRC1_BTOSC_48M,
    CLK_OUT_SRC1_USB_PLL_D3P5,
    CLK_OUT_SRC1_USB_PLL_D2P5,
    CLK_OUT_SRC1_USB_PLL_D2P0,
    CLK_OUT_SRC1_USB_PLL_D1P5,
    CLK_OUT_SRC1_USB_PLL_D1P0,
    CLK_OUT_SRC1_SYS_PLL_D3P5,
    CLK_OUT_SRC1_SYS_PLL_D2P5,
    CLK_OUT_SRC1_SYS_PLL_D2P0,
    CLK_OUT_SRC1_SYS_PLL_D1P5,
    CLK_OUT_SRC1_SYS_PLL_D1P0,
};
void clk_out0(u8 gpio, enum CLK_OUT_SOURCE0 clk);
void clk_out1(u8 gpio, enum CLK_OUT_SOURCE0 clk);
void clk_out2(u8 gpio, enum CLK_OUT_SOURCE1 clk, u32 div);
//无 clk_out3

void clk_out0_close(u8 gpio);
void clk_out1_close(u8 gpio);
void clk_out2_close(u8 gpio);



enum CLK_OUT_SOURCE {
    //ch0,ch1.  no div
    CLK_OUT_RTC_OSC = 0x0300,
    CLK_OUT_LRC_CLK,
    CLK_OUT_STD_12M,
    CLK_OUT_STD_24M,
    CLK_OUT_STD_48M,
    CLK_OUT_SSC_12M,
    CLK_OUT_SSC_24M,
    CLK_OUT_SSC_48M,
    CLK_OUT_BTOSC_24M,
    CLK_OUT_BTOSC_48M,
    CLK_OUT_SRC_CLK,
    CLK_OUT_HSB_CLK,
    CLK_OUT_LSB_CLK,
    CLK_OUT_PLL_96M,
    CLK_OUT_PRR3_RCLK,
    CLK_OUT_RC_16M,
    CLK_OUT_XOSC_FSCK,
    CLK_OUT_IMD_CLK,
    CLK_OUT_PSRAM_CLK,
    CLK_OUT_ALINK0_CLK,
    CLK_OUT_ALINK1_CLK,
    CLK_OUT_RF_CKO75M,
    CLK_OUT_DCDC_DCK,
    CLK_OUT_FMTX_IO_OUT,
    CLK_OUT_RING_CLK,

    //ch2.  div:0~63(div1~div64)
    CLK_OUT_RTC_OSC_DIV = 0x0400,
    CLK_OUT_LRC_CLK_DIV,
    CLK_OUT_BTOSC_24M_DIV,
    CLK_OUT_BTOSC_48M_DIV,
    CLK_OUT_USB_PLL_D3P5_DIV,
    CLK_OUT_USB_PLL_D2P5_DIV,
    CLK_OUT_USB_PLL_D2P0_DIV,
    CLK_OUT_USB_PLL_D1P5_DIV,
    CLK_OUT_USB_PLL_D1P0_DIV,
    CLK_OUT_SYS_PLL_D3P5_DIV,
    CLK_OUT_SYS_PLL_D2P5_DIV,
    CLK_OUT_SYS_PLL_D2P0_DIV,
    CLK_OUT_SYS_PLL_D1P5_DIV,
    CLK_OUT_SYS_PLL_D1P0_DIV,
};

#define CLK_OUT_CH_MASK             0x00000007
#define CLK_OUT_CH0_SEL(clk)        SFR(JL_CLOCK->CLK_CON1,0,5,clk)
#define CLK_OUT_CH0_GET_CLK()       ((JL_CLOCK->CLK_CON1>>0)&0x01f)
#define CLK_OUT_CH0_DIV(div)        //no div
#define CLK_OUT_CH0_EN(en)         //no en
#define CLK_OUT0_FIXED_IO_EN(en)   //no en
#define IS_CLK_OUT0_FIXED_IO()     (0)//no fix

#define CLK_OUT_CH1_SEL(clk)        SFR(JL_CLOCK->CLK_CON1,5,5,clk)
#define CLK_OUT_CH1_GET_CLK()       ((JL_CLOCK->CLK_CON1>>5)&0x01f)
#define CLK_OUT_CH1_DIV(div)        //no div
#define CLK_OUT_CH1_EN(en)         //no en
#define CLK_OUT1_FIXED_IO_EN(en)   //no en
#define IS_CLK_OUT1_FIXED_IO()     (0)//no fix

#define CLK_OUT_CH2_SEL(clk)        SFR(JL_CLOCK->CLK_CON1,10,4,clk)
#define CLK_OUT_CH2_GET_CLK()       ((JL_CLOCK->CLK_CON1>>10)&0x0f)
#define CLK_OUT_CH2_DIV(div)        SFR(JL_CLOCK->CLK_CON1,14,6,div)
#define CLK_OUT_CH2_EN(en)         //no en
#define CLK_OUT2_FIXED_IO_EN(en)   //no en
#define IS_CLK_OUT2_FIXED_IO()     (0)//no fix

#define CLK_OUT_CH3_SEL(clk)        //no ch3
#define CLK_OUT_CH3_GET_CLK()       (0)//no ch3
#define CLK_OUT_CH3_DIV(div)        //no div
#define CLK_OUT_CH3_EN(en)         //no en
#define CLK_OUT3_FIXED_IO_EN(en)   //no en
#define IS_CLK_OUT3_FIXED_IO()     (0)//no fix

#define CLK_OUT_CH4_SEL(clk)        //no ch4
#define CLK_OUT_CH4_GET_CLK()       (0)//no ch4
#define CLK_OUT_CH4_DIV(div)        //no div
#define CLK_OUT_CH4_EN(en)         //no en
#define CLK_OUT4_FIXED_IO_EN(en)   //no en
#define IS_CLK_OUT4_FIXED_IO()     (0)//no fix
u32 clk_out_fixed_io_check(u32 gpio);






//for bt
void clk_set_osc_cap(u8 sel_l, u8 sel_r);
u32 clk_get_osc_cap();
u32 get_btosc_info_for_update(void *info);


#define BT_CLOCK_IN(x)          SFR(JL_CLOCK->CLK_CON4,  16,  2,  x)
//for MACRO - BT_CLOCK_IN
enum {
    BT_CLOCK_IN_DISABLE = 0,
    BT_CLOCK_IN_STD_48M,
    BT_CLOCK_IN_SSC_48M,
    BT_CLOCK_IN_PAT_CLK,
};

/********************************************************************************/
#define GPCNT_EN(x)             SFR(JL_GPCNT->CON,  0,  1,  x)
#define GPCNT_CSS(x)            SFR(JL_GPCNT->CON,  1,  5,  x)
//for MACRO - GPCNT_CSS
enum {
    GPCNT_CSS_LSB = 0,
    GPCNT_CSS_OSC,
    GPCNT_CSS_INPUT_CH2,
    GPCNT_CSS_INPUT_CH3,
    GPCNT_CSS_CLOCK_IN,
    GPCNT_CSS_RING,
    GPCNT_CSS_PLL,
    GPCNT_CSS_INTPUT_CH1,
};

#define GPCNT_CLR_PEND(x)       SFR(JL_GPCNT->CON,  30,  1,  x)
#define GPCNT_GTS(x)            SFR(JL_GPCNT->CON,  16,  4,  x)

#define GPCNT_GSS(x)            SFR(JL_GPCNT->CON,  8, 5,  x)
//for MACRO - GPCNT_CSS
enum {
    GPCNT_GSS_LSB = 0,
    GPCNT_GSS_OSC,
    GPCNT_GSS_INPUT_CH14,    //iomap con1[27:24]
    GPCNT_GSS_INPUT_CH15,    //iomap con1[31:28]
    GPCNT_GSS_CLOCK_IN,		 //CLK_CON2[31:28]
    GPCNT_GSS_RING,
    GPCNT_GSS_PLL,
    GPCNT_GSS_INPUT_CH13,   //iomap con1[23:20]
};


#endif  /*CLOCK_HAL_H*/
