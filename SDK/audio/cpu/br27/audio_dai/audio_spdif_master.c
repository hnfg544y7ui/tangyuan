#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".audio_spdif_master.data.bss")
#pragma data_seg(".audio_spdif_master.data")
#pragma const_seg(".audio_spdif_master.text.const")
#pragma code_seg(".audio_spdif_master.text")
#endif
#include "includes.h"
#include "asm/includes.h"
#include "media/includes.h"
#include "system/includes.h"
/* #include "asm/clock.h" */
#include "audio_spdif_master.h"
#include "audio_spdif_slave.h"
#include "audio_config.h"
#include "sync/audio_syncts.h"

/* #define SPDIF_DEBUG_INFO */
#ifdef SPDIF_DEBUG_INFO
#define spdif_printf  r_printf
#else
#define spdif_printf(...)
#endif

#define SPDIF_STATE_INIT        0
#define SPDIF_STATE_OPEN        1
#define SPDIF_STATE_START       2
#define SPDIF_STATE_PREPARE     3
#define SPDIF_STATE_STOP        4
#define SPDIF_STATE_PAUSE       5
#define SPDIF_STATE_POWER_OFF   6

#define AUDIO_CFIFO_IDLE      0
#define AUDIO_CFIFO_INITED    1
#define AUDIO_CFIFO_START     2
#define AUDIO_CFIFO_CLOSE     3

const int config_audio_spdif_mix_enable = 1;
/* struct audio_spdif_hdl spdif_handler; */

#define IRQ_MIN_POINTS      60

static struct audio_spdif_hdl *spdif_hdl = NULL;

int iis_adapter_link_to_syncts_check(void *syncts);
int audio_spdif_syncts_enter_update_frame(struct audio_spdif_channel *ch);
int audio_spdif_syncts_exit_update_frame(struct audio_spdif_channel *ch);
int audio_spdif_channel_buffered_frames(struct audio_spdif_channel *ch);
static int audio_spdif_channel_fifo_write(struct audio_spdif_channel *ch, void *data, int len, u8 is_fixed_data);


/* static SPDIF_MASTER_PARM *__this = NULL; */

static int spdif_set_fifo_start_and_delay(struct audio_spdif_hdl *spdif);
int audio_spdif_set_protect_time(struct audio_spdif_hdl *spdif, int time, void *priv, void (*feedback)(void *));
static void audio_spdif_fifo_start(struct audio_spdif_hdl *spdif);

static void spdif_master_info_dump()
{
    spdif_printf("JL_SPDIF->SM_CLK = 0x%x\n", JL_SPDIF->SM_CLK);
    spdif_printf("JL_SPDIF->SM_CON = 0x%x\n", JL_SPDIF->SM_CON);
    spdif_printf("JL_SPDIF->SM_ABADR = 0x%x\n", JL_SPDIF->SM_ABADR);
    spdif_printf("JL_SPDIF->SM_ABUFLEN = x%x\n", JL_SPDIF->SM_ABUFLEN);
    spdif_printf("JL_SPDIF->SM_AHPTR = 0x%x\n", JL_SPDIF->SM_AHPTR);
    spdif_printf("JL_SPDIF->SM_ASPTR = 0x%x\n", JL_SPDIF->SM_ASPTR);
    spdif_printf("JL_SPDIF->SM_ASHN = 0x%x\n", JL_SPDIF->SM_ASHN);
    spdif_printf("JL_SPDIF->SM_APNS = 0x%x\n", JL_SPDIF->SM_APNS);
    spdif_printf("JL_SPDIF->SM_IBADR = 0x%x\n", JL_SPDIF->SM_IBADR);
    spdif_printf("JL_SPDIF->SM_IBUFLEN = x%x\n", JL_SPDIF->SM_IBUFLEN);
    spdif_printf("JL_SPDIF->SM_IHPTR = 0x%x\n", JL_SPDIF->SM_IHPTR);
    spdif_printf("JL_CLOCK->CLK_CON5[13:11] = 0x%x\n", (JL_CLOCK->CLK_CON5 >> 11) & 0b111);
}

___interrupt
static void spdif_master_isr()
{
    if (INF_IS_PND()) {
        INF_PND_CLR();
#if 0
        if (__this->inf_isr_cb) {
            __this->inf_isr_cb(__this->inf_buf, __this->inf_dma_len / 2);
        }
#endif
    }

    if (DAT_IS_PND()) {
#if 0
        s16 *buf_addr = NULL;
        buf_addr = (s16 *)(__this->data_buf + JL_SPDIF->SM_ASPTR);

        if (__this->data_isr_cb) {
            __this->data_isr_cb(buf_addr, __this->data_dma_len / 2);
        }
        JL_SPDIF->SM_ASHN = __this->data_dma_len / 8;
        /* SET_DATABUF_WRITE_BYTE(__this->data_dma_len / 2); */
#else
        putchar('D');
#endif
        DAT_PND_CLR();
    }
}

static void spdif_master_io_init(u8 gpio)
{
    gpio_hw_set_direction(IO_PORT_SPILT(gpio), 0);//0:out, 1:in
    gpio_hw_set_die(IO_PORT_SPILT(gpio), 0);
    gpio_set_fun_output_port(gpio, FO_SPDIF_DO, 1, 1);

    // test io 2
    /* gpio_hw_set_direction(IO_PORT_SPILT(IO_PORTB_08), 0);//0:out, 1:in */
    /* gpio_hw_set_die(IO_PORT_SPILT(IO_PORTB_08), 0); */
    /* gpio_set_fun_output_port(IO_PORTB_08, FO_SPDIF_DO, 1, 1); */

    /* OUTPUT_ANA_SEL(0); */
    IO_IN_TO_OUT_EN(0);//禁止从机输入映射到输出
    IO_OUTPUT_SEL(0);//输出IO选择主机信号
}

int spdif_master_init(struct audio_spdif_hdl *hd_spdif)
{
    SM_RESET();


    SM_DAT_LEN(hd_spdif->output_buf_len / 4);
    SM_INF_LEN(hd_spdif->output_inf_buf_len / 8);
    SM_SET_APNS(hd_spdif->output_buf_len / 16);


    SM_DATA_IE(1);
    /* SM_INFO_IE(1); */

    SM_AUX_DAT_L(0);
    SM_AUX_DAT_R(0);
    SM_VAL_FLAG_L(0);
    SM_VAL_FLAG_R(0);

    SM_DAT_MODE(hd_spdif->data_mode);
    /* SM_AUX_SEL(__this->data_mode); */
    SM_AUX_SEL(hd_spdif->data_mode ? 0 : 1); //1:辅助位使用音频数据地位（对应24bit位宽）;0:发送 Chl_1_aux Chl_r_aux寄存器值

    if (hd_spdif->output_buf) {
        JL_SPDIF->SM_ABADR = (volatile unsigned int)hd_spdif->output_buf;
    }

    if (hd_spdif->output_inf_buf) {
        JL_SPDIF->SM_IBADR = (volatile unsigned int)hd_spdif->output_inf_buf;
    }
    SW_VAL_USER(0);

    SFR(JL_CLOCK->CLK_CON5, 11, 3, 2);
    int spdif_tx_clk = SPDIF_TX_CLK;
    int integer = spdif_tx_clk / hd_spdif->sample_rate / 128;
    float decimal = (float)spdif_tx_clk / hd_spdif->sample_rate / 128 - (float)(integer);
    int int_decimal;
    if (decimal > 0.5) {
        integer = integer + 1;
        int_decimal = (float)((decimal - 1) * 32768);
    } else {
        int_decimal = (float)(decimal * 32768);
    }
    spdif_printf("spdif_master : clk = %d, integer = %d, decimal= %d\n", spdif_tx_clk, integer, int_decimal);

    SM_CLK_INT_DIV(integer);
    SM_CLK_FRAC_DIV(int_decimal);

    SM_CLK_DIV_MODE(0);
    SM_CLK_DIV_NSEL(0);


    DAT_PND_CLR();
    INF_PND_CLR();
    spdif_master_io_init(hd_spdif->tx_io);
    request_irq(IRQ_SPDIF_M_IDX, 3, spdif_master_isr, 0);

    return 0;
}

