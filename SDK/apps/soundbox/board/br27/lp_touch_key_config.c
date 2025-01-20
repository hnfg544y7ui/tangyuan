#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".lp_touch_key_config.data.bss")
#pragma data_seg(".lp_touch_key_config.data")
#pragma const_seg(".lp_touch_key_config.text.const")
#pragma code_seg(".lp_touch_key_config.text")
#endif
#include "app_config.h"
#include "syscfg_id.h"
#include "key_driver.h"
#include "cpu/includes.h"
#include "system/init.h"
#include "asm/lp_touch_key_api.h"

#if TCFG_LP_TOUCH_KEY_ENABLE

#define TCFG_LP_TOUCH_KEY0_EN    			1                  		//是否使能触摸按键0 —— PB1
#define TCFG_LP_TOUCH_KEY1_EN    			0                  		//是否使能触摸按键1 —— PB2
#define TCFG_LP_TOUCH_KEY2_EN    			0                  		//是否使能触摸按键2 —— PB3
#define TCFG_LP_TOUCH_KEY3_EN    			0                  		//是否使能触摸按键3 —— PR0
#define TCFG_LP_TOUCH_KEY4_EN    			0                  		//是否使能触摸按键4 —— PR1
#define TCFG_LP_TOUCH_KEY5_EN    			0                  		//是否使能触摸按键5 —— PB4
#define TCFG_LP_TOUCH_KEY6_EN    			0                  		//是否使能触摸按键6 —— PB5
#define TCFG_LP_TOUCH_KEY7_EN    			0                  		//是否使能触摸按键7 —— PB6
#define TCFG_LP_TOUCH_KEY8_EN    			0                  		//是否使能触摸按键8 —— PB7
#define TCFG_LP_TOUCH_KEY9_EN    			0                  		//是否使能触摸按键9 —— PB8

#define TCFG_LP_TOUCH_KEY0_WAKEUP_EN        1                  		//是否使能触摸按键0可以软关机低功耗唤醒
#define TCFG_LP_TOUCH_KEY1_WAKEUP_EN        0                  		//是否使能触摸按键1可以软关机低功耗唤醒
#define TCFG_LP_TOUCH_KEY2_WAKEUP_EN        0                  		//是否使能触摸按键2可以软关机低功耗唤醒
#define TCFG_LP_TOUCH_KEY3_WAKEUP_EN        0                  		//是否使能触摸按键3可以软关机低功耗唤醒
#define TCFG_LP_TOUCH_KEY4_WAKEUP_EN        0                  		//是否使能触摸按键4可以软关机低功耗唤醒
#define TCFG_LP_TOUCH_KEY5_WAKEUP_EN        0                  		//是否使能触摸按键5可以软关机低功耗唤醒
#define TCFG_LP_TOUCH_KEY6_WAKEUP_EN        0                  		//是否使能触摸按键6可以软关机低功耗唤醒
#define TCFG_LP_TOUCH_KEY7_WAKEUP_EN        0                  		//是否使能触摸按键7可以软关机低功耗唤醒
#define TCFG_LP_TOUCH_KEY8_WAKEUP_EN        0                  		//是否使能触摸按键8可以软关机低功耗唤醒
#define TCFG_LP_TOUCH_KEY9_WAKEUP_EN        0                  		//是否使能触摸按键9可以软关机低功耗唤醒

//两个按键以上，可以做简单的滑动处理
#define TCFG_LP_SLIDE_KEY_ENABLE            0                       //是否使能触摸按键的滑动功能

#define TCFG_LP_EARTCH_KEY_CH               2                       //带入耳检测流程的按键号：0/1/2/3/4
#define TCFG_LP_EARTCH_KEY_REF_CH           1                       //入耳检测算法需要的另一个按键通道作为参考：0/1/2/3/4, 即入耳检测至少要使能两个触摸通道
#define TCFG_LP_EARTCH_SOFT_INEAR_VAL       3000                    //入耳检测算法需要的入耳阈值，参考文档设置
#define TCFG_LP_EARTCH_SOFT_OUTEAR_VAL      2000                    //入耳检测算法需要的出耳阈值，参考文档设置

