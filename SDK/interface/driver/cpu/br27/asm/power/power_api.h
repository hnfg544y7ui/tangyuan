/* file  		power_api.h
 * brief        pmu hardware api
 * date     	2022-5-10
 * version  	V1.0
 * copyright    Copyright:(c)JIELI  2011, All Rights Reserved.
 *
 * attention:
 * abstract
 *
 */

#ifndef __POWER_API__
#define __POWER_API__

#include "power/low_power.h"

//=========================电源参数配置==================================
struct low_power_param {
    //---------------power_config
    u8 vddiom_lev;			//vddiom
    u8 vddiow_lev;			//vddiow

    //--------------lowpower
    u8 config;				//低功耗使能，蓝牙&&系统空闲可进入低功耗
    u8 osc_type;			//低功耗晶振类型，btosc/lrc
    u32 btosc_hz;			//蓝牙晶振频率
    u32 osc_delay_us;		//低功耗晶振起振延时，为预留配置。

    u8 lptmr_flow;			//低功耗参数由用户配置
    u32 t1;
    u32 t2;
    u32 t3_lrc;
    u32 t4_lrc;
    u32 t3_btosc;
    u32 t4_btosc;
};

//config
#define SLEEP_EN                            BIT(0)
#define DEEP_SLEEP_EN                       BIT(1)

//osc_type
enum {
    OSC_TYPE_LRC = 0,
    OSC_TYPE_BT_OSC,
    OSC_TYPE_NULL,
};

//电源模式
enum {
    PWR_LDO15,
    PWR_DCDC15,
};

//==============================软关机参数配置============================
//软关机会复位寄存器，该参数为传给rom配置的参数。
struct soft_flag0_t {
    u8 wdt_dis: 1;
    u8 poweroff: 1;
    u8 flash_port: 2; //00:A first; 01: A only; 10: B only; 11: B first
    u8 lvd_en: 1;
    u8 pmu_en: 1;
    u8 iov2_ldomode: 1;
    u8 res: 1;
};
struct soft_flag1_t {
    u8 usbdp: 2;
    u8 usbdm: 2;
    u8 uart_key_port: 1;
    u8 ldoin: 3;
};
struct soft_flag2_t {
    u8 pb12: 4;
    u8 pb13: 4;
};
struct soft_flag3_t {
    u8 pb14: 4;
    u8 res: 4;
};

struct soft_flag4_t {
    u8 fast_boot: 1;
    u8 flash_stable_delay_sel: 2;
    u8 flash_power_keep: 1;
    u8 skip_flash_reset: 1;
    u8 flash_spi_baud: 1;//0: spi_div1,   1: spi_div2
    u8 res: 2;
};

struct soft_flag5_t {
    u8 mvddio: 4;
    u8 wvbg: 4;
};
struct boot_soft_flag_t {
    union {
        struct soft_flag0_t boot_ctrl;
        u8 value;
    } flag0;
    union {
        struct soft_flag1_t misc;
        u8 value;
    } flag1;
    union {
        struct soft_flag2_t pb12_pb13;
        u8 value;
    } flag2;
    union {
        struct soft_flag3_t pb14_res;
        u8 value;
    } flag3;
    union {
        struct soft_flag4_t fast_boot_ctrl;
        u8 value;
    } flag4;
    union {
        struct soft_flag5_t level;
        u8 value;
    } flag5;
    u32 poweron;
};

enum soft_flag_io_stage {
    SOFTFLAG_HIGH_RESISTANCE,
    SOFTFLAG_PU,
    SOFTFLAG_PD,

    SOFTFLAG_OUT0,
    SOFTFLAG_OUT0_HD0,
    SOFTFLAG_OUT0_HD,
    SOFTFLAG_OUT0_HD0_HD,

    SOFTFLAG_OUT1,
    SOFTFLAG_OUT1_HD0,
    SOFTFLAG_OUT1_HD,
    SOFTFLAG_OUT1_HD0_HD,
};

//==============================电源接口============================
#define AT_VOLATILE_RAM_POWER        	AT(.power_driver.data)
#define AT_VOLATILE_RAM_CODE_POWER      AT(.power_driver.text.cache.L1)

void power_set_mode(u8 mode);

void power_config_keep_vddio(u8 en);

void power_config_keep_nvdd(u8 en);

void power_config_rtc_clk(u8 en);

void power_config_lpctmu_en(u8 en);

void power_config_lpnfc_en(u8 en);

void power_init(const struct low_power_param *param);

void p11_code_mpu_protect(u8 enable);

void power_early_init(u32 arg);

//==============================soff接口============================
void mask_softflag_config(struct boot_soft_flag_t *softflag);

void soff_latch_release();

void power_set_callback(u8 mode, void (*powerdown_enter)(u8 step), void (*powerdown_exit)(u32), void (*soft_poweroff_enter)(void));

//==============================system接口============================
u32 get_lptmr_usec(void);

#endif