int spdif_master_start(struct audio_spdif_hdl *spdif)
{
    memset(spdif->output_buf, 0x0, spdif->output_buf_len);
    os_mutex_pend(&spdif->mutex, 0);
    spdif->state = SPDIF_STATE_PREPARE;
    spdif->prepare_points = 0;
    spdif->protect_pns = 0;
    spdif->unread_samples = 0;


    os_mutex_post(&spdif->mutex);

    spdif_set_fifo_start_and_delay(spdif);
    audio_spdif_set_protect_time(spdif, 1, NULL, NULL);
    spdif->protect_pns = spdif->protect_time ? (spdif->protect_time * spdif->sample_rate / 1000) : 0;

    JL_SPDIF->SM_APNS = spdif->protect_pns ? spdif->protect_pns : IRQ_MIN_POINTS;
    /* JL_SPDIF->SM_AHPTR = 0; */
    /* JL_SPDIF->SM_ASPTR = 0; */
    /* JL_SPDIF->SM_ASHN  = 0; */
    spdif->prepare_points = spdif->start_points;
    if (!config_audio_spdif_mix_enable) {
        puts(" spdif fifo _start \n\n");
        audio_spdif_fifo_start(spdif);
    }

    /* SM_OUT_EN(1); */
    /* SM_EN(1); */
    spdif_printf("spdif_master_start\n");
    spdif_master_info_dump();
    return 0;
}

int spdif_master_stop(struct audio_spdif_hdl *spdif)
{
    if (spdif->state != SPDIF_STATE_PREPARE && spdif->state != SPDIF_STATE_START) {
        return 0;
    }
    SM_OUT_EN(0);
    SM_EN(0);
    SM_RESET();
    spdif->state = SPDIF_STATE_STOP;
    spdif->fifo_state = AUDIO_CFIFO_CLOSE;
    return 0;
}

void spdif_master_uninit()
{
    /* spdif_master_stop(); */

    INF_PND_CLR();
    DAT_PND_CLR();
#if 0
    if (__this->data_buf) {
        free(__this->data_buf);
        __this->data_buf = NULL;
    }

    if (__this->inf_buf) {
        free(__this->inf_buf);
        __this->inf_buf = NULL;
    }
    __this = NULL;
#endif
    spdif_printf("spdif_master_exit OK\n");
}

__attribute__((weak)) u16 spdif_start_points(void)
{
    return 320;
}

static int spdif_set_fifo_start_and_delay(struct audio_spdif_hdl *spdif)
{
    if (spdif->sample_rate) {
        /*这里处理SPDIF设置最大的延时点数*/
        spdif->delay_points = ((spdif->sample_rate / (1000 / spdif->delay_ms)) >> 2) << 2;
        if (spdif->delay_points > JL_SPDIF->SM_ABUFLEN) {
            spdif->delay_points = JL_SPDIF->SM_ABUFLEN;
        }
        spdif->irq_points = ((spdif->sample_rate / 400) >> 2) << 2;
        if (spdif->irq_points > 512) {
            spdif->irq_points = 512;
        }

        spdif->start_points = ((spdif->sample_rate / (1000 / spdif->start_ms)) >> 2) << 2;
        u16 min_once_points = ((spdif->delay_points - spdif->irq_points - 1) >> 2) << 2;
        if (spdif->start_points > min_once_points) {
            spdif->start_points = min_once_points;
        }

        u16 start_points_min = spdif_start_points();
        if ((spdif->start_points < start_points_min) && (spdif->delay_points > start_points_min)) {
            spdif->start_points = start_points_min;
        }
        log_i("spdif delay set : %d, %d, %d\n", spdif->delay_points, spdif->irq_points,
              spdif->start_points);
    }

    return 0;
}
int audio_spdif_set_sample_rate(struct audio_spdif_hdl *spdif, int sample_rate)
{
    int reg;

    os_mutex_pend(&spdif->mutex, 0);

    spdif->sample_rate = sample_rate;
    os_mutex_post(&spdif->mutex);

    /*log_info("sample %d\n", spdif->sample_rate);*/

    return 0;
}

int audio_spdif_get_sample_rate(struct audio_spdif_hdl *spdif)
{
    return spdif->sample_rate;
}

int audio_spdif_set_protect_time(struct audio_spdif_hdl *spdif, int time, void *priv, void (*feedback)(void *))
{
    spin_lock(&spdif->lock);
    spdif->protect_time = time;
    spdif->protect_pns = 0;
    /* spdif->feedback_priv = priv; */
    /* spdif->underrun_feedback = feedback; */
    spin_unlock(&spdif->lock);
    return 0;
}

static void audio_spdif_release_fifo(void *_spdif)
{
    struct audio_spdif_hdl *spdif = (struct audio_spdif_hdl *)_spdif;

    /* JL_AUDIO->DAC_SWN = spdif->prepare_points; */
}

static void audio_spdif_fifo_start(struct audio_spdif_hdl *spdif)
{
    if (spdif->state != SPDIF_STATE_PREPARE) {
        /* log_e("spdif module on error, state : 0x%x\n", spdif->state); */
        return;
    }
    /*!!!这里0不写入，DAC SWN写0会导致HRP不停向后跑*/
    if (spdif->prepare_points) {
        JL_SPDIF->SM_ASHN = spdif->prepare_points;
        __asm_csync();
    }
    /* printf("\n--func=%s spdif->prepare_points %d \n", __FUNCTION__,spdif->prepare_points); */
    spin_lock(&spdif->lock);
    SM_OUT_EN(1);
    SM_EN(1);
    spdif->state = SPDIF_STATE_START;
    spin_unlock(&spdif->lock);
    /* log_info("audio_spdif_fifo_start : 0x%x, %d, DAC_CON = 0x%x\n", dac->prepare_points, dac->gain, JL_AUDIO->DAC_CON); */
}

u8 audio_spdif_is_working(struct audio_spdif_hdl *hd_spdif)
{
    if (!hd_spdif) {
        return 0;
    }
    spin_lock(&hd_spdif->lock);
    if (hd_spdif->state == SPDIF_STATE_START) {
        spin_unlock(&hd_spdif->lock);
        return 1;
    }
    spin_unlock(&hd_spdif->lock);

    return 0;
}
int audio_spdif_channel_buffered_frames(struct audio_spdif_channel *ch)
{
    struct audio_spdif_hdl *hdl = (struct audio_spdif_hdl *)ch->hd_spdif;
    if (!hdl) {
        return 0;
    }

    int buffered_frames = 0;

    os_mutex_pend(&hdl->mutex, 0);
    if (ch->state != AUDIO_CFIFO_START) {
        goto ret_value;
    }


    buffered_frames = ((JL_SPDIF->SM_ABUFLEN - JL_SPDIF->SM_ASHN - 1) >> ch->fifo.bit_wide) - audio_cfifo_channel_unread_diff_samples(&ch->fifo);
    if (buffered_frames < 0) {
        buffered_frames = 0;
    }

ret_value:
    os_mutex_post(&hdl->mutex);

    return buffered_frames;
}

int audio_spdif_data_len(void *private_data)
{
    struct audio_spdif_channel *ch = (struct audio_spdif_channel *)private_data;
    return audio_spdif_channel_buffered_frames(ch);
}



