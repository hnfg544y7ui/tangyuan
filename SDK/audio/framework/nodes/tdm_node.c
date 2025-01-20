#include "jlstream.h"
#include "media/audio_base.h"
#include "sync/audio_syncts.h"
#include "circular_buf.h"
#include "audio_splicing.h"
#include "audio_tdm.h"
#include "app_config.h"
#include "gpio.h"
#include "audio_cvp.h"
#include "media/audio_general.h"
#include "reference_time.h"
#include "audio_config_def.h"
#include "effects/effects_adj.h"
#include "media/audio_dev_sync.h"
#include "audio_tdm.h"

#if (TCFG_TDM_TX_ENABLE || TCFG_MULTI_CH_TDM_NODE_ENABLE)
#define TDM_LOG_ENABLE          1
#if TDM_LOG_ENABLE
#define LOG_TAG     "[TDM]"
#define LOG_ERROR_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#include "debug.h"
#else
#define log_info(fmt, ...)
#define log_error(fmt, ...)
#define log_debug(fmt, ...)
#endif
extern const int config_media_tws_en;
#define TDM_DEBUG 0
/* 保存配置文件信息的结构体 */
struct tdm_file_cfg {
    u16 protect_time;          /*TDM延时保护时间*/
} __attribute__((packed));

struct tdm_ch_hdl {
    struct stream_frame *frame;
    u8 timestamp_ok;
};
struct tdm_node_hdl {
    char name[16];
    u8 tdm_start;
    u8 tdm_init;
    u8 bit_width;
    u8 syncts_enabled;
    u8 nch;
    int sample_rate;
    struct stream_node *node;
    struct stream_frame *frame;
    enum stream_scene scene;
    u8 force_write_slience_data;
    u8 match_outsr_count;
    u8 syncts_mount_en;        /*响应前级同步节点的挂载动作，默认1,,特殊流程才会配置0*/
    struct tdm_file_cfg cfg;		//保存文件的配置参数
    void *dev_sync;
    OS_MUTEX mutex;
    spinlock_t lock;
    struct list_head sync_list;
    u8 reference_network;
    u8 time_out_ts_not_align;//超时的时间戳是否需要丢弃
    u8 timestamp_ok;
#if TCFG_MULTI_CH_TDM_NODE_ENABLE
    struct tdm_ch_hdl tdm[4];
    u8 master_ch_idx;
    u16 ch_idx;  //tdm 通道,和iport 的id 对应
    struct stream_frame *push_frame;
#endif
};

struct tdm_write_cb_t {
    u8 scene;
    const char *name;
    struct list_head entry;
    void (*write_callback)(void *buf, int len);
};

struct tdm_gloabl_hdl_t {
    u8 init;
    struct list_head head;
};

struct audio_tdm_sync_node {
    u8 need_triggered;
    u8 triggered;
    u8 network;
    u32 timestamp;
    void *hdl;
    struct list_head entry;
};

int audio_tdm_data_len(void)
{
    return tdm_get_tx_ch_points();
}

int audio_tdm_get_sample_rate(void)
{
    return TDM_SAMPLE_RATE;
}


static struct tdm_gloabl_hdl_t tdm_gloabl_hdl;

/*
 *计算tdm实际输出采样率，用于给从设备做输入采样基准
 * */
void audio_tdm_syncts_trigger_with_timestamp(struct tdm_node_hdl *hdl, u32 timestamp)
{
    struct audio_tdm_sync_node *node;
    int time_diff = 0;
    if (!hdl) {
        return ;
    }
    os_mutex_pend(&hdl->mutex, 0);
    list_for_each_entry(node, &hdl->sync_list, entry) {
        if (node->need_triggered) {
            continue;
        }
        node->need_triggered = 1;
    }
    os_mutex_post(&hdl->mutex);
}

