#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".audio_setup.data.bss")
#pragma data_seg(".audio_setup.data")
#pragma const_seg(".audio_setup.text.const")
#pragma code_seg(".audio_setup.text")
#endif
/*
 ******************************************************************************************
 *							Audio Setup
 *
 * Discription: 音频模块初始化，配置，调试等
 *
 * Notes:
 ******************************************************************************************
 */
#include "cpu/includes.h"
#include "media/includes.h"
#include "system/includes.h"
#include "app_config.h"
#include "audio_config.h"
#include "sdk_config.h"
#include "audio_adc.h"
#include "asm/audio_common.h"
#include "media/audio_energy_detect.h"
#include "adc_file.h"
#include "linein_file.h"
#include "fm_file.h"
#include "spdif_file.h"
#include "audio_demo/audio_demo.h"
#include "update.h"
#include "user_cfg.h"
#include "gpio_config.h"
#include "media/audio_general.h"
#include "audio_spdif_master.h"
#include "media_config.h"
#include "effects/audio_eq.h"

#if (SYS_VOL_TYPE == VOL_TYPE_DIGITAL)
#include "audio_dvol.h"
#endif

#if TCFG_AUDIO_DUT_ENABLE
#include "audio_dut_control.h"
#endif/*TCFG_AUDIO_DUT_ENABLE*/

#if TCFG_SMART_VOICE_ENABLE
#include "smart_voice.h"
#endif

struct audio_dac_hdl dac_hdl;
struct audio_adc_hdl adc_hdl;

struct audio_spdif_hdl spdif_hdl;

typedef struct {
    u8 audio_inited;
    atomic_t ref;
} audio_setup_t;
audio_setup_t audio_setup = {0};
#define __this      (&audio_setup)

___interrupt
static void audio_irq_handler()
{
    if (JL_AUDIO->DAC_CON & BIT(7)) {
        JL_AUDIO->DAC_CON |= BIT(6);
        if (JL_AUDIO->DAC_CON & BIT(5)) {
            audio_dac_irq_handler(&dac_hdl);
        }
    }

    if (JL_AUDIO->ADC_CON & BIT(7)) {
        JL_AUDIO->ADC_CON |= BIT(6);
        if ((JL_AUDIO->ADC_CON & BIT(5)) && (JL_AUDIO->ADC_CON & BIT(4))) {
            audio_adc_irq_handler(&adc_hdl);
        }
    }
    asm("csync");
}

struct dac_platform_data dac_data = {
    .performance_mode   = TCFG_DAC_PERFORMANCE_MODE,
    .dma_buf_time_ms    = TCFG_AUDIO_DAC_BUFFER_TIME_MS,
    .dcc_level          = 15,
    .l_ana_gain        = TCFG_AUDIO_FL_CHANNEL_GAIN,
    .r_ana_gain        = TCFG_AUDIO_FR_CHANNEL_GAIN,
    .rl_ana_gain        = TCFG_AUDIO_RL_CHANNEL_GAIN,
    .rr_ana_gain        = TCFG_AUDIO_RR_CHANNEL_GAIN,
#if (TCFG_DAC_PERFORMANCE_MODE == DAC_MODE_HIGH_PERFORMANCE)
    .pa_isel0           = TCFG_AUDIO_DAC_HP_PA_ISEL0,
    .pa_isel1           = TCFG_AUDIO_DAC_HP_PA_ISEL1,
    .pa_isel2           = TCFG_AUDIO_DAC_HP_PA_ISEL2,
#else
    .pa_isel0           = TCFG_AUDIO_DAC_LP_PA_ISEL0,
    .pa_isel1           = TCFG_AUDIO_DAC_LP_PA_ISEL1,
    .pa_isel2           = TCFG_AUDIO_DAC_LP_PA_ISEL2,
#endif
};