static int audio_spdif_channel_use_syncts_frames(struct audio_spdif_channel *ch, int frames)
{
    struct audio_spdif_hdl *hdl = (struct audio_spdif_hdl *)ch->hd_spdif;
    if (!hdl) {
        return 0;
    }

    struct audio_spdif_sync_node *node;
    u8 have_syncts = 0;
    u8 point_offset = 1;
    if (hdl->data_mode == 0) {
        point_offset = 2;
    }

    list_for_each_entry(node, &hdl->sync_list, entry) {
        if (!node->triggered) {
            continue;
        }
        /*
         * 这里需要将其他通道延时数据大于音频同步计数通道的延迟数据溢出值通知到音频同步
         * 锁存模块，否则会出现硬件锁存不同通道数据的溢出误差
         */
        int diff_samples = audio_cfifo_channel_unread_diff_samples(&((struct audio_spdif_channel *)node->ch)->fifo);
        sound_pcm_update_overflow_frames(node->hdl, diff_samples);
        if ((u32)node->ch != (u32)ch) {
            continue;
        }
        /* if (node->network == AUDIO_NETWORK_LOCAL) { */
        /* sound_pcm_update_frame_num_and_time(node->hdl, frames, audio_jiffies_usec(), audio_spdif_channel_buffered_frames(ch)); */
        /* } else { */
        /* sound_pcm_update_frame_num(node->hdl, frames); */
        /* } */
        sound_pcm_update_frame_num(node->hdl, frames);
        if (node->network == AUDIO_NETWORK_LOCAL && ch->state == AUDIO_CFIFO_START) {
            if (audio_syncts_latch_enable(node->hdl)) {
                spin_lock(&hdl->lock);
                int timeout = (1000000 / hdl->sample_rate) * (clk_get("sys") / 1000000) * 5 / 4;
                u32 swn = JL_SPDIF->SM_ASHN / point_offset;
                while (--timeout > 0 && swn == (JL_SPDIF->SM_ASHN / point_offset));
                u32 time = audio_jiffies_usec();
                swn = JL_SPDIF->SM_ASHN / point_offset;
                spin_unlock(&hdl->lock);
                int buffered_frames = (JL_SPDIF->SM_ABUFLEN / point_offset - swn - 1) - audio_cfifo_channel_unread_diff_samples(&ch->fifo);
                sound_pcm_update_frame_num_and_time(node->hdl, 0, time, buffered_frames);
            }
        }


        have_syncts = 1;
    }

    if (have_syncts) {
        u16 free_points = JL_SPDIF->SM_ASHN / point_offset;
        int timeout = (1000000 / hdl->sample_rate) * (clk_get("sys") / 1000000);
        while (free_points == (JL_SPDIF->SM_ASHN / point_offset) && (--timeout > 0));
    }

    return 0;
}


static void audio_spdif_update_frame_begin(struct audio_spdif_channel *ch, u32 frames)
{
    audio_spdif_syncts_enter_update_frame(ch);
}

static void audio_spdif_update_frame(struct audio_spdif_channel *ch, u32 frames)
{
    audio_spdif_channel_use_syncts_frames(ch, frames);
}

static void audio_spdif_update_frame_end(struct audio_spdif_channel *ch)
{
    audio_spdif_syncts_exit_update_frame(ch);
}

int audio_spdif_fill_slience_frames(void *private_data,  int frames)
{
    struct audio_spdif_channel *ch = (struct audio_spdif_channel *)private_data;
    struct audio_spdif_hdl *hdl = (struct audio_spdif_hdl *)ch->hd_spdif;
    if (!hdl) {
        return 0;
    }

    int wlen = 0;
    u16 point_offset = 1;
    if (hdl->data_mode == 0) {
        point_offset = 2;
    }
    wlen = audio_spdif_channel_fifo_write(ch, (void *)0, frames * hdl->channel << point_offset, 1);

    return (wlen >> point_offset) / hdl->channel;
}


int audio_spdif_syncts_enter_update_frame(struct audio_spdif_channel *ch)
{
    struct audio_spdif_sync_node *node;
    struct audio_spdif_hdl *hdl = (struct audio_spdif_hdl *)ch->hd_spdif;
    if (!hdl) {
        return 0;
    }
    int ret = 0;
    list_for_each_entry(node, &hdl->sync_list, entry) {
        ret = sound_pcm_enter_update_frame(node->hdl);
    }
    return ret;
}

int audio_spdif_syncts_exit_update_frame(struct audio_spdif_channel *ch)
{
    struct audio_spdif_sync_node *node;
    struct audio_spdif_hdl *hdl = (struct audio_spdif_hdl *)ch->hd_spdif;
    if (!hdl) {
        return 0;
    }

    int ret = 0;
    list_for_each_entry(node, &hdl->sync_list, entry) {
        ret = sound_pcm_exit_update_frame(node->hdl);
    }

    return ret;
}

void audio_spdif_force_use_syncts_frames(void *private_data, int frames, u32 timestamp)
{
    struct audio_spdif_sync_node *node;
    struct audio_spdif_channel *ch = (struct audio_spdif_channel *)private_data;
    struct audio_spdif_hdl *hdl = (struct audio_spdif_hdl *)ch->hd_spdif;
    if (!hdl) {
        return ;
    }
    os_mutex_pend(&hdl->mutex, 0);
    list_for_each_entry(node, &hdl->sync_list, entry) {
        if ((u32)node->ch != (u32)ch)  {
            continue;
        }
        if (!node->triggered) {
            if (audio_syncts_support_use_trigger_timestamp(node->hdl)) {
                u8 timestamp_enable = audio_syncts_get_trigger_timestamp(node->hdl, &node->timestamp);
                if (!timestamp_enable) {
                    continue;
                }
            }
            int time_diff = node->timestamp - timestamp;
            if (time_diff > 0) {
                os_mutex_post(&hdl->mutex);
                return;
            }
            if (node->hdl) {
                sound_pcm_update_frame_num(node->hdl, frames);
            }
        }
    }
    os_mutex_post(&hdl->mutex);
}



void audio_spdif_syncts_trigger_with_timestamp(void *private_data, u32 timestamp)
{
    struct audio_spdif_sync_node *node;
    struct audio_spdif_channel *ch = (struct audio_spdif_channel *)private_data;
    struct audio_spdif_hdl *hdl = (struct audio_spdif_hdl *)ch->hd_spdif;
    if (!hdl) {
        return ;
    }

    int time_diff = 0;
    os_mutex_pend(&hdl->mutex, 0);
    list_for_each_entry(node, &hdl->sync_list, entry) {
        if (node->triggered || ((u32)node->ch != (u32)ch)) {
            continue;
        }
        if (audio_syncts_support_use_trigger_timestamp(node->hdl)) {
            u8 timestamp_enable = audio_syncts_get_trigger_timestamp(node->hdl, &node->timestamp);
            if (!timestamp_enable) {
                continue;
            }
        }

        //int one_sample = PCM_SAMPLE_ONE_SECOND / dac->sample_rate;
        time_diff = timestamp - node->timestamp;
        /*printf("----- %d, %u, %u-----\n", time_diff, node->timestamp, timestamp);*/
        if (time_diff >= 0) {
            /*
            if (node->hdl) {
                sound_pcm_update_frame_num(node->hdl, TIME_TO_PCM_SAMPLES(time_diff, dac->sample_rate));
            }
            */
            sound_pcm_syncts_latch_trigger(node->hdl);
            /*printf("------------>>>> sound pcm syncts latch trigger : %d, %u, %u---\n", time_diff, node->timestamp, timestamp);*/
            node->triggered = 1;
        }
    }
    os_mutex_post(&hdl->mutex);
}