int audio_tdm_fill_slience_frames(int frames)
{
    int wlen = 0;
    u16 point_offset = 1;
    if (TDM_32_BIT_EN) {
        point_offset = 2;
    }
    u8 *temp_data = zalloc(frames  << point_offset);
    if (temp_data) {
        u32 wlen = audio_tdm_write(temp_data, frames << point_offset);
        free(temp_data);
        return (wlen >> point_offset);
    }
    return 0;
}


static int tdm_adpater_detect_timestamp(struct tdm_node_hdl *hdl, struct stream_frame *frame)
{
    int diff = 0;
    u32 current_time = 0;

    if (frame->flags & FRAME_FLAG_SYS_TIMESTAMP_ENABLE) {
        putchar('A');
        // sys timestamp
        return 0;
    }

    if (!(frame->flags & FRAME_FLAG_TIMESTAMP_ENABLE)) {
        if (!hdl->force_write_slience_data) { //无播放同步时，强制填一段静音包
            hdl->force_write_slience_data = 1;
            int slience_time_us = (hdl->cfg.protect_time ? hdl->cfg.protect_time : 8) * 1000;
#if TCFG_MULTI_CH_TDM_NODE_ENABLE
            int slience_frames = (u64)slience_time_us * hdl->sample_rate / 1000000 * TDM_SLOT_NUM;
#else
            int slience_frames = (u64)slience_time_us * hdl->sample_rate / 1000000;
#endif
            audio_tdm_fill_slience_frames(slience_frames);
        }
        return 0;
    }

    if (hdl->syncts_enabled) {
        audio_tdm_syncts_trigger_with_timestamp(hdl, frame->timestamp);
        return 0;
    }

    u8 network = 0;
    /*log_info("----[%d %d %d]----\n", audio_reference_clock_time(), audio_tdm_data_len(&hdl->tdm_ch), hdl->sample_rate);*/
    u32 local_network = 0;
    u32 reference_time = 0;
    local_irq_disable();
    local_network = 1;
#if TCFG_MULTI_CH_TDM_NODE_ENABLE
    current_time = audio_jiffies_usec() * TIMESTAMP_US_DENOMINATOR + PCM_SAMPLE_TO_TIMESTAMP(audio_tdm_data_len() / TDM_SLOT_NUM, hdl->sample_rate);
#else
    current_time = audio_jiffies_usec() * TIMESTAMP_US_DENOMINATOR + PCM_SAMPLE_TO_TIMESTAMP(audio_tdm_data_len(), hdl->sample_rate);
#endif
    local_irq_enable();
#if TDM_DEBUG
    printf("network %d, %d ,%d\n", network, local_network, hdl->reference_network);
#endif
    diff = frame->timestamp - current_time;
    diff /= TIMESTAMP_US_DENOMINATOR;
    if (diff < 0) {
        if (__builtin_abs(diff) <= 1000) {
            goto syncts_start;
        }
        if (hdl->time_out_ts_not_align) { //不做对齐
            if (local_network && !config_media_tws_en) {//本地网络时钟，不需要时间戳对齐
                hdl->syncts_enabled = 1;
                audio_tdm_syncts_trigger_with_timestamp(hdl, frame->timestamp);
#if TDM_DEBUG
                putchar('Q');
#endif
                return 0;
            }
        }

#if TDM_DEBUG
        putchar('T');
        log_info("tdm_node:[%s, %u %u %d]\n", hdl->name, frame->timestamp, current_time, diff);
#endif
        return 2;
    }

    if (diff > 1000000) {
        log_info("tdm node timestamp error : %u, %u, %u, diff : %d\n", frame->timestamp, current_time, reference_time, diff);
    }
#if TCFG_MULTI_CH_TDM_NODE_ENABLE
    int slience_frames = (u64)diff * hdl->sample_rate / 1000000 * TDM_SLOT_NUM;
#else
    int slience_frames = (u64)diff * hdl->sample_rate / 1000000;
#endif
    int filled_frames = audio_tdm_fill_slience_frames(slience_frames);
#if TDM_DEBUG
    log_info("tdm slience_frames %d\n", filled_frames);
#endif
    if (filled_frames < slience_frames) {
        return 1;
    }

syncts_start:
    log_info("tdm_node:<%u %u %d>\n", frame->timestamp, current_time, diff);
    hdl->syncts_enabled = 1;
    audio_tdm_syncts_trigger_with_timestamp(hdl, frame->timestamp);

    return 0;
}

