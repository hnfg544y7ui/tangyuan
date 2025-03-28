#ifndef  __EFUSE_H__
#define  __EFUSE_H__


u32 efuse_get_chip_id();
u32 efuse_get_bttx_pwr_trim();
u32 efuse_get_vbat_trim_4p20();
u32 efuse_get_vbat_trim_4p35();
u32 efuse_get_charge_cur_trim();
u32 efuse_get_gpadc_vbg_trim();
u32 get_chip_version();
u32 efuse_get_dcvdd_trim();
u32 efuse_get_usb_tx_ldo_trim();
u32 efuse_get_lrc_trim();

u32 efuse_get_wvdd_lev_trim();


void efuse_init();

#define      CHIP_VERSION_A     0x00
#define      CHIP_VERSION_B     0x01

#endif  /*EFUSE_H*/
