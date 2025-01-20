#ifndef __LP_NFC_TAG_H__
#define __LP_NFC_TAG_H__

#include "typedef.h"

enum NFC_P2M_EVENT {
    NFC_P2M_CHANGE_BUF_EVENT = 0xaa,
};

enum NFC_M2P_CMD {
    NFC_M2P_UP_BUF_ADDR = 0x66,
    NFC_M2P_INIT,
    NFC_M2P_ENABLE,         //模块使能
    NFC_M2P_DISABLE,        //模块关闭
    NFC_M2P_UP_UID,         //更新UID
    NFC_M2P_UP_BT_MAC,      //更新蓝牙MAC地址
    NFC_M2P_UP_BT_NAME,     //更新蓝牙名
    NFC_M2P_UP_ALL_INFO,    //更新所以信息
};

enum LP_NFC_SOFTOFF_MODE {
    LP_NFC_SOFTOFF_MODE_LEGACY  = 0, //普通关机
    LP_NFC_SOFTOFF_MODE_ADVANCE = 1, //带NFC关机
};

enum LP_NFC_TAG_TYPE {
    CUSTOM_TAG = 1,//自定义标签，可读写
    JL_BT_TAG,//杰理蓝牙标签，只可读
};

struct lp_nfc_tag_platform_data {
    u8 uid[7];
    u8 gain_cfg0: 4;//增益配置0
    u8 gain_cfg1: 4;//增益配置1
    u8 bf_cfg: 3;//ATQ-A响应的bit1~5的防冲突值(只有其中1个bit为1)
    u8 cur_trim_cfg0: 5;//电流校准配置0
    u8 cur_trim_cfg1: 5;//电流校准配置1
    u8 rx_io_mode: 1;//RX 数字or模拟选择
    u8 op_mode: 2;//nfc operat mode
    u8 pause_cfg: 4;//pause dect cfg 滤波时间单位是一个lpnfc_clk周期，用于过滤载波的低脉冲信号
    u8 idle_timeout_time: 4;//空闲超时时间，单位s
    u8 tag_type: 2;//标签类型，0:杰理蓝牙标签(只可读)， 1:空白标签(可读写)
    u8 ctl_wkup_cyc_sel: 3;//检测到主机的灵敏度
    u8 ctl_invalid_cyc_sel: 3;//模拟稳定需要的时间周期的选择：2^(2 + x) 个 nfc_clk
    u8 ctl_valid_time;//单位ms,时间要大于invalid_cyc
    u8 ctl_scan_time;//扫描时间:单位10ms
};

#define LP_NFC_TAG_PLATFORM_DATA_BEGIN(data) \
	static const struct lp_nfc_tag_platform_data data = {


//以下为SDK隐藏配置，用户不能随意更改
#define LP_NFC_TAG_PLATFORM_DATA_END() \
    .gain_cfg0 = 0b0100, \
    .gain_cfg1 = 0b0100, \
    .bf_cfg = 0b010, \
    .cur_trim_cfg0 = 0, \
    .cur_trim_cfg1 = 0, \
    .op_mode = 0b00, \
    .pause_cfg = 0b0010, \
    .idle_timeout_time = 3, \
    .ctl_invalid_cyc_sel = 0b011, \
    .ctl_valid_time = 5, \
    .ctl_scan_time = 1, \
};


void lp_nfc_tag_init(const struct lp_nfc_tag_platform_data *pdata);
void lp_nfc_jl_bt_tag_set_mac(const u8 *bt_mac, u8 set_only);
void lp_nfc_jl_bt_tag_set_name(const char *bt_name, u8 set_only);
void lp_nfc_tag_enable(void);
void lp_nfc_tag_disable(void);
u8 lp_nfc_tag_softoff_mode_query(void);
void lp_nfc_tag_event_irq_handler(void);

#endif