void tdm_node_write_callback_add(const char *name, u8 scene, void (*cb)(void *, int))
{
    if (!tdm_gloabl_hdl.init) {
        tdm_gloabl_hdl.init = 1;
        INIT_LIST_HEAD(&tdm_gloabl_hdl.head);
    }
    struct tdm_write_cb_t *bulk = zalloc(sizeof(struct tdm_write_cb_t));
    bulk->name = name;
    bulk->write_callback = cb;
    bulk->scene = scene;
    list_add(&bulk->entry, &tdm_gloabl_hdl.head);
}

void tdm_node_write_callback_del(const char *name)
{
    struct tdm_write_cb_t *bulk;
    struct tdm_write_cb_t *temp;
    list_for_each_entry_safe(bulk, temp, &tdm_gloabl_hdl.head, entry) {
        if (!strcmp(bulk->name, name)) {
            list_del(&bulk->entry);
            free(bulk);
        }
    }
}

static void tdm_node_write_callback_deal(struct tdm_node_hdl *hdl, struct stream_frame *frame)
{
    struct tdm_write_cb_t *bulk;
    struct tdm_write_cb_t *temp;
    if (tdm_gloabl_hdl.init) {
        list_for_each_entry_safe(bulk, temp, &tdm_gloabl_hdl.head, entry) {
            if (bulk->write_callback) {
                if ((bulk->scene == hdl->scene) || (bulk->scene == 0XFF)) {
                    bulk->write_callback(frame->data, frame->len);
                }
            }
        }
    }
}

void audio_tdm_force_use_syncts_frames(struct tdm_node_hdl *hdl, int frames, u32 timestamp)
{
    struct audio_tdm_sync_node *node;
    os_mutex_pend(&hdl->mutex, 0);
    list_for_each_entry(node, &hdl->sync_list, entry) {
        if (!node->triggered) {
            if (audio_syncts_support_use_trigger_timestamp(node->hdl)) {
                u8 timestamp_enable = audio_syncts_get_trigger_timestamp(node->hdl, &node->timestamp);
                if (!timestamp_enable) {
                    continue;
                }
            }
            int time_diff = node->timestamp - timestamp;
            if (time_diff > 0) {
                continue;
            }
            if (node->hdl) {
                sound_pcm_update_frame_num(node->hdl, frames);
            }
        }
    }
    os_mutex_post(&hdl->mutex);
}


int audio_tdm_syncts_enter_update_frame(struct tdm_node_hdl *hdl)
{
    struct audio_tdm_sync_node *node;
    int ret = 0;
    os_mutex_pend(&hdl->mutex, 0);
    list_for_each_entry(node, &hdl->sync_list, entry) {
        ret = sound_pcm_enter_update_frame(node->hdl);
    }
    os_mutex_post(&hdl->mutex);
    return ret;
}

int audio_tdm_syncts_exit_update_frame(struct tdm_node_hdl *hdl)
{
    struct audio_tdm_sync_node *node;
    int ret = 0;
    os_mutex_pend(&hdl->mutex, 0);
    list_for_each_entry(node, &hdl->sync_list, entry) {
        ret = sound_pcm_exit_update_frame(node->hdl);
    }
    os_mutex_post(&hdl->mutex);

    return ret;
}
static int audio_tdm_channel_use_syncts_frames(struct tdm_node_hdl *hdl, int frames)
{
    struct audio_tdm_sync_node *node;
    u8 have_syncts = 0;

    spin_lock(&hdl->lock);
    list_for_each_entry(node, &hdl->sync_list, entry) {
        if (node->triggered) {
            continue;
        }
        if (!node->need_triggered) {
            continue;
        }

        if (audio_syncts_support_use_trigger_timestamp(node->hdl)) {
            u8 timestamp_enable = audio_syncts_get_trigger_timestamp(node->hdl, &node->timestamp);
            if (!timestamp_enable) {
                continue;
            }
        }
        sound_pcm_syncts_latch_trigger(node->hdl);
        node->triggered = 1;
    }


    list_for_each_entry(node, &hdl->sync_list, entry) {
        if (!node->triggered) {
            continue;
        }
        if (1) {
            spin_lock(&hdl->lock);
            /* u32 time = audio_jiffies_usec(); */
            u32 time = tdm_timestamp();
            spin_unlock(&hdl->lock);
            sound_pcm_update_frame_num_and_time(node->hdl, frames, time, 0);
        }
    }
    spin_unlock(&hdl->lock);

    return 0;
}

