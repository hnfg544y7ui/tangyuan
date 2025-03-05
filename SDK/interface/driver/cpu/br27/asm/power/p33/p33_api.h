#ifndef __P33_API_H__
#define __P33_API_H__

void adc_pmu_ch_select(u32 ch);

void sdpg_config(int enable);

void p33_dump();

bool is_charge_online();

enum {
    DVDD_VOL_SEL_0725V = 0,
    DVDD_VOL_SEL_075V,
    DVDD_VOL_SEL_0775V,
    DVDD_VOL_SEL_080V,
    DVDD_VOL_SEL_0825V,
    DVDD_VOL_SEL_0850V,
    DVDD_VOL_SEL_0875V,
    DVDD_VOL_SEL_090V,
    DVDD_VOL_SEL_0925V,
    DVDD_VOL_SEL_0950V,
    DVDD_VOL_SEL_0975V,
    DVDD_VOL_SEL_100V,
    DVDD_VOL_SEL_1025V,
    DVDD_VOL_SEL_105V,
    DVDD_VOL_SEL_1075V,
    DVDD_VOL_SEL_110V,
};


void dvdd_vol_sel(u8 vol);
u8 get_dvdd_vol_sel();

enum {
    RVDD_VOL_SEL_0725V = 0,
    RVDD_VOL_SEL_075V,
    RVDD_VOL_SEL_0775V,
    RVDD_VOL_SEL_080V,
    RVDD_VOL_SEL_0825V,
    RVDD_VOL_SEL_0850V,
    RVDD_VOL_SEL_0875V,
    RVDD_VOL_SEL_090V,
    RVDD_VOL_SEL_0925V,
    RVDD_VOL_SEL_0950V,
    RVDD_VOL_SEL_0975V,
    RVDD_VOL_SEL_100V,
    RVDD_VOL_SEL_1025V,
    RVDD_VOL_SEL_105V,
    RVDD_VOL_SEL_1075V,
    RVDD_VOL_SEL_110V,
};

void rvdd_vol_sel(u8 vol);
u8 get_rvdd_vol_sel();

enum {
    DCVDD_VOL_SEL_100V = 0,
    DCVDD_VOL_SEL_1025V,
    DCVDD_VOL_SEL_105V,
    DCVDD_VOL_SEL_1075V,
    DCVDD_VOL_SEL_110V,
    DCVDD_VOL_SEL_1125V,
    DCVDD_VOL_SEL_115V,
    DCVDD_VOL_SEL_1175V,
    DCVDD_VOL_SEL_120V,
    DCVDD_VOL_SEL_1225V,
    DCVDD_VOL_SEL_125V,
    DCVDD_VOL_SEL_1275V,
    DCVDD_VOL_SEL_130V,
    DCVDD_VOL_SEL_1325V,
    DCVDD_VOL_SEL_135V,
    DCVDD_VOL_SEL_1375V,
};

void dcvdd_vol_sel(u8 vol);
u8 get_dcvdd_vol_sel();

enum {
    WVDD_VOL_SEL_040V = 0,
    WVDD_VOL_SEL_045V,
    WVDD_VOL_SEL_050V,
    WVDD_VOL_SEL_055V,
    WVDD_VOL_SEL_060V,
    WVDD_VOL_SEL_065V,
    WVDD_VOL_SEL_070V,
    WVDD_VOL_SEL_075V,
    WVDD_VOL_SEL_080V,
    WVDD_VOL_SEL_085V,
    WVDD_VOL_SEL_090V,
    WVDD_VOL_SEL_095V,
    WVDD_VOL_SEL_100V,
    WVDD_VOL_SEL_105V,
    WVDD_VOL_SEL_110V,
    WVDD_VOL_SEL_115V,
};

void wvdd_vol_sel(u8 vol);
void wvdd_load_en(u8 en);
void wvdd_en(u8 en);

enum {
    PVDD_VOL_SEL_040V = 0,
    PVDD_VOL_SEL_045V,
    PVDD_VOL_SEL_050V,
    PVDD_VOL_SEL_055V,
    PVDD_VOL_SEL_060V,
    PVDD_VOL_SEL_065V,
    PVDD_VOL_SEL_070V,
    PVDD_VOL_SEL_075V,
    PVDD_VOL_SEL_080V,
    PVDD_VOL_SEL_085V,
    PVDD_VOL_SEL_090V,
    PVDD_VOL_SEL_095V,
    PVDD_VOL_SEL_100V,
    PVDD_VOL_SEL_105V,
    PVDD_VOL_SEL_110V,
    PVDD_VOL_SEL_115V,
};

void pvdd_config(u32 lev, u32 low_lev, u32 output);
void pvdd_output(u32 output);

enum {
    VDDIOM_VOL_21V = 0,
    VDDIOM_VOL_22V,
    VDDIOM_VOL_23V,
    VDDIOM_VOL_24V,
    VDDIOM_VOL_25V,
    VDDIOM_VOL_26V,
    VDDIOM_VOL_27V,
    VDDIOM_VOL_28V,
    VDDIOM_VOL_29V,
    VDDIOM_VOL_30V,
    VDDIOM_VOL_31V,
    VDDIOM_VOL_32V,
    VDDIOM_VOL_33V,
    VDDIOM_VOL_34V,
    VDDIOM_VOL_35V,
    VDDIOM_VOL_36V,
};

void vddiom_vol_sel(u8 vol);
u8 get_vddiom_vol_sel();

enum {
    VDDIOW_VOL_21V = 0,
    VDDIOW_VOL_22V,
    VDDIOW_VOL_23V,
    VDDIOW_VOL_24V,
    VDDIOW_VOL_25V,
    VDDIOW_VOL_26V,
    VDDIOW_VOL_27V,
    VDDIOW_VOL_28V,
    VDDIOW_VOL_29V,
    VDDIOW_VOL_30V,
    VDDIOW_VOL_31V,
    VDDIOW_VOL_32V,
    VDDIOW_VOL_33V,
    VDDIOW_VOL_34V,
    VDDIOW_VOL_35V,
    VDDIOW_VOL_36V,
};

void vddiow_vol_sel(u8 vol);

u32 get_vddiom_vol();

void dvd_ds_en(u8 en);
u8 get_dvd_ds_en();

/*
1.类型：支持普通io / 模拟io
2.滤波：普通io无滤波参数、模拟io可配置滤波参数
3.边沿：支持普通io单边沿，模拟io可配置双边沿
*/
#define MAX_WAKEUP_PORT     12  //最大同时支持数字io输入个数
#define MAX_WAKEUP_ANA_PORT 3   //最大同时支持模拟io输入个数

#endif