#if TCFG_AUDIO_ADC_ENABLE
const struct adc_platform_cfg adc_platform_cfg_table[AUDIO_ADC_MAX_NUM] = {
#if TCFG_ADC0_ENABLE
    [0] = {
        .mic_mode           = TCFG_ADC0_MODE,
        .mic_ain_sel        = TCFG_ADC0_AIN_SEL,
        .mic_bias_sel       = TCFG_ADC0_BIAS_SEL,
        .mic_bias_rsel      = TCFG_ADC0_BIAS_RSEL,
        .power_io           = TCFG_ADC0_POWER_IO,
        .mic_dcc            = TCFG_ADC0_DCC_LEVEL,
    },
#endif
#if TCFG_ADC1_ENABLE
    [1] = {
        .mic_mode           = TCFG_ADC1_MODE,
        .mic_ain_sel        = TCFG_ADC1_AIN_SEL,
        .mic_bias_sel       = TCFG_ADC1_BIAS_SEL,
        .mic_bias_rsel      = TCFG_ADC1_BIAS_RSEL,
        .power_io           = TCFG_ADC1_POWER_IO,
        .mic_dcc            = TCFG_ADC1_DCC_LEVEL,
    },
#endif
#if TCFG_ADC2_ENABLE
    [2] = {
        .mic_mode           = TCFG_ADC2_MODE,
        .mic_ain_sel        = TCFG_ADC2_AIN_SEL,
        .mic_bias_sel       = TCFG_ADC2_BIAS_SEL,
        .mic_bias_rsel      = TCFG_ADC2_BIAS_RSEL,
        .power_io           = TCFG_ADC2_POWER_IO,
        .mic_dcc            = TCFG_ADC2_DCC_LEVEL,
    },
#endif
#if TCFG_ADC3_ENABLE
    [3] = {
        .mic_mode           = TCFG_ADC3_MODE,
        .mic_ain_sel        = TCFG_ADC3_AIN_SEL,
        .mic_bias_sel       = TCFG_ADC3_BIAS_SEL,
        .mic_bias_rsel      = TCFG_ADC3_BIAS_RSEL,
        .power_io           = TCFG_ADC3_POWER_IO,
        .mic_dcc            = TCFG_ADC3_DCC_LEVEL,
    },
#endif
};
#endif
int get_dac_channel_num(void)
{
    return dac_hdl.channel;
}

//当两个值都设为0，将跳过trim值检测流程。
const int trim_value_lr_err_max = 600;   // 距离参考值的差值限制 (不建议修改)
const int trim_value_lr_diff_err_max = 300;   // 左右声道的差值限制 (不建议修改)

static void audio_common_initcall()
{
    audio_common_param_t common_param = {0};
    extern u32 efuse_get_audio_vbg_trim();
    common_param.vbg_trim_value = efuse_get_audio_vbg_trim();

#if (TCFG_APP_FM_EN || (TCFG_CLOCK_SYS_SRC == SYS_CLOCK_INPUT_PLL_RCL))
    // FM 模式切频点需要切 DAC 时钟频率，所以 DAC 要选数字时钟
    common_param.clock_mode = AUDIO_COMMON_CLK_DIG_SINGLE;
#else
    common_param.clock_mode = AUDIO_COMMON_CLK_DIF_XOSC;
#endif

    common_param.vcm_cap_en = TCFG_AUDIO_VCM_CAP_EN;
    if (common_param.vcm_cap_en) {
        common_param.power_level = TCFG_DAC_POWER_LEVEL;
    } else {
        common_param.power_level = TCFG_DAC_POWER_LEVEL + 5;
        if (common_param.power_level == 9) {
            printf("!!!!if vcm without cap cannot use power_level 5!!!!");
            common_param.power_level = 8;
        }
    }
    audio_common_init(&common_param);
    dac_data.power_level = common_param.power_level;
    audio_common_clock_open();
}