void audio_tdm_update_frame(void *hdl, u32 frames)
{
    audio_tdm_channel_use_syncts_frames((struct tdm_node_hdl *)hdl, frames);
}

__NODE_CACHE_CODE(tdm)
static void tdm_write_data(struct stream_iport *iport, struct stream_note *note)
{
    s16 *ptr;
    struct stream_frame *frame;
    struct stream_node *node = iport->node;
    struct tdm_node_hdl *hdl = (struct tdm_node_hdl *)iport->node->private_data;
    u16 point_offset = 1;
    if (iport->prev->fmt.bit_wide) {
        point_offset = 2;
    }
    while (1) {
        frame = jlstream_pull_frame(iport, NULL);
        if (!frame) {
            break;
        }
        int err = tdm_adpater_detect_timestamp(hdl, frame);
        if (err == 1) { //需要继续填充数据至时间戳
            hdl->timestamp_ok = 0;
            jlstream_return_frame(iport, frame);
            break;
        }

        if (err == 2) { //需要直接丢弃改帧数据
            audio_tdm_force_use_syncts_frames(hdl, (frame->len >> point_offset) / 2, frame->timestamp);
            jlstream_free_frame(frame);
            continue;
        }
        hdl->timestamp_ok = 1;
        s16 *data = (s16 *)(frame->data + frame->offset);
        int remain = frame->len - frame->offset;
        int wlen = 0;

        wlen = audio_tdm_write(data, remain);
        frame->offset += wlen;
        if (wlen < remain) {
            note->state |= NODE_STA_OUTPUT_BLOCKED;
            jlstream_return_frame(iport, frame);
            break;
        }
        jlstream_free_frame(frame);
    }
}

static u8 scene_cfg_index_get(u8 scene)
{
    //按可视化工具中的配置项顺序排列
    switch (scene) {
    case STREAM_SCENE_A2DP:
        return 1;
    case STREAM_SCENE_LINEIN:
        return 2;
    case STREAM_SCENE_MUSIC:
        return 3;
    case STREAM_SCENE_FM:
        return 4;
    case STREAM_SCENE_SPDIF:
        return 5;
    default:
        return 0;
    }
}

