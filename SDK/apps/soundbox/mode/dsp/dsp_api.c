#include "system/includes.h"
#include "app_config.h"
#include "app_task.h"
#include "media/includes.h"
#include "tone_player.h"
#include "app_tone.h"
#include "app_main.h"
#include "ui_manage.h"
#include "vm.h"
#include "ui/ui_api.h"
#include "scene_switch.h"
#include "audio_config_def.h"
#include "audio_config.h"
#include "dsp_mode.h"
#include "iis_player.h"
#include "mic_player.h"
#include "effects/audio_llns_dns.h"
#include "effects/audio_plate_reverb.h"
#include "node_param_update.h"
#include "volume_node.h"


#if TCFG_APP_DSP_EN

#define LOG_TAG             "[APP_DSP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

struct dsp_opr {
    s16 volume ;
    u8 onoff;
    u8 audio_state; /*判断dsp模式使用模拟音量还是数字音量*/
};
static struct dsp_opr dsp_hdl = {0};
#define __this 	(&dsp_hdl)

struct dsp_vol {
    //cppcheck-suppress  unusedStructMember
    bool mute;
    s16 dvol_step;
    s16 dvol;
    s16 max_dvol;
    u8 save_vol_cnt;
    u16 save_vol_timer;
};

static u8 dsp_plate_reverb_bypass = 0;
static u8 dsp_llns_dns_bypass1 = 0;
static u8 dsp_llns_dns_bypass2 = 0;
static bool dsp_dvol_mute_flag = 0;
static struct dsp_vol g_dsp_dvol = {0};
static llns_dns_param_tool_set llns_cfg1 = {0};
static llns_dns_param_tool_set llns_cfg2 = {0};

static int le_audio_dsp_volume_pp(void);

// 打开dsp数据流操作
int dsp_start(void)
{
    if (__this->onoff == 1) {
        printf("dsp is aleady start\n");
        return true;
    }

    if (get_dsp_source()) { //根据输入源选择打开 IIS 或 MIC
        iis_player_open();
    } else {
        mic_player_open();
    }
    __this->audio_state = APP_AUDIO_STATE_MUSIC;
    __this->volume = app_audio_get_volume(__this->audio_state);
    __this->onoff = 1;

#if (TCFG_AS_WIRELESS_MIC_DSP_ENABLE)
    char *vol_name = "Vol_DSP_IIS";
    struct volume_cfg cfg = {0};
    int ret = jlstream_get_node_param(NODE_UUID_VOLUME_CTRLER, vol_name, (void *)&cfg, sizeof(struct volume_cfg));
    if (ret > 0) {
        g_dsp_dvol.max_dvol = (cfg.cfg_level_max >= cfg.cur_vol) ? cfg.cfg_level_max : cfg.cur_vol;
        g_dsp_dvol.dvol_step =  g_dsp_dvol.max_dvol / cfg.cfg_level_max;
        ret = syscfg_read(CFG_WIRELESS_MIC0_VOLUME, &g_dsp_dvol.dvol, 2);
        if (ret > 0) {
            if (g_dsp_dvol.dvol != cfg.cur_vol) {
                dsp_set_dvol(g_dsp_dvol.dvol, 0xff); //有记录的音量先用记录的音量
            }
        } else {
            g_dsp_dvol.dvol = cfg.cur_vol;
            dsp_set_dvol(g_dsp_dvol.dvol, 0xff);
        }
        g_printf(" %s dvol:%d\n", __FUNCTION__, g_dsp_dvol.dvol);
    }
#endif

    return true;
}


// stop dsp 数据流
void dsp_stop(void)
{
    if (__this->onoff == 0) {
        printf("dsp is aleady stop\n");
        return;
    }
    if (get_dsp_source()) { //根据输入源选择关闭 IIS 或 MIC
        iis_player_close();
    } else {
        mic_player_close();
    }


    app_audio_set_volume(APP_AUDIO_STATE_MUSIC, __this->volume, 1);
    __this->onoff = 0;
}


/*-------------------------------------------------------------------*/
/**@brief   获取 dsp  播放状态
  @param    无
  @return   1:当前正在打开 0：当前正在关闭
  @note
 */
/*-------------------------------------------------------------------*/
u8 dsp_get_status(void)
{
    return __this->onoff;
}

