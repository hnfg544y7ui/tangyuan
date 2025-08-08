#ifndef __CPU_DAC_H__
#define __CPU_DAC_H__


#include "generic/typedef.h"
#include "os/os_api.h"
#include "audio_src.h"
#include "system/spinlock.h"
#include "audio_def.h"
#include "audio_general.h"

/***************************************************************************
  							Audio DAC Features
Notes:以下为芯片规格定义，不可修改，仅供引用
***************************************************************************/
#define AUDIO_DAC_CHANNEL_NUM				4	//DAC通道数
#define AUDIO_ADDA_IRQ_MULTIPLEX_ENABLE			//DAC和ADC中断入口复用使能


/************************************************************************************************************
                                       AUDIO DAC 相关配置

1.  DAC 的输出声道数和输出方式由 TCFG_AUDIO_DAC_CONNECT_MODE 和 TCFG_AUDIO_DAC_MODE 来决定。
    下表是不同组合的支持情况，如果支持该组合配置表格中为使用的DAC输出引脚，不支持的则为 not support。

    +-------------------------------------+------------------+---------------+----------------------------+
    |                                     | DAC_MODE_SINGLE  | DAC_MODE_DIFF | DAC_MODE_VCMO (VCMO就是RL) |
    +-------------------------------------+------------------+---------------+----------------------------+
    |DAC_OUTPUT_MONO_L           (1 声道) | FL               |  FL-FR        |  FL-VCMO                   |
    |DAC_OUTPUT_LR               (2 声道) | FL   FR          |  FL-FR  RL-RR |  FL-VCMO  FR-VCMO          |
    |DAC_OUTPUT_FRONT_LR_REAR_L  (3 声道) | FL   FR   RL     |  not support  |  not support               |
    |DAC_OUTPUT_FRONT_LR_REAR_R  (3 声道) |   not support    |  not support  |  FL-VCMO  FR-VCMO  RR-VCMO |
    |DAC_OUTPUT_FRONT_LR_REAR_LR (4 声道) | FL   FR   RL  RR |  not support  |  not support               |
    +-------------------------------------+------------------+---------------+----------------------------+

2.  DAC 支持高性能模式（DAC_MODE_HIGH_PERFORMANCE）和低功耗模式（DAC_MODE_LOW_POWER ），
    用户可依据实际场景配置 DAC_PERFORMANCE_MODE 选择。

3.  TCFG_DAC_POWER_LEVEL 控制DAC供电电压，外部硬件VCM引脚有挂电容的选择 AUDIO_VCM_CAP_LEVEL1/2/3/4/5，
    VCM引脚没挂电容的选择 AUDIO_VCM_CAPLESS_LEVEL1/2/3/4，数字越大输出幅度越大，功耗越高。
************************************************************************************************************/

// TCFG_AUDIO_DAC_CONNECT_MODE
//#define DAC_OUTPUT_MONO_L                  (0)   //左声道
//#define DAC_OUTPUT_LR                      (2)   //立体声
//#define DAC_OUTPUT_FRONT_LR_REAR_L         (7)   //三声道单端输出 前L+前R+后L (不可设置vcmo公共端)
//#define DAC_OUTPUT_FRONT_LR_REAR_R         (8)   //三声道单端输出 前L+前R+后R (可设置vcmo公共端)
//#define DAC_OUTPUT_FRONT_LR_REAR_LR        (9)   //四声道单端输出

//#define DAC_OUTPUT_MONO_R                  (1)   // 未使用
//#define DAC_OUTPUT_MONO_LR_DIFF            (3)   // 未使用
//#define DAC_OUTPUT_DUAL_LR_DIFF            (6)   // 未使用