#if TCFG_MULTI_CH_TDM_NODE_ENABLE
__NODE_CACHE_CODE(tdm)
static int tdm_iport_write(struct stream_iport *iport, struct stream_note *note)
{
    struct tdm_node_hdl *hdl = (struct tdm_node_hdl *)iport->node->private_data;
    struct tdm_ch_hdl *tdm = &hdl->tdm[iport->id];
    int ret = 0;
    if (!tdm->frame) {
        tdm->frame = jlstream_pull_frame(iport, NULL);
        if (!tdm->frame) {
            return 0;
        }
        tdm->frame->offset = 0;
    }
    for (int i = 0; i < 4; i++) {
        if (hdl->ch_idx & BIT(i)) {
            if (!hdl->tdm[i].frame) {
                ret++;
            }
        }
    }
    if (ret) { //未全部拿到数据
        return 0;
    }
    struct stream_frame *frame = hdl->tdm[hdl->master_ch_idx].frame;
    u16 point_offset = 1;
    if (TDM_32_BIT_EN) {
        point_offset = 2;
    }
    if (!frame) {
        return 1;
    }
    os_mutex_pend(&hdl->mutex, 0);
    int err = tdm_adpater_detect_timestamp(hdl, frame);
    if (err == 1) { //需要继续填充数据至时间戳
        for (int i = 0; i < 4; i++) {
            hdl->tdm[i].timestamp_ok = 0;
        }
        os_mutex_post(&hdl->mutex);
        return 1;
    }

    if (err == 2) { //需要直接丢弃改帧数据
        audio_tdm_force_use_syncts_frames(hdl, (frame->len >> point_offset) / 2, frame->timestamp);
        for (int i = 0 ; i < 4; i++) {
            if (hdl->tdm[i].frame) {
                jlstream_free_frame(hdl->tdm[i].frame);
                hdl->tdm[i].frame = NULL;
                hdl->tdm[i].timestamp_ok = 0;
            }
        }
        os_mutex_post(&hdl->mutex);
        return 1;
    }

    if (!hdl->push_frame) {
        int output_len = frame->len * TDM_SLOT_NUM / 2;
        hdl->push_frame = jlstream_get_frame(hdl_node(hdl)->oport, output_len);
        hdl->push_frame->len = output_len;
        if (TDM_32_BIT_EN) {
            s32 *outbuf = (s32 *) hdl->push_frame->data;
            s32 *in0 = (s32 *)(hdl->tdm[0].frame ? (hdl->tdm[0].frame)->data : NULL) ;
            s32 *in1 = (s32 *)(hdl->tdm[1].frame ? (hdl->tdm[1].frame)->data : NULL) ;
            s32 *in2 = (s32 *)(hdl->tdm[2].frame ? (hdl->tdm[2].frame)->data : NULL) ;
            s32 *in3 = (s32 *)(hdl->tdm[3].frame ? (hdl->tdm[3].frame)->data : NULL) ;
            for (int i = 0; i < frame->len / 4 / 2; i++) {
                *outbuf++ = in0 ? in0[2 * i] : 0 ;
                *outbuf++ = in0 ? in0[2 * i + 1] : 0 ;
                *outbuf++ = in1 ? in1[2 * i] : 0 ;
                *outbuf++ = in1 ? in1[2 * i + 1] : 0 ;
                *outbuf++ = in2 ? in2[2 * i] : 0 ;
                *outbuf++ = in2 ? in2[2 * i + 1] : 0 ;
                *outbuf++ = in3 ? in3[2 * i] : 0 ;
                *outbuf++ = in3 ? in3[2 * i + 1] : 0 ;
            }
        } else {
            s16 *outbuf = (s16 *)hdl->push_frame->data;
            s16 *in0 = (s16 *)(hdl->tdm[0].frame ? (hdl->tdm[0].frame)->data : NULL) ;
            s16 *in1 = (s16 *)(hdl->tdm[1].frame ? (hdl->tdm[1].frame)->data : NULL) ;
            s16 *in2 = (s16 *)(hdl->tdm[2].frame ? (hdl->tdm[2].frame)->data : NULL) ;
            s16 *in3 = (s16 *)(hdl->tdm[3].frame ? (hdl->tdm[3].frame)->data : NULL) ;
            for (int i = 0; i < frame->len / 2 / 2; i++) {
                *outbuf++ = in0 ? in0[2 * i] : 0 ;
                *outbuf++ = in0 ? in0[2 * i + 1] : 0 ;
                *outbuf++ = in1 ? in1[2 * i] : 0 ;
                *outbuf++ = in1 ? in1[2 * i + 1] : 0 ;
                *outbuf++ = in2 ? in2[2 * i] : 0 ;
                *outbuf++ = in2 ? in2[2 * i + 1] : 0 ;
                *outbuf++ = in3 ? in3[2 * i] : 0 ;
                *outbuf++ = in3 ? in3[2 * i + 1] : 0 ;
            }
        }
        for (int i = 0; i < 4; i++) {
            if (hdl->tdm[i].frame) {
                jlstream_free_frame(hdl->tdm[i].frame);
                hdl->tdm[i].frame = NULL;
            }
        }
    }
    s16 *ddata1 = (s16 *) hdl->push_frame->data;
    int wlen = audio_tdm_write(hdl->push_frame->data, hdl->push_frame->len);
    os_mutex_post(&hdl->mutex);
    if (!wlen) { //写不进去了
        note->state |= NODE_STA_OUTPUT_BLOCKED;
        return 0;
    }
    jlstream_free_frame(hdl->push_frame);
    hdl->push_frame = NULL;
    note->state &= ~NODE_STA_OUTPUT_BLOCKED;
    return 1;
}
#endif