__attribute__((weak))
int dac_adapter_link_to_syncts_check(void *syncts)
{
    return 0;
}

void audio_spdif_add_syncts_with_timestamp(struct audio_spdif_channel *ch, void *syncts, u32 timestamp)
{
    struct audio_spdif_hdl *hdl = ch->hd_spdif;
    if (!ch || !hdl) {
        return;
    }
    struct audio_spdif_sync_node *node = NULL;
    os_mutex_pend(&hdl->mutex, 0);
    if (iis_adapter_link_to_syncts_check(syncts)) {
        printf("syncts has beed link to iis");
        os_mutex_post(&hdl->mutex);
        return;
    }
    if (dac_adapter_link_to_syncts_check(syncts)) {
        printf("syncts has beed link to dac");
        os_mutex_post(&hdl->mutex);
        return;
    }

    list_for_each_entry(node, &hdl->sync_list, entry) {
        if ((u32)node->hdl == (u32)syncts) {
            os_mutex_post(&hdl->mutex);
            return;
        }
    }
    os_mutex_post(&hdl->mutex);

    if (sound_pcm_get_syncts_network(syncts) != AUDIO_NETWORK_LOCAL) { //本地时钟不需要关联
        log_e("spdif not support a2dp 2t1 network\n");
        return;
    }
    log_d(">>>>>>>>>>>>add spdif syncts %x\n", syncts);
    node = (struct audio_spdif_sync_node *)zalloc(sizeof(struct audio_spdif_sync_node));
    node->hdl = syncts;
    node->network = sound_pcm_get_syncts_network(syncts);
    node->timestamp = timestamp;
    node->ch = ch;
    /* if (sound_pcm_get_syncts_network(syncts) != AUDIO_NETWORK_LOCAL) { //本地时钟不需要关联 */
    /* sound_pcm_device_register(syncts, PCM_INSIDE_DAC); */
    /* } */
    os_mutex_pend(&hdl->mutex, 0);
    spin_lock(&hdl->lock);
    list_add(&node->entry, &hdl->sync_list);
    spin_unlock(&hdl->lock);
    os_mutex_post(&hdl->mutex);
}
void audio_spdif_remove_syncts_handle(struct audio_spdif_channel *ch, void *syncts)
{
    struct audio_spdif_hdl *hdl = ch->hd_spdif;
    if (!ch || !hdl) {
        return;
    }
    os_mutex_pend(&hdl->mutex, 0);
    struct audio_spdif_sync_node *node;
    list_for_each_entry(node, &hdl->sync_list, entry) {
        if ((u32)node->ch != (u32)ch)  {
            continue;
        }
        if (node->hdl == syncts) {
            goto remove_node;
        }
    }
    os_mutex_post(&hdl->mutex);

    return;
remove_node:

    log_d(">>>>>>>>>>>>del spdif syncts %x\n", syncts);

    spin_lock(&hdl->lock);
    list_del(&node->entry);
    free(node);
    spin_unlock(&hdl->lock);
    os_mutex_post(&hdl->mutex);
}

static int audio_spdif_channel_fifo_write(struct audio_spdif_channel *ch, void *data, int len, u8 is_fixed_data)
{
    if (len == 0) {
        return 0;
    }
    if (config_audio_spdif_mix_enable) {
        struct audio_spdif_hdl *hd_spdif = ch->hd_spdif;
        os_mutex_pend(&hd_spdif->mutex, 0);
        if (hd_spdif->data_mode == 0) {
            ch->fifo.bit_wide = hd_spdif->data_mode ? 0 : 1;
        }
        int w_len = 0;
        int frames = 0;
        /* int unread_samples = (JL_SPDIF->SM_ABUFLEN - JL_SPDIF->SM_ASHN - 1) / 2; */
        int unread_samples = (JL_SPDIF->SM_ABUFLEN - JL_SPDIF->SM_ASHN) >> (ch->fifo.bit_wide);
        /* printf("unread_samples %d  %d\n",unread_samples,hd_spdif->unread_samples); */
        if (hd_spdif->unread_samples < unread_samples) {
            //reset cbuf
            JL_SPDIF->SM_ASPTR = (audio_cfifo_get_write_offset(&hd_spdif->fifo) << ch->fifo.bit_wide);
            unread_samples = 0;
        }
        audio_cfifo_read_update(&hd_spdif->fifo, hd_spdif->unread_samples - unread_samples);

        hd_spdif->unread_samples = unread_samples;

        if (is_fixed_data) {
            w_len = audio_cfifo_channel_write_fixed_data(&ch->fifo, (s16)data, len);
        } else {
            w_len = audio_cfifo_channel_write(&ch->fifo, data, len);
        }
        int fifo_frames = (w_len >> (hd_spdif->data_mode ? 1 : 2)) / hd_spdif->channel;

        int samples = audio_cfifo_get_unread_samples(&hd_spdif->fifo) - hd_spdif->unread_samples;

        ASSERT(samples >= 0, " samples : %d, %d\n", audio_cfifo_get_unread_samples(&hd_spdif->fifo), hd_spdif->unread_samples);

        hd_spdif->unread_samples += samples;

        frames = samples;


        audio_spdif_update_frame_begin(ch, fifo_frames);
        if (frames) {
            JL_SPDIF->SM_ASHN = frames << ch->fifo.bit_wide;
            __asm_csync();
        }
        /* audio_spdif_fifo_start(hd_spdif); */
        audio_spdif_update_frame(ch, fifo_frames);
        audio_spdif_update_frame_end(ch);

        ASSERT(JL_SPDIF->SM_ASPTR == (audio_cfifo_get_write_offset(&hd_spdif->fifo) << ch->fifo.bit_wide), ", %d %d\n", JL_SPDIF->SM_ASPTR, audio_cfifo_get_write_offset(&hd_spdif->fifo));
        os_mutex_post(&hd_spdif->mutex);

        return w_len;
    }
    return 0;
}

int audio_spdif_channel_write(void *private_data, struct audio_spdif_hdl *spdif, void *buf, int len)
{
    struct audio_spdif_channel *ch = private_data;
    if (!ch) {
        return audio_spdif_channel_fifo_write(&spdif->main_ch, buf, len, 0);
    } else {
        return audio_spdif_channel_fifo_write(ch, buf, len, 0);
    }
}

void audio_spdif_channel_start(void *private_data)
{
    if (config_audio_spdif_mix_enable) {
        struct audio_spdif_hdl *hd_spdif = spdif_hdl;
        if (!hd_spdif) {
            log_e("DAC channel start error! dac_hdl is NULL\n");
            return;
        }
        os_mutex_pend(&hd_spdif->mutex, 0);
        if (!audio_spdif_is_working(hd_spdif)) {
            spdif_master_start(hd_spdif);
        }
        u16 point_offset = 1;
        if (hd_spdif->data_mode == 0) {
            point_offset = 2;
        }

        struct audio_spdif_channel *ch = private_data;
        int delay_samples = 0, start_len = 0;
        if (hd_spdif->fifo_state != AUDIO_CFIFO_INITED) {
            hd_spdif->fifo.bit_wide = hd_spdif->data_mode ? 0 : 1;
            /* printf("audio fifo init"); */
            audio_cfifo_init(&hd_spdif->fifo, hd_spdif->output_buf, hd_spdif->output_buf_len, hd_spdif->sample_rate, hd_spdif->channel);
            hd_spdif->fifo_state = AUDIO_CFIFO_INITED;
        }
        if (hd_spdif->state != SPDIF_STATE_START) {

            audio_spdif_fifo_start(hd_spdif);
            if (!ch) {
                JL_SPDIF->SM_ASPTR = 0;
                audio_cfifo_channel_add(&hd_spdif->fifo, &hd_spdif->main_ch.fifo, hd_spdif->prepare_points, 0);
                hd_spdif->prepare_points = 0;
                hd_spdif->main_ch.state = AUDIO_CFIFO_START;
            } else {
                JL_SPDIF->SM_ASPTR = 0;
                audio_cfifo_channel_add(&hd_spdif->fifo, &ch->fifo, ch->attr.delay_time, ch->attr.write_mode);
                hd_spdif->prepare_points = 0;
                ch->state = AUDIO_CFIFO_START;
            }
        } else {
            if (!ch) {
                if (hd_spdif->main_ch.state != AUDIO_CFIFO_START) {
                    audio_cfifo_channel_add(&hd_spdif->fifo, &hd_spdif->main_ch.fifo, hd_spdif->start_points, 0);
                    hd_spdif->main_ch.state = AUDIO_CFIFO_START;
                }
            } else {
                if (ch->state != AUDIO_CFIFO_START) {
                    audio_cfifo_channel_add(&hd_spdif->fifo, &ch->fifo, ch->attr.delay_time, ch->attr.write_mode);
                    ch->state = AUDIO_CFIFO_START;
                }
            }
        }
        os_mutex_post(&hd_spdif->mutex);
    }
}

