#ifndef  __GPADC_HW_H__
#define  __GPADC_HW_H__
//br27
#include "generic/typedef.h"
#define ADC_CH_MASK_TYPE_SEL	0xffff0000
#define ADC_CH_MASK_CH_SEL	    0x0000ffff

#define ADC_CH_TYPE_BT     	(0x0<<16)
#define ADC_CH_TYPE_PMU    	(0x1<<16)
#define ADC_CH_TYPE_AUDIO  	(0x2<<16)
#define ADC_CH_TYPE_LPCTM  	(0x3<<16)
#define ADC_CH_TYPE_RTC    	(0x4<<16)
#define ADC_CH_TYPE_PLL1   	(0x5<<16)
#define ADC_CH_TYPE_FM     	(0x6<<16)
#define ADC_CH_TYPE_HUSB   	(0x7<<16)
#define ADC_CH_TYPE_IO		(0x10<<16)

#define ADC_CH_BT_			(ADC_CH_TYPE_BT | 0x0)
#define ADC_CH_PMU_VBG  	(ADC_CH_TYPE_PMU | 0x0)//MVBG/WVBG
#define ADC_CH_PMU_VSW  	(ADC_CH_TYPE_PMU | 0x1)
#define ADC_CH_PMU_PROGI	(ADC_CH_TYPE_PMU | 0x2)
#define ADC_CH_PMU_PROGF	(ADC_CH_TYPE_PMU | 0x3)
#define ADC_CH_PMU_VTEMP	(ADC_CH_TYPE_PMU | 0x4)
#define ADC_CH_PMU_VPWR 	(ADC_CH_TYPE_PMU | 0x5) //1/4vpwr
#define ADC_CH_PMU_VBAT 	(ADC_CH_TYPE_PMU | 0x6)  //1/4vbat
#define ADC_CH_PMU_VBAT_2	(ADC_CH_TYPE_PMU | 0x7)
#define ADC_CH_PMU_VB17  	(ADC_CH_TYPE_PMU | 0x8)
#define ADC_CH_PMU_VQPS  	(ADC_CH_TYPE_PMU | 0x9)
#define ADC_CH_PMU_DCVD		(ADC_CH_TYPE_PMU | 0xa)
#define ADC_CH_PMU_DVDD		(ADC_CH_TYPE_PMU | 0xb)
#define ADC_CH_PMU_RVDD  	(ADC_CH_TYPE_PMU | 0xc)
#define ADC_CH_PMU_TWVD  	(ADC_CH_TYPE_PMU | 0xd)
#define ADC_CH_PMU_PVDD  	(ADC_CH_TYPE_PMU | 0xe)
#define ADC_CH_PMU_EVDD  	(ADC_CH_TYPE_PMU | 0xf)
#define ADC_CH_AUDIO_VADADC	(ADC_CH_TYPE_AUDIO | BIT(0))
#define ADC_CH_AUDIO_VCM   	(ADC_CH_TYPE_AUDIO | BIT(1))
#define ADC_CH_AUDIO_VBGLDO	(ADC_CH_TYPE_AUDIO | BIT(2))
#define ADC_CH_AUDIO_HPVDD 	(ADC_CH_TYPE_AUDIO | BIT(3))
#define ADC_CH_AUDIO_RN    	(ADC_CH_TYPE_AUDIO | BIT(4))
#define ADC_CH_AUDIO_RP    	(ADC_CH_TYPE_AUDIO | BIT(5))
#define ADC_CH_AUDIO_LN    	(ADC_CH_TYPE_AUDIO | BIT(6))
#define ADC_CH_AUDIO_LP    	(ADC_CH_TYPE_AUDIO | BIT(7))
#define ADC_CH_AUDIO_DACVDD	(ADC_CH_TYPE_AUDIO | BIT(8))
#define ADC_CH_LPCTM_		(ADC_CH_TYPE_LPCTM | 0x0)
#define ADC_CH_RTC_PR0    	(ADC_CH_TYPE_RTC | 0x0)
#define ADC_CH_RTC_PR1    	(ADC_CH_TYPE_RTC | 0x1)
#define ADC_CH_RTC_OSC32K 	(ADC_CH_TYPE_RTC | 0x2)
#define ADC_CH_RTC_RTCVDD 	(ADC_CH_TYPE_RTC | 0x3)// 3/4 RTCVDD
#define ADC_CH_PLL1_	    (ADC_CH_TYPE_PLL1 | 0x0)
#define ADC_CH_FM_			(ADC_CH_TYPE_FM | 0x0)
#define ADC_CH_HUSB_		(ADC_CH_TYPE_HUSB | 0x0)
#define ADC_CH_IO_PA2    (ADC_CH_TYPE_IO | 0x0)
#define ADC_CH_IO_PA5    (ADC_CH_TYPE_IO | 0x1)
#define ADC_CH_IO_PA6    (ADC_CH_TYPE_IO | 0x2)
#define ADC_CH_IO_PA10   (ADC_CH_TYPE_IO | 0x3)
#define ADC_CH_IO_PA12   (ADC_CH_TYPE_IO | 0x4)
#define ADC_CH_IO_PB1    (ADC_CH_TYPE_IO | 0x5)
#define ADC_CH_IO_PB3    (ADC_CH_TYPE_IO | 0x6)
#define ADC_CH_IO_PB4    (ADC_CH_TYPE_IO | 0x7)
#define ADC_CH_IO_PB8    (ADC_CH_TYPE_IO | 0x8)
#define ADC_CH_IO_PB10   (ADC_CH_TYPE_IO | 0x9)
#define ADC_CH_IO_PB11   (ADC_CH_TYPE_IO | 0xA)
#define ADC_CH_IO_PC2    (ADC_CH_TYPE_IO | 0xB)
#define ADC_CH_IO_PC3    (ADC_CH_TYPE_IO | 0xC)
#define ADC_CH_IO_PC4    (ADC_CH_TYPE_IO | 0xD)
#define ADC_CH_IO_PC5    (ADC_CH_TYPE_IO | 0xE)
#define ADC_CH_IO_PC6    (ADC_CH_TYPE_IO | 0xF)