__NODE_CACHE_CODE(tdm)
static void tdm_handle_frame(struct stream_iport *iport, struct stream_note *note)
{
    struct tdm_node_hdl *hdl = (struct tdm_node_hdl *)iport->node->private_data;

    struct stream_frame *frame;
    struct stream_node *node = iport->node;
    if (!hdl->tdm_start) {
        return 	;
    }

#if TCFG_MULTI_CH_TDM_NODE_ENABLE
    int ret = 0;
    /*
     * 遍历iport
     */
    struct stream_iport *_iport = iport;
    do {
        ret = 0;
        do {
            ret += tdm_iport_write(_iport, note);
            _iport = _iport->sibling;
            if (!_iport) {
                _iport = hdl_node(hdl)->iport;
            }
        } while (_iport != iport);
    } while (ret);
#else
    tdm_write_data(iport, note);
#endif
}

// flag   0:time  1:points
static int tdm_ioc_get_delay(struct tdm_node_hdl *hdl)
{
    int len = audio_tdm_data_len();
    if (len == 0) {
        return 0;
    }
    int rate = audio_tdm_get_sample_rate();
    ASSERT(rate != 0);

    return (len * 10000) / rate;
}

static int tdm_ioc_negotiate(struct stream_iport *iport)
{
    int ret = NEGO_STA_ACCPTED;
    struct stream_fmt *in_fmt = &iport->prev->fmt;
    struct tdm_node_hdl *hdl = (struct tdm_node_hdl *)iport->node->private_data;
    struct audio_general_params *params = audio_general_get_param();
    u8 bit_width = audio_general_out_dev_bit_width();
    if (bit_width) {
        if (in_fmt->bit_wide != DATA_BIT_WIDE_24BIT) {
            in_fmt->bit_wide = DATA_BIT_WIDE_24BIT;
            ret = NEGO_STA_CONTINUE;
        }
    } else {
        if (in_fmt->bit_wide != DATA_BIT_WIDE_16BIT) {
            in_fmt->bit_wide = DATA_BIT_WIDE_16BIT;
            ret = NEGO_STA_CONTINUE;
        }
    }
#if TCFG_MULTI_CH_TDM_NODE_ENABLE
    if (in_fmt->channel_mode != AUDIO_CH_LR) {
        in_fmt->channel_mode = AUDIO_CH_LR;
        ret = NEGO_STA_CONTINUE;
    }
#else
    if (in_fmt->channel_mode != AUDIO_CH_MIX) { //统一协商成单通道,写入tdm后再做声道变换
        in_fmt->channel_mode = AUDIO_CH_MIX;
        ret = NEGO_STA_CONTINUE;
    }
#endif

    u32 sample_rate = 0;

    if (!audio_tdm_get_hdl()) {//节点被打断suspend后，tdm硬件被关闭的情况下，直接将tdm_start状态清空，防止sr是0协商不过
        hdl->tdm_start = 0;
    }
    hdl->sample_rate = sample_rate = audio_tdm_get_sample_rate();
    if (in_fmt->sample_rate != sample_rate) {
        in_fmt->sample_rate = sample_rate;
        ret = NEGO_STA_CONTINUE;
    }

    return ret;
}



static void tdm_adapter_open_iport(struct stream_iport *iport)
{
    iport->handle_frame = tdm_handle_frame;
}