void audio_spdif_channel_close(void *private_data)
{
    if (config_audio_spdif_mix_enable) {
        struct audio_spdif_hdl *hd_spdif = spdif_hdl;
        if (!hd_spdif) {
            log_e("SPDIF channel close error! spdif_hdl is NULL\n");
            return;
        }
        struct audio_spdif_channel *ch = private_data;
        if (ch == NULL) {
            ch = &hd_spdif->main_ch;
        }

        os_mutex_pend(&hd_spdif->mutex, 0);
        if (ch->state != AUDIO_CFIFO_IDLE && ch->state != AUDIO_CFIFO_CLOSE) {
            if (ch->state == AUDIO_CFIFO_START) {
                audio_cfifo_channel_del(&ch->fifo);
                ch->state = AUDIO_CFIFO_CLOSE;
            }
        }
        if (hd_spdif->fifo_state != AUDIO_CFIFO_IDLE) {
            if (audio_cfifo_channel_num(&hd_spdif->fifo) == 0) {
                spdif_master_stop(hd_spdif);
            }
        }
        os_mutex_post(&hd_spdif->mutex);
    }
}


/*
 * Audio SPDIF通道的属性设置（延时/写入模式/...）
 */
int audio_spdif_channel_set_attr(struct audio_spdif_channel *ch, struct audio_spdif_channel_attr *attr)
{
    if (config_audio_spdif_mix_enable) {
        if (!attr) {
            return -EINVAL;
        }
        memcpy(&ch->attr, attr, sizeof(ch->attr));
        //TODO : 可继续增加其他属性配置
    }
    return 0;
}

/*
 * Audio SPDIF申请加入一个新的SPDIF通道
 */
int audio_spdif_new_channel(struct audio_spdif_hdl *hd_spdif, struct audio_spdif_channel *ch)
{
    if (config_audio_spdif_mix_enable) {
        if (!hd_spdif || !ch) {
            return -EINVAL;
        }

        spin_lock(&hd_spdif->lock);
        memset(ch, 0x0, sizeof(struct audio_spdif_channel));

        ch->hd_spdif = hd_spdif;
        ch->state = AUDIO_CFIFO_IDLE;

        spin_unlock(&hd_spdif->lock);
    }

    return 0;
}

#define AUDIO_SPDIF_MAX_SAMPLE_RATE      AUDIO_DAC_MAX_SAMPLE_RATE  //     96000
#define TCFG_AUDIO_SPDIF_BUFFER_TIME_MS   TCFG_AUDIO_DAC_BUFFER_TIME_MS   //50 //缓冲长度（ms）
int spdif_master_out_init(struct audio_spdif_hdl *spdif_handler, SPDIF_MASTER_PARM *spdif_master_cfg)
{
    puts("spdif master early init \n");
    memset(spdif_handler, 0x0, sizeof(struct audio_spdif_hdl));
    spdif_handler->tx_io = spdif_master_cfg->tx_io;
    /* spdif_handler->data_mode = 	SPDIF_M_DAT_24BIT; */
    spdif_handler->data_mode = spdif_master_cfg->bit_width ? SPDIF_M_DAT_24BIT : SPDIF_M_DAT_16BIT;
    spdif_handler->output_buf_len = TCFG_AUDIO_SPDIF_BUFFER_TIME_MS * ((AUDIO_SPDIF_MAX_SAMPLE_RATE + 999) / 1000) * 2 * (
                                        spdif_handler->data_mode ? 4 : 2);
    printf(">>>> buf time:%dms, buf_len : %d\n", TCFG_AUDIO_SPDIF_BUFFER_TIME_MS, spdif_handler->output_buf_len);
    spdif_handler->output_buf = dma_malloc(spdif_handler->output_buf_len);
    memset(spdif_handler->output_buf, 0x00, spdif_handler->output_buf_len);

    spdif_handler->output_inf_buf_len = 192 * 2 * 4;
    spdif_handler->output_inf_buf = dma_malloc(spdif_handler->output_inf_buf_len);
    memset(spdif_handler->output_inf_buf, 0x0, spdif_handler->output_inf_buf_len);
    ASSERT(spdif_handler->output_buf != NULL);
    spdif_handler->state = SPDIF_STATE_INIT;
    spdif_handler->fifo_state = AUDIO_CFIFO_IDLE;
    spdif_handler->channel = 2;
    spdif_handler->sample_rate = 44100;
    spdif_handler->start_ms = 10;
    spdif_handler->delay_ms = 20;

    spdif_handler->main_ch.hd_spdif = spdif_handler;
    spdif_handler->main_ch.state = AUDIO_CFIFO_IDLE;
    spdif_handler->main_ch.attr.delay_time = 20;
    spdif_handler->main_ch.attr.protect_time = 8;
    spdif_handler->main_ch.attr.write_mode = 0;//WRITE_MODE_BLOCK;
    os_mutex_create(&spdif_handler->mutex);

    INIT_LIST_HEAD(&(spdif_handler->sync_list));
    spin_lock_init(&spdif_handler->lock);

    spdif_hdl = spdif_handler;
#if 0
    // for test
    audio_spdif_new_channel(spdif_handler, &spdif_ch);

    struct audio_spdif_channel_attr attr;
    attr.delay_time = 10;
    attr.protect_time = 8;
    attr.write_mode = 0;//WRITE_MODE_BLOCK;
    audio_spdif_channel_set_attr(&spdif_ch, &attr);
    /* spdif_master_init(&test_spdif_master); */
    /* spdif_master_init(spdif_handler,&test_spdif_master); */
    /* int err = task_create(spdif_app_task, NULL, SPDIF_TEST_TASK_NAME); */
#endif
    return 0;
}