/*-------------------------------------------------------------------*/
/*@brief   dsp 音量设置函数
  @param    需要设置的音量
  @return
  @note    在dsp 情况下针对了某些情景进行了处理，设置音量需要使用独立接口
*/
/*-------------------------------------------------------------------*/
int dsp_volume_set(s16 vol)
{
    app_audio_set_volume(__this->audio_state, vol, 1);
    printf("dsp vol: %d", __this->volume);
    __this->volume = vol;

    return true;
}

#if (TCFG_AS_WIRELESS_MIC_DSP_ENABLE)
/* --------------------------------------------------------------------------*/
/**
 * @brief 切换reverb参数
 *
 * @param bypass 0xff:切换参数，1：bypass，0：关闭bypass
 */
/* ----------------------------------------------------------------------------*/
void app_plate_reverb_parm_switch(int bypass)
{
    static u8 cfg_index = 0;
    //没有bypass，则切换参数
    if (0xff == bypass) {
        cfg_index++;
    } else {
        dsp_plate_reverb_bypass = bypass;
    }
    g_printf("cfg_index:%d", cfg_index);
    //第二个参数需要与音频框图对应节点名称一致
    int ret = plate_reverb_update_parm_base(0, "PlateRever51011", cfg_index, bypass);
    if (-1 == ret) {
        //读不到节点参数，可能index已经超过可读参数
        cfg_index = 0;
        ret = plate_reverb_update_parm_base(0, "PlateRever51011", cfg_index, bypass);
    }
}

/* --------------------------------------------------------------------------*/
/**
 * @brief 切换llns_dns参数
 *
 * @param bypass 0xff:切换参数，1：bypass，0：关闭bypass
 */
/* ----------------------------------------------------------------------------*/
void app_llns_dns_parm_switch(int bypass)
{
    static u8 cfg_index = 0;

    g_printf("cfg_index:%d, bypass:%d", cfg_index, bypass);

    //没有bypass，则切换参数
    if (0xff == bypass) {
        cfg_index++;
    } else {
        dsp_llns_dns_bypass1 = bypass;
        dsp_llns_dns_bypass2 = bypass;

        dsp_stop();
        dsp_start(); //重新开关数据流是为了重设延时

        return;
    }

    //第二个参数需要与音频框图对应节点名称一致
    int ret = 0;
    ret = llns_dns_update_parm_base(0, "LLNS_DNS3", cfg_index, bypass);
    if (-1 == ret) {
        //读不到节点参数，可能index已经超过可读参数
        cfg_index = 0;
        ret = llns_dns_update_parm_base(0, "LLNS_DNS3", cfg_index, bypass);
    }

    ret = llns_dns_update_parm_base(0, "LLNS_DNS4", cfg_index, bypass);
    if (-1 == ret) {
        //读不到节点参数，可能index已经超过可读参数
        cfg_index = 0;
        ret = llns_dns_update_parm_base(0, "LLNS_DNS4", cfg_index, bypass);
    }
}

static void dsp_volume_save_do(void *priv)
{
    if (++g_dsp_dvol.save_vol_cnt >= 5) {
        sys_timer_del(g_dsp_dvol.save_vol_timer);
        g_dsp_dvol.save_vol_timer = 0;
        g_dsp_dvol.save_vol_cnt = 0;
        printf("save dsp vol:%d\n", g_dsp_dvol.dvol);
        syscfg_write(CFG_WIRELESS_MIC0_VOLUME, &g_dsp_dvol.dvol, 2);
        return;
    }
}

static void dsp_volume_change(void)
{
    g_dsp_dvol.save_vol_cnt = 0;
    if (g_dsp_dvol.save_vol_timer == 0) {
        g_dsp_dvol.save_vol_timer = sys_timer_add(NULL, dsp_volume_save_do, 1000);//中断里不能操作vm 关中断不能操作vm
    }
}