static void tdm_ioc_start(struct tdm_node_hdl *hdl, struct stream_iport *iport)
{
#if TCFG_MULTI_CH_TDM_NODE_ENABLE
    u8 ch_idx = iport->id;
    hdl->ch_idx |= BIT(ch_idx);
#endif

    /* open tdm tx */
    if (hdl->tdm_init == 0) {
        hdl->tdm_init = 1;
        /* 申请 tdm需要的资源，初始化tdm配置 */
        hdl->bit_width = hdl->node->iport->prev->fmt.bit_wide;
        if (hdl->tdm_start == 0) {
            hdl->tdm_start = 1;
            u8 cfg_index = scene_cfg_index_get(hdl->scene);
            int ret = jlstream_read_node_data_by_cfg_index(hdl->node->uuid, hdl->node->subid, cfg_index, (void *)&hdl->cfg, hdl->name);
            if (!ret) {
                ret = jlstream_read_node_data_by_cfg_index(hdl->node->uuid, hdl->node->subid, 0, (void *)&hdl->cfg, hdl->name);
            }
            if (!ret) {
                hdl->cfg.protect_time = 8;
                log_error("tdm node param read err, set default\n");
            }
#if TCFG_MULTI_CH_TDM_NODE_ENABLE
            hdl->master_ch_idx = ch_idx;
#endif
        }

        hdl->syncts_mount_en = 1;
        audio_tdm_tx_open(hdl);
    }
}