#if 0
static short const tsin_441k[441] = {
    0x0000, 0x122d, 0x23fb, 0x350f, 0x450f, 0x53aa, 0x6092, 0x6b85, 0x744b, 0x7ab5, 0x7ea2, 0x7fff, 0x7ec3, 0x7af6, 0x74ab, 0x6c03,
    0x612a, 0x545a, 0x45d4, 0x35e3, 0x24db, 0x1314, 0x00e9, 0xeeba, 0xdce5, 0xcbc6, 0xbbb6, 0xad08, 0xa008, 0x94fa, 0x8c18, 0x858f,
    0x8181, 0x8003, 0x811d, 0x84ca, 0x8af5, 0x9380, 0x9e3e, 0xaaf7, 0xb969, 0xc94a, 0xda46, 0xec06, 0xfe2d, 0x105e, 0x223a, 0x3365,
    0x4385, 0x5246, 0x5f5d, 0x6a85, 0x7384, 0x7a2d, 0x7e5b, 0x7ffa, 0x7f01, 0x7b75, 0x7568, 0x6cfb, 0x6258, 0x55b7, 0x4759, 0x3789,
    0x2699, 0x14e1, 0x02bc, 0xf089, 0xdea7, 0xcd71, 0xbd42, 0xae6d, 0xa13f, 0x95fd, 0x8ce1, 0x861a, 0x81cb, 0x800b, 0x80e3, 0x844e,
    0x8a3c, 0x928c, 0x9d13, 0xa99c, 0xb7e6, 0xc7a5, 0xd889, 0xea39, 0xfc5a, 0x0e8f, 0x2077, 0x31b8, 0x41f6, 0x50de, 0x5e23, 0x697f,
    0x72b8, 0x799e, 0x7e0d, 0x7fee, 0x7f37, 0x7bed, 0x761f, 0x6ded, 0x6380, 0x570f, 0x48db, 0x392c, 0x2855, 0x16ad, 0x048f, 0xf259,
    0xe06b, 0xcf20, 0xbed2, 0xafd7, 0xa27c, 0x9705, 0x8db0, 0x86ab, 0x821c, 0x801a, 0x80b0, 0x83da, 0x8988, 0x919c, 0x9bee, 0xa846,
    0xb666, 0xc603, 0xd6ce, 0xe86e, 0xfa88, 0x0cbf, 0x1eb3, 0x3008, 0x4064, 0x4f73, 0x5ce4, 0x6874, 0x71e6, 0x790a, 0x7db9, 0x7fdc,
    0x7f68, 0x7c5e, 0x76d0, 0x6ed9, 0x64a3, 0x5863, 0x4a59, 0x3acc, 0x2a0f, 0x1878, 0x0661, 0xf42a, 0xe230, 0xd0d0, 0xc066, 0xb145,
    0xa3bd, 0x9813, 0x8e85, 0x8743, 0x8274, 0x8030, 0x8083, 0x836b, 0x88da, 0x90b3, 0x9acd, 0xa6f5, 0xb4ea, 0xc465, 0xd515, 0xe6a3,
    0xf8b6, 0x0aee, 0x1ced, 0x2e56, 0x3ecf, 0x4e02, 0x5ba1, 0x6764, 0x710e, 0x786f, 0x7d5e, 0x7fc3, 0x7f91, 0x7cc9, 0x777a, 0x6fc0,
    0x65c1, 0x59b3, 0x4bd3, 0x3c6a, 0x2bc7, 0x1a41, 0x0833, 0xf5fb, 0xe3f6, 0xd283, 0xc1fc, 0xb2b7, 0xa503, 0x9926, 0x8f60, 0x87e1,
    0x82d2, 0x804c, 0x805d, 0x8303, 0x8833, 0x8fcf, 0x99b2, 0xa5a8, 0xb372, 0xc2c9, 0xd35e, 0xe4da, 0xf6e4, 0x091c, 0x1b26, 0x2ca2,
    0x3d37, 0x4c8e, 0x5a58, 0x664e, 0x7031, 0x77cd, 0x7cfd, 0x7fa3, 0x7fb4, 0x7d2e, 0x781f, 0x70a0, 0x66da, 0x5afd, 0x4d49, 0x3e04,
    0x2d7d, 0x1c0a, 0x0a05, 0xf7cd, 0xe5bf, 0xd439, 0xc396, 0xb42d, 0xa64d, 0x9a3f, 0x9040, 0x8886, 0x8337, 0x806f, 0x803d, 0x82a2,
    0x8791, 0x8ef2, 0x989c, 0xa45f, 0xb1fe, 0xc131, 0xd1aa, 0xe313, 0xf512, 0x074a, 0x195d, 0x2aeb, 0x3b9b, 0x4b16, 0x590b, 0x6533,
    0x6f4d, 0x7726, 0x7c95, 0x7f7d, 0x7fd0, 0x7d8c, 0x78bd, 0x717b, 0x67ed, 0x5c43, 0x4ebb, 0x3f9a, 0x2f30, 0x1dd0, 0x0bd6, 0xf99f,
    0xe788, 0xd5f1, 0xc534, 0xb5a7, 0xa79d, 0x9b5d, 0x9127, 0x8930, 0x83a2, 0x8098, 0x8024, 0x8247, 0x86f6, 0x8e1a, 0x978c, 0xa31c,
    0xb08d, 0xbf9c, 0xcff8, 0xe14d, 0xf341, 0x0578, 0x1792, 0x2932, 0x39fd, 0x499a, 0x57ba, 0x6412, 0x6e64, 0x7678, 0x7c26, 0x7f50,
    0x7fe6, 0x7de4, 0x7955, 0x7250, 0x68fb, 0x5d84, 0x5029, 0x412e, 0x30e0, 0x1f95, 0x0da7, 0xfb71, 0xe953, 0xd7ab, 0xc6d4, 0xb725,
    0xa8f1, 0x9c80, 0x9213, 0x89e1, 0x8413, 0x80c9, 0x8012, 0x81f3, 0x8662, 0x8d48, 0x9681, 0xa1dd, 0xaf22, 0xbe0a, 0xce48, 0xdf89,
    0xf171, 0x03a6, 0x15c7, 0x2777, 0x385b, 0x481a, 0x5664, 0x62ed, 0x6d74, 0x75c4, 0x7bb2, 0x7f1d, 0x7ff5, 0x7e35, 0x79e6, 0x731f,
    0x6a03, 0x5ec1, 0x5193, 0x42be, 0x328f, 0x2159, 0x0f77, 0xfd44, 0xeb1f, 0xd967, 0xc877, 0xb8a7, 0xaa49, 0x9da8, 0x9305, 0x8a98,
    0x848b, 0x80ff, 0x8006, 0x81a5, 0x85d3, 0x8c7c, 0x957b, 0xa0a3, 0xadba, 0xbc7b, 0xcc9b, 0xddc6, 0xefa2, 0x01d3, 0x13fa, 0x25ba,
    0x36b6, 0x4697, 0x5509, 0x61c2, 0x6c80, 0x750b, 0x7b36, 0x7ee3, 0x7ffd, 0x7e7f, 0x7a71, 0x73e8, 0x6b06, 0x5ff8, 0x52f8, 0x444a,
    0x343a, 0x231b, 0x1146, 0xff17, 0xecec, 0xdb25, 0xca1d, 0xba2c, 0xaba6, 0x9ed6, 0x93fd, 0x8b55, 0x850a, 0x813d, 0x8001, 0x815e,
    0x854b, 0x8bb5, 0x947b, 0x9f6e, 0xac56, 0xbaf1, 0xcaf1, 0xdc05, 0xedd3
};