void audio_dac_initcall(void)
{
    printf("audio_dac_initcall\n");

    dac_data.max_sample_rate    = AUDIO_DAC_MAX_SAMPLE_RATE;
    dac_data.bit_width = audio_general_out_dev_bit_width();
    audio_dac_init(&dac_hdl, &dac_data);
    //dac_hdl.ng.threshold = 4;			//DAC底噪优化阈值
    //dac_hdl.ng.detect_interval = 200;	//DAC底噪优化检测间隔ms

    audio_common_audio_init((void *)(adc_hdl.mic_param), (void *)(dac_hdl.pd));

#if defined(TCFG_AUDIO_DAC_24BIT_MODE) && TCFG_AUDIO_DAC_24BIT_MODE
    audio_dac_set_bit_mode(&dac_hdl, 1);
#endif

    audio_dac_set_analog_vol(&dac_hdl, 0);

    request_irq(IRQ_AUDIO_IDX, 2, audio_irq_handler, 0);

    struct audio_dac_trim dac_trim;
    int len = syscfg_read(CFG_DAC_TRIM_INFO, &dac_trim, sizeof(struct audio_dac_trim));
    if ((len != sizeof(struct audio_dac_trim)) || (audio_dac_trim_value_check(&dac_trim))) {
        struct trim_init_param_t trim_init = {0};
        int err = audio_dac_do_trim(&dac_hdl, &dac_trim, &trim_init);
        if (err == 0) {
            syscfg_write(CFG_DAC_TRIM_INFO, &dac_trim, sizeof(struct audio_dac_trim));
        }
        printf("dac_trim: err = %d\n", err);
    }
    audio_dac_set_trim_value(&dac_hdl, &dac_trim);

    audio_dac_set_fade_handler(&dac_hdl, NULL, audio_fade_in_fade_out);

    /*硬件SRC模块滤波器buffer设置，可根据最大使用数量设置整体buffer*/
    /* audio_src_base_filt_init(audio_src_hw_filt, sizeof(audio_src_hw_filt)); */
}

#if defined(TCFG_AUDIO_DAC_IO_ENABLE) && TCFG_AUDIO_DAC_IO_ENABLE
static void audio_dac_io_initcall()
{
    struct audio_dac_io_param param = {0};
    param.state[0] = 1;                 //FL声道初始电平状态
    param.state[1] = 1;                 //FR声道初始电平状态
    param.state[2] = 1;                 //RL声道初始电平状态
    param.state[3] = 1;                 //RR声道初始电平状态
    param.irq_points = 512;
    param.channel = BIT(0) | BIT(1) | BIT(2) | BIT(3);    //使能四声道
    param.digital_gain = 8192;         //数字音量：-8192~8192
    //此外IOVDD配置也会影响DAC输出电平
    audio_dac_io_init(&param);
}
#endif

struct audio_adc_private_param adc_private_param = {
    .micldo_vsel   =  3,
};

void audio_input_initcall(void)
{
    printf("audio_input_initcall\n");

    audio_adc_init(&adc_hdl, &adc_private_param);
    adc_hdl.bit_width = audio_general_in_dev_bit_width();

#if TCFG_AUDIO_LINEIN_ENABLE
    audio_linein_file_init();
#endif/*TCFG_AUDIO_LINEIN_ENABLE*/

    audio_adc_file_init();

    audio_all_adc_file_init();

#if TCFG_APP_FM_EN
    audio_fm_file_init();
#endif

#if TCFG_SPDIF_ENABLE
    audio_spdif_file_init();
#endif
}

static void wl_audio_clk_on(void)
{
    JL_WL_AUD->CON0 = 1;
    JL_ASS->CLK_CON |= BIT(0);//audio时钟
}

#ifdef BT_DUT_DAC_INTERFERE
static void write_dac(void *p)
{
    printf("audio_dac_demo_open\n");
    audio_dac_demo_open();
}
#endif

void audio_interfere_bt_dut(void)
{
#ifdef BT_DUT_ADC_INTERFERE
    printf("audio_adc_mic_demo_open\n");
    audio_adc_mic_demo_open(0b0001, 3, 44100, 0);
#endif

#ifdef BT_DUT_DAC_INTERFERE
    os_task_create(write_dac, NULL, 7, 128, 128, "dac_test_task");
#endif
}

/*audio初始化过程不进入低功耗*/
static u8 audio_init_complete(void)
{
    if (!__this->audio_inited) {
        return 0;
    }
    return 1;
}
REGISTER_LP_TARGET(audio_init_lp_target) = {
    .name = "audio_dec_init",
    .is_idle = audio_init_complete,
};