static void tdm_ioc_stop(struct tdm_node_hdl *hdl)
{
    if (hdl->frame) {
        jlstream_free_frame(hdl->frame);
        hdl->frame = NULL;
    }

    if (hdl->tdm_start) {
        audio_tdm_close();
        log_debug(">>>>>>>>>>>>>>>>>>>>>>>>tx uninit <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    }
    hdl->syncts_enabled = 0;
    hdl->force_write_slience_data = 0;
    hdl->tdm_start = 0;
    hdl->tdm_init = 0;
}

int audio_tdm_add_syncts_with_timestamp(struct tdm_node_hdl *hdl, void *syncts, u32 timestamp)
{

    struct audio_tdm_sync_node *node = NULL;
    os_mutex_pend(&hdl->mutex, 0);

    list_for_each_entry(node, &hdl->sync_list, entry) {
        if ((u32)node->hdl == (u32)syncts) {
            os_mutex_post(&hdl->mutex);
            return 0;
        }
    }
    log_debug(">>>>>>>>>>>>add hdl syncts %x\n", syncts);
    node = (struct audio_tdm_sync_node *)zalloc(sizeof(struct audio_tdm_sync_node));
    node->hdl = syncts;
    node->network = sound_pcm_get_syncts_network(syncts);
    node->timestamp = timestamp;
    if (sound_pcm_get_syncts_network(syncts) != AUDIO_NETWORK_LOCAL) { //本地时钟不需要关联
        sound_pcm_device_register(syncts, PCM_OUTSIDE_DAC);
    }
    spin_lock(&hdl->lock);
    list_add(&node->entry, &hdl->sync_list);
    spin_unlock(&hdl->lock);
    os_mutex_post(&hdl->mutex);
    return 1;
}

void audio_tdm_remove_syncts_handle(struct tdm_node_hdl *hdl, void *syncts)
{
    struct audio_tdm_sync_node *node;

    os_mutex_pend(&hdl->mutex, 0);
    list_for_each_entry(node, &hdl->sync_list, entry) {
        if (node->hdl == syncts) {
            goto remove_node;
        }
    }
    os_mutex_post(&hdl->mutex);

    return;
remove_node:
    log_debug(">>>>>>>>>>>>del tdm syncts %x\n", syncts);
    spin_lock(&hdl->lock);
    list_del(&node->entry);
    free(node);
    spin_unlock(&hdl->lock);
    os_mutex_post(&hdl->mutex);
}

static int tdm_adapter_syncts_ioctl(struct tdm_node_hdl *hdl, struct audio_syncts_ioc_params *params)
{
    if (!params) {
        return 0;
    }
    switch (params->cmd) {
    case AUDIO_SYNCTS_MOUNT_ON_SNDPCM:
        if (hdl->syncts_mount_en) {
            audio_tdm_add_syncts_with_timestamp(hdl, (void *)params->data[0], params->data[1]);
        }
        if (hdl->reference_network == 0xff) {
            hdl->reference_network = params->data[2];
        }
        break;
    case AUDIO_SYNCTS_UMOUNT_ON_SNDPCM:
        if (hdl->syncts_mount_en) {
            audio_tdm_remove_syncts_handle(hdl, (void *)params->data[0]);
        }
        break;
    }
    return 0;
}

static int tdm_adapter_ioctl(struct stream_iport *iport, int cmd, int arg)
{
    int ret = 0;
    struct tdm_node_hdl *hdl = (struct tdm_node_hdl *)iport->node->private_data;
    switch (cmd) {
    case NODE_IOC_OPEN_IPORT:
        tdm_adapter_open_iport(iport);
        break;
    case NODE_IOC_NEGOTIATE:
        *(int *)arg |= tdm_ioc_negotiate(iport);
        break;
    case NODE_IOC_SET_SCENE:
        hdl->scene = arg;
        break;
    case NODE_IOC_GET_DELAY:
        int master_delay = 0;
#if TCFG_MULTI_CH_TDM_NODE_ENABLE
        int self_delay = tdm_ioc_get_delay(hdl) / TDM_SLOT_NUM;
#else
        int self_delay = tdm_ioc_get_delay(hdl);
#endif
        /* return (self_delay ?self_delay: 1); */
        return self_delay;
    case NODE_IOC_START:
        tdm_ioc_start(hdl, iport);
        break;
    case NODE_IOC_SUSPEND:
    case NODE_IOC_STOP:
        tdm_ioc_stop(hdl);
        break;
    case NODE_IOC_SYNCTS:
        tdm_adapter_syncts_ioctl(hdl, (struct audio_syncts_ioc_params *)arg);
        break;
    case NODE_IOC_GET_ODEV_CACHE:
#if TCFG_MULTI_CH_TDM_NODE_ENABLE
        int wlen =  audio_tdm_data_len() / TDM_SLOT_NUM;
#else
        int wlen =  audio_tdm_data_len();
#endif
        return wlen;
    case NODE_IOC_SET_PARAM:
        hdl->reference_network = arg;
        break;
    case NODE_IOC_SET_TS_PARM:
        hdl->time_out_ts_not_align = arg;
        break;
    default:
        break;
    }
    return ret;
}

static int tdm_adapter_bind(struct stream_node *node, u16 uuid)
{
    struct tdm_node_hdl *hdl = (struct tdm_node_hdl *)node->private_data ;
    hdl->node = node;
    hdl->reference_network = 0xff;
    os_mutex_create(&hdl->mutex);
    spin_lock_init(&hdl->lock);
    INIT_LIST_HEAD(&(hdl->sync_list));
    return 0;
}


static void tdm_adapter_release(struct stream_node *node)
{
}
#if TCFG_MULTI_CH_TDM_NODE_ENABLE
REGISTER_STREAM_NODE_ADAPTER(multi_ch_tdm_node_adapter) = {
    .name       = "multi_ch_tdm",
    .uuid       = NODE_UUID_MULTI_CH_TDM_TX,
    .bind       = tdm_adapter_bind,
    .ioctl      = tdm_adapter_ioctl,
    .release    = tdm_adapter_release,
    .hdl_size   = sizeof(struct tdm_node_hdl),
};
#else
REGISTER_STREAM_NODE_ADAPTER(tdm_node_adapter) = {
    .name       = "tdm",
    .uuid       = NODE_UUID_TDM_TX,
    .bind       = tdm_adapter_bind,
    .ioctl      = tdm_adapter_ioctl,
    .release    = tdm_adapter_release,
    .hdl_size   = sizeof(struct tdm_node_hdl),
};
#endif
#endif