/************************************
             dac 供电模式
************************************/
// TCFG_DAC_POWER_LEVEL
#define	AUDIO_VCM_CAP_LEVEL1	           (0)	// VCM-cap, VCM = 1.2v
#define	AUDIO_VCM_CAP_LEVEL2	           (1)  // VCM-cap, VCM = 1.3v
#define	AUDIO_VCM_CAP_LEVEL3	           (2)	// VCM-cap, VCM = 1.4v
#define	AUDIO_VCM_CAP_LEVEL4	           (3)	// VCM-cap, VCM = 1.5v
#define	AUDIO_VCM_CAP_LEVEL5	           (4)	// VCM-cap, VCM = 1.6v
#define	AUDIO_VCM_CAPLESS_LEVEL1           (5)	// VCM-capless, DACLDO = 2.4v
#define	AUDIO_VCM_CAPLESS_LEVEL2           (6)	// VCM-capless, DACLDO = 2.6v
#define	AUDIO_VCM_CAPLESS_LEVEL3           (7)	// VCM-capless, DACLDO = 2.8v
#define	AUDIO_VCM_CAPLESS_LEVEL4           (8)	// VCM-capless, DACLDO = 3.0v


#define AUDIO_DAC_SYNC_IDLE             0
#define AUDIO_DAC_SYNC_PROBE            1
#define AUDIO_DAC_SYNC_START            2
#define AUDIO_DAC_SYNC_NO_DATA          3
#define AUDIO_DAC_ALIGN_NO_DATA         4
#define AUDIO_DAC_SYNC_ALIGN_COMPLETE   5
#define AUDIO_DAC_SYNC_KEEP_RATE_DONE   6

#define AUDIO_SRC_SYNC_ENABLE   1
#define SYNC_LOCATION_FLOAT      1
#if SYNC_LOCATION_FLOAT
#define PCM_PHASE_BIT           0
#else
#define PCM_PHASE_BIT           8
#endif

#define DA_LEFT        0
#define DA_RIGHT       1
#define DA_REAR_RIGHT  2
#define DA_REAR_LEFT   3

#define DA_SOUND_NORMAL                 0x0
#define DA_SOUND_RESET                  0x1
#define DA_SOUND_WAIT_RESUME            0x2

#define DACR32_DEFAULT		8192
#define DA_SYNC_INPUT_BITS              20
#define DA_SYNC_MAX_NUM                 (1 << DA_SYNC_INPUT_BITS)


#define DAC_ANALOG_OPEN_PREPARE         (1)
#define DAC_ANALOG_OPEN_FINISH          (2)
#define DAC_ANALOG_CLOSE_PREPARE        (3)
#define DAC_ANALOG_CLOSE_FINISH         (4)

#define DAC_TRIM_SEL_FL_P               0
#define DAC_TRIM_SEL_FL_N               0
#define DAC_TRIM_SEL_FR_P               1
#define DAC_TRIM_SEL_FR_N               1
#define DAC_TRIM_SEL_RL_P               2
#define DAC_TRIM_SEL_RL_N               2
#define DAC_TRIM_SEL_RR_P               3
#define DAC_TRIM_SEL_RR_N               3
#define DAC_TRIM_SEL_FL                 4
#define DAC_TRIM_SEL_FR                 4
#define DAC_TRIM_SEL_RL                 5
#define DAC_TRIM_SEL_RR                 5
#define DAC_TRIM_SEL_VCM                6

#define DAC_TRIM_PA_FL                  0
#define DAC_TRIM_PA_FR                  1
#define DAC_TRIM_PA_RL                  2
#define DAC_TRIM_PA_RR                  3

#define TYPE_DAC_AGAIN      (0x01)
#define TYPE_DAC_DGAIN      (0x02)

//void audio_dac_power_state(u8 state)
//在应用层重定义 audio_dac_power_state 函数可以获取dac模拟开关的状态
struct audio_dac_hdl;