static const unsigned char tsin_192k[768] = {
    0x01, 0x00, 0x19, 0x02, 0x33, 0x04, 0x49, 0x06, 0x60, 0x08, 0x73, 0x0a, 0x84, 0x0c, 0x91, 0x0e,
    0x9b, 0x10, 0x9f, 0x12, 0x9f, 0x14, 0x99, 0x16, 0x8d, 0x18, 0x7a, 0x1a, 0x60, 0x1c, 0x3f, 0x1e,
    0x13, 0x20, 0xe0, 0x21, 0xa4, 0x23, 0x5e, 0x25, 0x0d, 0x27, 0xb3, 0x28, 0x4c, 0x2a, 0xda, 0x2b,
    0x5d, 0x2d, 0xd2, 0x2e, 0x3b, 0x30, 0x97, 0x31, 0xe5, 0x32, 0x26, 0x34, 0x57, 0x35, 0x7a, 0x36,
    0x8f, 0x37, 0x93, 0x38, 0x89, 0x39, 0x70, 0x3a, 0x44, 0x3b, 0x0a, 0x3c, 0xbf, 0x3c, 0x64, 0x3d,
    0xf7, 0x3d, 0x7b, 0x3e, 0xea, 0x3e, 0x4b, 0x3f, 0x9b, 0x3f, 0xd7, 0x3f, 0x04, 0x40, 0x1e, 0x40,
    0x27, 0x40, 0x1f, 0x40, 0x04, 0x40, 0xd8, 0x3f, 0x9a, 0x3f, 0x4c, 0x3f, 0xeb, 0x3e, 0x79, 0x3e,
    0xf8, 0x3d, 0x63, 0x3d, 0xc0, 0x3c, 0x0a, 0x3c, 0x45, 0x3b, 0x6f, 0x3a, 0x89, 0x39, 0x94, 0x38,
    0x8e, 0x37, 0x7a, 0x36, 0x58, 0x35, 0x24, 0x34, 0xe5, 0x32, 0x97, 0x31, 0x3b, 0x30, 0xd2, 0x2e,
    0x5c, 0x2d, 0xdb, 0x2b, 0x4d, 0x2a, 0xb2, 0x28, 0x0e, 0x27, 0x5e, 0x25, 0xa4, 0x23, 0xe0, 0x21,
    0x14, 0x20, 0x3e, 0x1e, 0x5f, 0x1c, 0x7a, 0x1a, 0x8d, 0x18, 0x98, 0x16, 0x9e, 0x14, 0xa0, 0x12,
    0x9b, 0x10, 0x92, 0x0e, 0x84, 0x0c, 0x73, 0x0a, 0x60, 0x08, 0x49, 0x06, 0x33, 0x04, 0x19, 0x02,
    0x00, 0x00, 0xe6, 0xfd, 0xce, 0xfb, 0xb6, 0xf9, 0xa1, 0xf7, 0x8d, 0xf5, 0x7c, 0xf3, 0x6e, 0xf1,
    0x66, 0xef, 0x60, 0xed, 0x61, 0xeb, 0x67, 0xe9, 0x73, 0xe7, 0x86, 0xe5, 0xa0, 0xe3, 0xc2, 0xe1,
    0xed, 0xdf, 0x1f, 0xde, 0x5c, 0xdc, 0xa2, 0xda, 0xf2, 0xd8, 0x4d, 0xd7, 0xb3, 0xd5, 0x25, 0xd4,
    0xa3, 0xd2, 0x2e, 0xd1, 0xc4, 0xcf, 0x68, 0xce, 0x1c, 0xcd, 0xdb, 0xcb, 0xa9, 0xca, 0x85, 0xc9,
    0x72, 0xc8, 0x6c, 0xc7, 0x77, 0xc6, 0x90, 0xc5, 0xbb, 0xc4, 0xf6, 0xc3, 0x40, 0xc3, 0x9c, 0xc2,
    0x09, 0xc2, 0x86, 0xc1, 0x14, 0xc1, 0xb4, 0xc0, 0x65, 0xc0, 0x28, 0xc0, 0xfc, 0xbf, 0xe2, 0xbf,
    0xd9, 0xbf, 0xe2, 0xbf, 0xfc, 0xbf, 0x28, 0xc0, 0x66, 0xc0, 0xb4, 0xc0, 0x15, 0xc1, 0x86, 0xc1,
    0x09, 0xc2, 0x9c, 0xc2, 0x40, 0xc3, 0xf6, 0xc3, 0xbb, 0xc4, 0x91, 0xc5, 0x76, 0xc6, 0x6d, 0xc7,
    0x71, 0xc8, 0x86, 0xc9, 0xa9, 0xca, 0xdb, 0xcb, 0x1b, 0xcd, 0x69, 0xce, 0xc4, 0xcf, 0x2d, 0xd1,
    0xa3, 0xd2, 0x25, 0xd4, 0xb4, 0xd5, 0x4d, 0xd7, 0xf2, 0xd8, 0xa2, 0xda, 0x5c, 0xdc, 0x20, 0xde,
    0xec, 0xdf, 0xc1, 0xe1, 0xa1, 0xe3, 0x86, 0xe5, 0x73, 0xe7, 0x68, 0xe9, 0x61, 0xeb, 0x61, 0xed,
    0x65, 0xef, 0x6e, 0xf1, 0x7c, 0xf3, 0x8d, 0xf5, 0xa0, 0xf7, 0xb7, 0xf9, 0xce, 0xfb, 0xe7, 0xfd,
    0xff, 0xff, 0x19, 0x02, 0x32, 0x04, 0x4a, 0x06, 0x5f, 0x08, 0x73, 0x0a, 0x83, 0x0c, 0x91, 0x0e,
    0x9b, 0x10, 0x9f, 0x12, 0x9f, 0x14, 0x99, 0x16, 0x8c, 0x18, 0x79, 0x1a, 0x5f, 0x1c, 0x3e, 0x1e,
    0x13, 0x20, 0xe1, 0x21, 0xa4, 0x23, 0x5e, 0x25, 0x0e, 0x27, 0xb2, 0x28, 0x4b, 0x2a, 0xdb, 0x2b,
    0x5d, 0x2d, 0xd2, 0x2e, 0x3b, 0x30, 0x97, 0x31, 0xe6, 0x32, 0x25, 0x34, 0x58, 0x35, 0x7a, 0x36,
    0x8e, 0x37, 0x94, 0x38, 0x8a, 0x39, 0x6f, 0x3a, 0x45, 0x3b, 0x0b, 0x3c, 0xbf, 0x3c, 0x64, 0x3d,
    0xf7, 0x3d, 0x79, 0x3e, 0xec, 0x3e, 0x4c, 0x3f, 0x9b, 0x3f, 0xd8, 0x3f, 0x04, 0x40, 0x1e, 0x40,
    0x27, 0x40, 0x1f, 0x40, 0x03, 0x40, 0xd8, 0x3f, 0x9a, 0x3f, 0x4b, 0x3f, 0xeb, 0x3e, 0x7a, 0x3e,
    0xf7, 0x3d, 0x64, 0x3d, 0xbf, 0x3c, 0x0a, 0x3c, 0x45, 0x3b, 0x6f, 0x3a, 0x89, 0x39, 0x94, 0x38,
    0x8f, 0x37, 0x7b, 0x36, 0x57, 0x35, 0x25, 0x34, 0xe5, 0x32, 0x96, 0x31, 0x3b, 0x30, 0xd3, 0x2e,
    0x5d, 0x2d, 0xdb, 0x2b, 0x4d, 0x2a, 0xb2, 0x28, 0x0e, 0x27, 0x5d, 0x25, 0xa5, 0x23, 0xe0, 0x21,
    0x13, 0x20, 0x3e, 0x1e, 0x60, 0x1c, 0x79, 0x1a, 0x8c, 0x18, 0x99, 0x16, 0x9f, 0x14, 0xa0, 0x12,
    0x9b, 0x10, 0x91, 0x0e, 0x84, 0x0c, 0x73, 0x0a, 0x60, 0x08, 0x4a, 0x06, 0x32, 0x04, 0x19, 0x02,
    0x00, 0x00, 0xe6, 0xfd, 0xce, 0xfb, 0xb7, 0xf9, 0xa0, 0xf7, 0x8d, 0xf5, 0x7c, 0xf3, 0x6f, 0xf1,
    0x65, 0xef, 0x60, 0xed, 0x62, 0xeb, 0x67, 0xe9, 0x73, 0xe7, 0x86, 0xe5, 0xa0, 0xe3, 0xc3, 0xe1,
    0xec, 0xdf, 0x21, 0xde, 0x5b, 0xdc, 0xa1, 0xda, 0xf2, 0xd8, 0x4d, 0xd7, 0xb4, 0xd5, 0x25, 0xd4,
    0xa3, 0xd2, 0x2d, 0xd1, 0xc5, 0xcf, 0x69, 0xce, 0x1b, 0xcd, 0xdb, 0xcb, 0xa9, 0xca, 0x86, 0xc9,
    0x71, 0xc8, 0x6c, 0xc7, 0x77, 0xc6, 0x91, 0xc5, 0xbc, 0xc4, 0xf6, 0xc3, 0x40, 0xc3, 0x9c, 0xc2,
    0x08, 0xc2, 0x86, 0xc1, 0x14, 0xc1, 0xb4, 0xc0, 0x65, 0xc0, 0x29, 0xc0, 0xfc, 0xbf, 0xe2, 0xbf,
    0xd9, 0xbf, 0xe2, 0xbf, 0xfc, 0xbf, 0x29, 0xc0, 0x66, 0xc0, 0xb4, 0xc0, 0x14, 0xc1, 0x86, 0xc1,
    0x09, 0xc2, 0x9d, 0xc2, 0x41, 0xc3, 0xf6, 0xc3, 0xbb, 0xc4, 0x91, 0xc5, 0x77, 0xc6, 0x6d, 0xc7,
    0x72, 0xc8, 0x86, 0xc9, 0xa8, 0xca, 0xda, 0xcb, 0x1b, 0xcd, 0x69, 0xce, 0xc5, 0xcf, 0x2d, 0xd1,
    0xa3, 0xd2, 0x25, 0xd4, 0xb3, 0xd5, 0x4e, 0xd7, 0xf3, 0xd8, 0xa2, 0xda, 0x5b, 0xdc, 0x1f, 0xde,
    0xec, 0xdf, 0xc2, 0xe1, 0xa0, 0xe3, 0x87, 0xe5, 0x73, 0xe7, 0x67, 0xe9, 0x62, 0xeb, 0x60, 0xed,
    0x65, 0xef, 0x6f, 0xf1, 0x7c, 0xf3, 0x8d, 0xf5, 0xa0, 0xf7, 0xb6, 0xf9, 0xcd, 0xfb, 0xe6, 0xfd,
};