extern int audio_timer_init(void);

/*audio模块初始化*/
static int audio_init()
{
    puts("audio_init\n");
    wl_audio_clk_on();
    audio_general_init();
    audio_input_initcall();

#if (SYS_VOL_TYPE == VOL_TYPE_DIGITAL)
    audio_digital_vol_init(NULL, 0);
#endif

#if (TCFG_DAC_NODE_ENABLE || TCFG_AUDIO_DAC_IO_ENABLE || TCFG_AUDIO_ADC_ENABLE)
    audio_common_initcall();
#endif

#if TCFG_DAC_NODE_ENABLE
    audio_dac_initcall();
#elif (defined(TCFG_AUDIO_DAC_IO_ENABLE) && TCFG_AUDIO_DAC_IO_ENABLE)
    audio_dac_io_initcall();
#elif (defined(TCFG_AUDIO_ADC_ENABLE) && TCFG_AUDIO_ADC_ENABLE)
    request_irq(IRQ_AUDIO_IDX, 2, audio_irq_handler, 0);
#endif
#if TCFG_SPDIF_MASTER_NODE_ENABLE
    SPDIF_MASTER_PARM	spdif_master_cfg;
    spdif_master_cfg.tx_io = IO_PORTB_15;
    spdif_master_cfg.bit_width = audio_general_out_dev_bit_width();
    spdif_master_out_init(&spdif_hdl, &spdif_master_cfg);
#endif
#if TCFG_AUDIO_DUT_ENABLE
    audio_dut_init();
#endif /*TCFG_AUDIO_DUT_ENABLE*/

#if TCFG_EQ_ENABLE
    audio_eq_lib_init();
#endif

    audio_interfere_bt_dut();

#if TCFG_SMART_VOICE_ENABLE
    audio_smart_voice_detect_init(NULL);
#endif /* #if TCFG_SMART_VOICE_ENABLE */

    audio_timer_init();
    __this->audio_inited = 1;
    puts("audio_init succ\n");
    return 0;
}
platform_initcall(audio_init);

static void audio_uninit()
{
#if TCFG_DAC_NODE_ENABLE
    audio_dac_close(&dac_hdl);
#endif
}
platform_uninitcall(audio_uninit);

/*关闭audio相关模块使能*/
void audio_disable_all(void)
{
    //DAC:DACEN
    JL_AUDIO->DAC_CON &= ~BIT(4);
    //ADC:ADCEN
    JL_AUDIO->ADC_CON &= ~BIT(4);
    //EQ:
    JL_EQ->CON0 &= ~BIT(1);
    JL_EQ1->CON0 &= ~BIT(1);
    //FFT:
    JL_FFT->CON = BIT(1);//置1强制关闭模块，不管是否已经运算完成
    //ANC:anc_en anc_start
    //JL_ANC->CON0 &= ~(BIT(1) | BIT(29));
#if TCFG_DAC_NODE_ENABLE
    audio_dac_close(&dac_hdl);
#endif
}

REGISTER_UPDATE_TARGET(audio_update_target) = {
    .name = "audio",
    .driver_close = audio_disable_all,
};

/*
 *自定义dac上电延时时间，具体延时多久应通过示波器测量
 */
#if 1
void dac_power_on_delay()
{
    os_time_dly(50);
}
#endif



extern struct adc_platform_data adc_data;
int read_mic_type_config(void)
{
    MIC_TYPE_CONFIG mic_type;
    int ret = syscfg_read(CFG_MIC_TYPE_ID, &mic_type, sizeof(MIC_TYPE_CONFIG));
    if (ret > 0) {
        ///// >>>>todo
        /* log_info_hexdump((u8 *)&mic_type, sizeof(MIC_TYPE_CONFIG)); */
        /* adc_data.mic_mode = mic_type.type; */
        /* adc_data.mic_bias_res = mic_type.pull_up; */
        /* adc_data.mic_ldo_vsel = mic_type.ldo_lev; */
        return 0;
    }
    return -1;
}