int dsp_set_dvol(u8 vol, s16 mute_en)
{
    if (cpu_in_irq() || cpu_irq_disabled()) {
        printf("[ERROR]CPU in irq or CPU irq disable, Operation not permitted");
        return -EPERM;
    }

    if ((0xff != mute_en) && (0 != mute_en) && (1 != mute_en)) {
        printf("[ERROR]mute_en, Invalid argument");
        return -EINVAL;
    }

    char *vol_name = "Vol_DSP_IIS";
    struct volume_cfg cfg = {0};
    if (0xff == mute_en) {
        cfg.bypass = VOLUME_NODE_CMD_SET_VOL;
        cfg.cur_vol = vol;
    } else {
        cfg.bypass = VOLUME_NODE_CMD_SET_MUTE;
        cfg.cur_vol = mute_en;
    }
    int err = jlstream_set_node_param(NODE_UUID_VOLUME_CTRLER, vol_name, (void *)&cfg, sizeof(struct volume_cfg)) ;
    if (err < 0) {
        return -1;
    }
    printf("dsp dvol name: %s, dsp dvol:%d\n", vol_name, vol);
    if ((0xff == mute_en) && (vol != g_dsp_dvol.dvol)) {
        g_dsp_dvol.dvol = vol;
        dsp_volume_change();
    }
    return 0;
}

void dsp_dvol_mute(bool mute)
{
    static u8 volume_before_muting = 0;

    dsp_dvol_mute_flag = mute;

    if (mute) {
        //mute
        g_dsp_dvol.mute = 1;
        volume_before_muting = g_dsp_dvol.dvol;
        if (!volume_before_muting || volume_before_muting > g_dsp_dvol.max_dvol) {
            printf("volume_before_muting %d record error", volume_before_muting);
            volume_before_muting = g_dsp_dvol.max_dvol;
        }
        dsp_set_dvol(0, mute);
    } else {
        //unmute
        g_dsp_dvol.mute = 0;
        if (!volume_before_muting || volume_before_muting > g_dsp_dvol.max_dvol) {
            printf("volume_before_muting %d record error", volume_before_muting);
            volume_before_muting = g_dsp_dvol.max_dvol;
        }

        dsp_set_dvol(volume_before_muting, mute);
    }
}

void dsp_dvol_up(void)
{
    //mute时不允许调音量
    if (g_dsp_dvol.mute) {
        return;
    }

    if (g_dsp_dvol.dvol < g_dsp_dvol.max_dvol) {
        g_dsp_dvol.dvol += g_dsp_dvol.dvol_step;
        dsp_set_dvol(g_dsp_dvol.dvol, 0xff);
    } else {
        printf("[WARING]dsp volum is max\n");
    }

}

void dsp_dvol_down(void)
{
    //mute时不允许调音量
    if (g_dsp_dvol.mute) {
        return;
    }

    if (g_dsp_dvol.dvol) {
        g_dsp_dvol.dvol -= g_dsp_dvol.dvol_step;
        dsp_set_dvol(g_dsp_dvol.dvol, 0xff);
    } else {
        printf("[WARING]dsp volum is min\n");
    }
}

void dsp_effect_status_init(void)
{
    int ret = 0;

#if TCFG_PLATE_REVERB_NODE_ENABLE
    plate_reverb_param_tool_set reverb_cfg = {0};
    ret = jlstream_read_form_data(0, "PlateRever51011", 0, &reverb_cfg);
    if (!ret) {
        printf("read parm err, %s, %s\n", __func__, "PlateRever51011");
    } else {
        dsp_plate_reverb_bypass = reverb_cfg.is_bypass;
    }
#endif

#if TCFG_LLNS_DNS_NODE_ENABLE
    ret = jlstream_read_form_data(0, "LLNS_DNS3", 0, &llns_cfg1);
    if (!ret) {
        printf("read parm err, %s, %s\n", __func__, "LLNS_DNS3");
    } else {
        dsp_llns_dns_bypass1 = llns_cfg1.is_bypass;
    }

    ret = jlstream_read_form_data(0, "LLNS_DNS4", 0, &llns_cfg2);
    if (!ret) {
        printf("read parm err, %s, %s\n", __func__, "LLNS_DNS4");
    } else {
        dsp_llns_dns_bypass2 = llns_cfg2.is_bypass;
    }
#endif

    dsp_dvol_mute_flag = 0;
}

u8 get_dsp_llns1_bypass_status(void)
{
    return dsp_llns_dns_bypass1;
}

u8 get_dsp_llns2_bypass_status(void)
{
    return dsp_llns_dns_bypass2;
}

u8 get_dsp_reverb_bypass_status(void)
{
    return dsp_plate_reverb_bypass;
}

bool get_dsp_mute_status(void)
{
    return dsp_dvol_mute_flag;
}

#endif

#endif