static u16 tx_s_cnt = 0;
static int get_sine_data(u16 *s_cnt, s16 *data, u16 points, u8 ch)
{
    while (points--) {
        if (*s_cnt >= 441) {
            *s_cnt = 0;
        }
        /* *data++ = tsin_16k[*s_cnt]; */
        *data++ = tsin_441k[*s_cnt];
        /* *data++ = points; */
        if (ch == 2) {
            *data++ = tsin_441k[*s_cnt];
            /* *data++ = tsin_16k[*s_cnt]; */
            /* *data++ = points; */
        }
        (*s_cnt)++;
    }
    return 0;
}
static int get_sine_data_24(u16 *s_cnt, s32 *data, u16 points, u8 ch)
{
    while (points--) {
        if (*s_cnt >= 441) {
            *s_cnt = 0;
        }
        /* *data++ = tsin_16k[*s_cnt]; */
        *data++ = tsin_441k[*s_cnt] << 8;
        /* *data++ = points; */
        if (ch == 2) {
            *data++ = tsin_441k[*s_cnt] << 8;
            /* *data++ = tsin_16k[*s_cnt]; */
            /* *data++ = points; */
        }
        (*s_cnt)++;
    }
    return 0;
}

static int get_sine_192k(u16 *s_cnt, s8 *data, u16 points, u8 ch)
{
    while (points--) {
        if (*s_cnt >= 768) {
            *s_cnt = 0;
        }
        /* *data++ = tsin_16k[*s_cnt]; */
        *data++ = tsin_192k[*s_cnt];
        *data++ = tsin_192k[*s_cnt + 1];
        /* *data++ = points; */
        if (ch == 2) {
            *data++ = tsin_192k[*s_cnt];
            *data++ = tsin_192k[*s_cnt + 1];
            /* *data++ = tsin_16k[*s_cnt]; */
            /* *data++ = points; */
        }
        (*s_cnt)++;
        (*s_cnt)++;
    }
    return 0;
}
#endif

#if 0
#include "os/os_compat.h"
#define SPDIF_TEST_TASK_NAME "spdif_task"


#define FRAME_PONITS  128*2*2*2
struct audio_spdif_channel spdif_ch;
void spdif_app_task(void *priv)
{
    int wlen = 0;
    int dlen = 0;
    int w_points = 0;
    s32 *p_data = NULL;
    s32  *p_wdata = NULL;
    p_data = zalloc(FRAME_PONITS * 2 * 4);
    os_time_dly(500);

    audio_spdif_channel_start((void *)&spdif_ch);
    printf("\n--func22=%s\n", __FUNCTION__);
    while (1) {
        /* os_time_dly(1);	 */
        if (dlen <= 0) {
            putchar('R');
            get_sine_data_24(&tx_s_cnt, p_data, FRAME_PONITS, 2);
            dlen = FRAME_PONITS;
            p_wdata = p_data;
        }
        /* printf("\n1 dlen %d %d \n\n",dlen,w_points); */
        wlen = audio_spdif_channel_write(&spdif_ch, spdif_hdl, p_data, dlen * 2 * 4);
        if (wlen == 0) {
            wdt_clear();
            /* printf("\n spdif task \n"); */
            /* os_time_dly(1);	 */
        } else {
            /* w_points = wlen/4/2; */
            /* dlen -=w_points; 	 */
        }
        /* printf("\n 22222 dlen %d %d %d\n\n",dlen,w_points,wlen); */
    }
}
SPDIF_MASTER_PARM	test_spdif_master = {
    /* .clk_sel = SPDIF_CLOCK_PLL0_D2P0,	 */
    /* .tx_io = IO_PORTA_07, */
    .tx_io = IO_PORTB_02,
    /* .sr = 192000, */
    .sr = 44100,
    /* .data_mode = SPDIF_M_DAT_16BIT, */
    .data_mode = SPDIF_M_DAT_24BIT,
    /* .data_dma_len = 192 *2*2*2, */
    .data_dma_len = 3840 * 2 * 2,
    .inf_dma_len = 192 * 2 * 4,
    /* .data_isr_cb = handle_data, */
    /* .inf_isr_cb = handle_inf, */
};

#endif


#if 0
static u32 j = 0;
static u32 flag = 0;
static void handle_data(void *buf, u32 len)
{
    /* putchar('a'); */
    /* s8 *_buf = NULL; */
    /* _buf = (s8 *)buf; */
    /* s16 *_buf = NULL; */
    /* _buf = (s16 *)buf; */
    s32 *_buf = NULL;
    _buf = (s32 *)buf;

#if 1
    /* get_sine_192k(&tx_s_cnt, _buf, len / 2 / 2, 2); */
    /* get_sine_data(&tx_s_cnt, _buf, len / 2 / 2, 2); */
    get_sine_data_24(&tx_s_cnt, _buf, len / 4 / 2, 2);
#else
    if (flag == 1) {
        flag = 0;
        for (int i = 0; i < len / 2; i++) {
            _buf[i] = 0xFF;
        }
    } else if (flag == 0) {
        flag = 1;
        for (int i = 0; i < len / 2; i++) {
            _buf[i] = 0xaa;
        }
    }

#endif
}

static void handle_inf(void *buf, u32 len)
{
    /* putchar('i'); */
    /* put_buf(buf, 32); */
}


void spdif_master_test_demo()
{
    spdif_master_init(&test_spdif_master);
    spdif_master_start();
    /* while(1){ */
    /* clr_wdt();	 */
    /* } */
}










#endif