#define     ADC_VBG_CENTER  800 //VBG基准值
#define     ADC_VBG_TRIM_STEP     0   //
#define     ADC_VBG_DATA_WIDTH    0

enum AD_CH {
    AD_CH_BT = ADC_CH_BT_,

    AD_CH_PMU_VBG = ADC_CH_PMU_VBG,
    AD_CH_PMU_VSW,
    AD_CH_PMU_PROGI,
    AD_CH_PMU_PROGF,
    AD_CH_PMU_VTEMP,
    AD_CH_PMU_VPWR,	//1/4 VPWR
    AD_CH_PMU_VBAT, // 1/4 VBAT
    AD_CH_PMU_VBAT_2, // 1/2 VBAT  SDK中禁止使用
    AD_CH_PMU_VB17,
    AD_CH_PMU_VQPS,
    AD_CH_PMU_DCVD,
    AD_CH_PMU_DVDD,
    AD_CH_PMU_RVDD,
    AD_CH_PMU_TWVD,
    AD_CH_PMU_PVDD,
    AD_CH_PMU_EVDD,

    AD_CH_AUDIO = ADC_CH_TYPE_AUDIO, //防编译报错，该宏非法
    AD_CH_AUDIO_VADADC = ADC_CH_AUDIO_VADADC,
    AD_CH_AUDIO_VCM = ADC_CH_AUDIO_VCM,
    AD_CH_AUDIO_VBGLDO = ADC_CH_AUDIO_VBGLDO,
    AD_CH_AUDIO_HPVDD = ADC_CH_AUDIO_HPVDD,
    AD_CH_AUDIO_RN = ADC_CH_AUDIO_RN,
    AD_CH_AUDIO_RP = ADC_CH_AUDIO_RP,
    AD_CH_AUDIO_LN = ADC_CH_AUDIO_LN,
    AD_CH_AUDIO_LP = ADC_CH_AUDIO_LP,
    AD_CH_AUDIO_DACVDD = ADC_CH_AUDIO_DACVDD,

    AD_CH_LPCTM = ADC_CH_LPCTM_,

    AD_CH_RTC_PR0 = ADC_CH_RTC_PR0,
    AD_CH_RTC_PR1,
    AD_CH_RTC_OSC32K,
    AD_CH_RTC_RTCVDD,

    AD_CH_PLL1 = ADC_CH_PLL1_,

    AD_CH_FM = ADC_CH_FM_,

    AD_CH_HUSB = ADC_CH_HUSB_,

    AD_CH_IO_PA2 = ADC_CH_IO_PA2,
    AD_CH_IO_PA5,
    AD_CH_IO_PA6,
    AD_CH_IO_PA10,
    AD_CH_IO_PA12,
    AD_CH_IO_PB1,
    AD_CH_IO_PB3,
    AD_CH_IO_PB4,
    AD_CH_IO_PB8,
    AD_CH_IO_PB10,
    AD_CH_IO_PB11,
    AD_CH_IO_PC2,
    AD_CH_IO_PC3,
    AD_CH_IO_PC4,
    AD_CH_IO_PC5,
    AD_CH_IO_PC6,

    AD_CH_IOVDD = 0xffffffff,
};

#define AD_CH_LDOREF    AD_CH_PMU_VBG


void adc_pmu_ch_select(u32 ch);
// void adc_vdc13_save();
// void adc_vdc13_restore();
void adc_rtc_ch_select(u32 ch);
void adc_rtc_detect_en(u32 on);
void adc_rtc_pd(u32 value);
u32 efuse_get_gpadc_vbg_trim();
int get_hpvdd_voltage(void);


#endif  /*GPADC_HW_H*/