//电容检测灵敏度级数配置(范围: 0 ~ 9)
//该参数配置与触摸时电容变化量有关, 触摸时电容变化量跟模具厚度, 触摸片材质, 面积等有关,
//触摸时电容变化量越小, 推荐选择灵敏度级数越大,
//触摸时电容变化量越大, 推荐选择灵敏度级数越小,
//用户可以从灵敏度级数为0开始调试, 级数逐渐增大, 直到选择一个合适的灵敏度配置值.
#define TCFG_LP_TOUCH_KEY0_SENSITIVITY		5 	//触摸按键0电容检测灵敏度配置(级数0 ~ 9)
#define TCFG_LP_TOUCH_KEY1_SENSITIVITY		5 	//触摸按键1电容检测灵敏度配置(级数0 ~ 9)
#define TCFG_LP_TOUCH_KEY2_SENSITIVITY		5 	//触摸按键2电容检测灵敏度配置(级数0 ~ 9)
#define TCFG_LP_TOUCH_KEY3_SENSITIVITY		5 	//触摸按键3电容检测灵敏度配置(级数0 ~ 9)
#define TCFG_LP_TOUCH_KEY4_SENSITIVITY		5 	//触摸按键4电容检测灵敏度配置(级数0 ~ 9)
#define TCFG_LP_TOUCH_KEY5_SENSITIVITY		5 	//触摸按键5电容检测灵敏度配置(级数0 ~ 9)
#define TCFG_LP_TOUCH_KEY6_SENSITIVITY		5 	//触摸按键6电容检测灵敏度配置(级数0 ~ 9)
#define TCFG_LP_TOUCH_KEY7_SENSITIVITY		5 	//触摸按键7电容检测灵敏度配置(级数0 ~ 9)
#define TCFG_LP_TOUCH_KEY8_SENSITIVITY		5 	//触摸按键8电容检测灵敏度配置(级数0 ~ 9)
#define TCFG_LP_TOUCH_KEY9_SENSITIVITY		5 	//触摸按键9电容检测灵敏度配置(级数0 ~ 9)


//如果 IO按键与触摸按键有冲突，则需要关掉 IO按键的 宏
#if 1
//取消外置触摸的一些宏定义
#ifdef TCFG_IOKEY_ENABLE
#undef TCFG_IOKEY_ENABLE
#define TCFG_IOKEY_ENABLE 					DISABLE_THIS_MOUDLE
#endif /* #ifdef TCFG_IOKEY_ENABLE */
#endif

//如果 AD按键与触摸按键有冲突，则需要关掉 AD按键的 宏
#if 1
//取消外置触摸的一些宏定义
#ifdef TCFG_ADKEY_ENABLE
#undef TCFG_ADKEY_ENABLE
#define TCFG_ADKEY_ENABLE 					DISABLE_THIS_MOUDLE
#endif /* #ifdef TCFG_ADKEY_ENABLE */
#endif

#if TCFG_LP_EARTCH_KEY_ENABLE
#ifdef TCFG_EARTCH_EVENT_HANDLE_ENABLE
#undef TCFG_EARTCH_EVENT_HANDLE_ENABLE
#endif
#define TCFG_EARTCH_EVENT_HANDLE_ENABLE 	ENABLE_THIS_MOUDLE 		//入耳检测事件APP流程处理使能
#endif /* #if TCFG_LP_EARTCH_KEY_ENABLE */