struct dac_platform_data {
    u8 performance_mode;    	// low_power/high_performance
    u8 l_ana_gain;				// 前左声道模拟增益
    u8 r_ana_gain;				// 前右声道模拟增益
    u8 rl_ana_gain;				// 后左声道模拟增益
    u8 rr_ana_gain;				// 后右声道模拟增益
    u8 dcc_level;				// DAC去直流滤波器档位, 0~7:关闭    8~15：开启(档位越大，高通截止点越小)
    u8 bit_width;             	// DAC输出位宽
    u8 power_level;				// 电源挡位
    u8 pa_isel0;                // 电流档位:4~6
    u8 pa_isel1;                // 电流档位:3~6
    u8 pa_isel2;                // 电流档位:3~6
    u16 dma_buf_time_ms; 	    // DAC dma buf 大小
    float fixed_pns;            // 固定pns,单位ms
    u32 max_sample_rate;    	// 支持的最大采样率
};

struct trim_init_param_t {
    u8 clock_mode;
    u8 power_level;
};

struct audio_dac_trim {
    s16 left_p;
    s16 left_n;
    s16 right_p;
    s16 right_n;
    s16 vcomo;
};

// *INDENT-OFF*
struct audio_dac_sync {
    u32 channel : 3;
    u32 start : 1;
    u32 fast_align : 1;
    u32 connect_sync : 1;
    u32 release_by_bt : 1;
    u32 resevered : 1;
    u32 input_num : DA_SYNC_INPUT_BITS;
    int fast_points;
    int keep_points;
    int phase_sub;
    int in_rate;
    int out_rate;
    int bt_clkn;
    int bt_clkn_phase;
#if AUDIO_SRC_SYNC_ENABLE
    struct audio_src_sync_handle *src_sync;
    void *buf;
    int buf_len;
    void *filt_buf;
    int filt_len;
#else
    struct audio_src_base_handle *src_base;
#endif
#if SYNC_LOCATION_FLOAT
    float pcm_position;
#else
    u32 pcm_position;
#endif
    void *priv;
    int (*handler)(void *priv, u8 state);
    void *correct_priv;
    void (*correct_cabllback)(void *priv, int diff);
};

struct audio_dac_fade {
    u8 enable;
    volatile u8 ref_L_gain;
    volatile u8 ref_R_gain;
    int timer;
};

struct audio_dac_hdl {
    u8 state;
    u8 light_state;
    u8 ng_threshold;
    u8 analog_inited;

    u8 gain;
    u8 vol_set_en;
	u8 dvol_mute;             //DAC数字音量是否mute
    u8 volume_enhancement;
    u16 d_volume[4];
    s16 fade_vol;
    s16 fadein_frames;
	u32 dac_dvol;             //记录DAC 停止前数字音量寄存器DAC_VL0的值
	u32 dac_dvol1;            //记录DAC 停止前数字音量寄存器DAC_VL1的值

    u8 channel;
    u8 protect_fadein;
    u8 sound_state;
    /**sniff退出时，dac模拟提前初始化，避免模拟初始化延时,影响起始同步********/
	u8 power_on;
    u8 need_close;
    u8 anc_dac_open;
    u8 dac_read_flag;	//AEC可读取DAC参考数据的标志
	u8 active;
    u8 clk_fre;     // 0:6M   1:6.6666M
    u16 start_ms;
    u16 delay_ms;
    u16 start_points;
    u16 delay_points;
    u16 prepare_points;//未开始让DAC真正跑之前写入的PCM点数
    u16 irq_points;
    s16 protect_time;
    s16 protect_pns;
    u16 unread_samples;             /*未读样点个数*/
    s16 *output_buf;
    u32 output_buf_len;
    u32 sample_rate;
    OS_SEM *sem;
    OS_MUTEX mutex;
    spinlock_t lock;
    const struct dac_platform_data *pd;
	struct audio_dac_noisegate ng;
    void (*fade_handler)(u8 left_gain, u8 right_gain);
	u8 (*is_aec_ref_dac_ch)(void *dac_ch);
	void (*irq_handler_cb)(void);
};



#endif