const struct lp_touch_key_platform_data lp_touch_key_config = {
    /**/
    .ch[0].enable = TCFG_LP_TOUCH_KEY0_EN,
    .ch[0].wakeup_enable = TCFG_LP_TOUCH_KEY0_WAKEUP_EN,
    .ch[0].port = IO_PORTB_01,
    .ch[0].sensitivity = TCFG_LP_TOUCH_KEY0_SENSITIVITY,
    .ch[0].key_value = 0,

    .ch[1].enable = TCFG_LP_TOUCH_KEY1_EN,
    .ch[1].wakeup_enable = TCFG_LP_TOUCH_KEY1_WAKEUP_EN,
    .ch[1].port = IO_PORTB_02,
    .ch[1].sensitivity = TCFG_LP_TOUCH_KEY1_SENSITIVITY,
    .ch[1].key_value = 1,

    .ch[2].enable = TCFG_LP_TOUCH_KEY2_EN,
    .ch[2].wakeup_enable = TCFG_LP_TOUCH_KEY2_WAKEUP_EN,
    .ch[2].port = IO_PORTB_03,
    .ch[2].sensitivity = TCFG_LP_TOUCH_KEY2_SENSITIVITY,
    .ch[2].key_value = 2,

    .ch[3].enable = TCFG_LP_TOUCH_KEY3_EN,
    .ch[3].wakeup_enable = TCFG_LP_TOUCH_KEY3_WAKEUP_EN,
    .ch[3].port = IO_PORT_PR_00,
    .ch[3].sensitivity = TCFG_LP_TOUCH_KEY3_SENSITIVITY,
    .ch[3].key_value = 3,

    .ch[4].enable = TCFG_LP_TOUCH_KEY4_EN,
    .ch[4].wakeup_enable = TCFG_LP_TOUCH_KEY4_WAKEUP_EN,
    .ch[4].port = IO_PORT_PR_01,
    .ch[4].sensitivity = TCFG_LP_TOUCH_KEY4_SENSITIVITY,
    .ch[4].key_value = 4,

    .ch[5].enable = TCFG_LP_TOUCH_KEY5_EN,
    .ch[5].wakeup_enable = TCFG_LP_TOUCH_KEY5_WAKEUP_EN,
    .ch[5].port = IO_PORTB_04,
    .ch[5].sensitivity = TCFG_LP_TOUCH_KEY5_SENSITIVITY,
    .ch[5].key_value = 5,

    .ch[6].enable = TCFG_LP_TOUCH_KEY6_EN,
    .ch[6].wakeup_enable = TCFG_LP_TOUCH_KEY6_WAKEUP_EN,
    .ch[6].port = IO_PORTB_05,
    .ch[6].sensitivity = TCFG_LP_TOUCH_KEY6_SENSITIVITY,
    .ch[6].key_value = 6,

    .ch[7].enable = TCFG_LP_TOUCH_KEY7_EN,
    .ch[7].wakeup_enable = TCFG_LP_TOUCH_KEY7_WAKEUP_EN,
    .ch[7].port = IO_PORTB_06,
    .ch[7].sensitivity = TCFG_LP_TOUCH_KEY7_SENSITIVITY,
    .ch[7].key_value = 7,

    .ch[8].enable = TCFG_LP_TOUCH_KEY8_EN,
    .ch[8].wakeup_enable = TCFG_LP_TOUCH_KEY8_WAKEUP_EN,
    .ch[8].port = IO_PORTB_07,
    .ch[8].sensitivity = TCFG_LP_TOUCH_KEY8_SENSITIVITY,
    .ch[8].key_value = 8,

    .ch[9].enable = TCFG_LP_TOUCH_KEY9_EN,
    .ch[9].wakeup_enable = TCFG_LP_TOUCH_KEY9_WAKEUP_EN,
    .ch[9].port = IO_PORTB_08,
    .ch[9].sensitivity = TCFG_LP_TOUCH_KEY9_SENSITIVITY,
    .ch[9].key_value = 9,

    //
    .slide_mode_en = TCFG_LP_SLIDE_KEY_ENABLE,
    .slide_mode_key_value = 3,

    //
    .eartch_en = TCFG_LP_EARTCH_KEY_ENABLE,
    .eartch_ch = TCFG_LP_EARTCH_KEY_CH,
    .eartch_ref_ch = TCFG_LP_EARTCH_KEY_REF_CH,
    .eartch_soft_inear_val = TCFG_LP_EARTCH_SOFT_INEAR_VAL,
    .eartch_soft_outear_val = TCFG_LP_EARTCH_SOFT_OUTEAR_VAL,
};


int board_lp_touch_key_config()
{
    lp_touch_key_init(&lp_touch_key_config);

    return 0;
}
platform_initcall(board_lp_touch_key_config);

#endif
