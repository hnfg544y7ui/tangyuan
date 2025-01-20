#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".tdm_file.data.bss")
#pragma data_seg(".tdm_file.data")
#pragma const_seg(".tdm_file.text.const")
#pragma code_seg(".tdm_file.text")
#endif
#include "source_node.h"
#include "audio_dai/audio_tdm.h"
#include "media/audio_splicing.h"
#include "tdm_file.h"
#include "sync/audio_clk_sync.h"
#include "effects/effects_adj.h"
#include "app_msg.h"
#include "gpio_config.h"
#if 1
#define tdm_file_log	printf
#else
#define tdm_file_log(...)
#endif/*log_en*/

#if (TCFG_TDM_RX_ENABLE ||TCFG_MULTI_CH_TDM_RX_NODE_ENABLE)

struct tdm_file_hdl {
    void *source_node;
    u8 start;
    u8 dump_cnt;
    u8 ch_num;
    u8 timestamp_init_ch;
    u16 irq_points;
    u16 sample_rate;
    u32 timestamp;
    struct stream_frame *frame[4];
};

struct tdm_file_common {
    struct tdm_file_hdl *hdl;	//当前tdm节点的句柄
};

extern struct audio_tdm_hdl tdm_hdl;

static struct tdm_file_common tdm_f;

void audio_tdm_file_init(void)
{
}
__NODE_CACHE_CODE(tdm)
void tdm_sample_output_handler(s16 *data, int len)
{
    struct tdm_file_hdl *hdl = tdm_f.hdl;
    struct stream_frame *frame;
#if TCFG_MULTI_CH_TDM_RX_NODE_ENABLE
    u8 get_frame = 0;
    for (int ch_idx = 0; ch_idx < 4; ch_idx++) {
        hdl->frame[ch_idx] = source_plug_get_output_frame_by_id(hdl->source_node, ch_idx, len);
        if (hdl->frame[ch_idx]) {
            get_frame = 1;
        }
    }

    if (!get_frame) {
        return;
    }
    if (TDM_32_BIT_EN) {
        s32 *data0 = (hdl->frame[0] ? (s32 *)hdl->frame[0]->data : NULL);
        s32 *data1 = (hdl->frame[1] ? (s32 *)hdl->frame[1]->data : NULL);
        s32 *data2 = (hdl->frame[2] ? (s32 *)hdl->frame[2]->data : NULL);
        s32 *data3 = (hdl->frame[3] ? (s32 *)hdl->frame[3]->data : NULL);
        s32 *indata = (s32 *)data;
        for (int i = 0; i < len / 4 / TDM_SLOT_NUM; i++) {
            if (data0) {
                *data0++ = indata[8 * i];
                *data0++ = indata[8 * i + 1];
            }
            if (data1) {
                *data1++ = indata[8 * i + 2];
                *data1++ = indata[8 * i + 3];
            }
            if (data2) {
                *data2++ = indata[8 * i + 4];
                *data2++ = indata[8 * i + 5];
            }
            if (data3) {
                *data3++ = indata[8 * i + 6];
                *data3++ = indata[8 * i + 7];
            }
        }
    } else {
        s16 *data0 = hdl->frame[0] ? (s16 *)hdl->frame[0]->data : NULL;
        s16 *data1 = hdl->frame[1] ? (s16 *)hdl->frame[1]->data : NULL;
        s16 *data2 = hdl->frame[2] ? (s16 *)hdl->frame[2]->data : NULL;
        s16 *data3 = hdl->frame[3] ? (s16 *)hdl->frame[3]->data : NULL;
        s16 *indata = (s16 *)data;
        for (int i = 0; i < len / 2 / TDM_SLOT_NUM; i++) {
            if (data0) {
                *data0++ = indata[8 * i];
                *data0++ = indata[8 * i + 1];
            }
            if (data1) {
                *data1++ = indata[8 * i + 2];
                *data1++ = indata[8 * i + 3];
            }
            if (data2) {
                *data2++ = indata[8 * i + 4];
                *data2++ = indata[8 * i + 5];
            }
            if (data3) {
                *data3++ = indata[8 * i + 6];
                *data3++ = indata[8 * i + 7];
            }
        }
    }
    for (int ch_idx = 0; ch_idx < 4; ch_idx++) {
        frame = hdl->frame[ch_idx];
        if (frame) {
            if (!hdl->timestamp) {
                hdl->timestamp = audio_jiffies_usec() * TIMESTAMP_US_DENOMINATOR;
                hdl->timestamp_init_ch = ch_idx;
            }
            if (hdl->timestamp_init_ch == ch_idx) {
                hdl->timestamp = audio_jiffies_usec() * TIMESTAMP_US_DENOMINATOR;
            }
            frame->len          = len / TDM_SLOT_NUM * 2;
#if 0
            frame->flags        = FRAME_FLAG_SYS_TIMESTAMP_ENABLE;
            frame->timestamp    = audio_jiffies_usec();
#else
            frame->flags        = FRAME_FLAG_TIMESTAMP_ENABLE | FRAME_FLAG_PERIOD_SAMPLE | FRAME_FLAG_UPDATE_TIMESTAMP;
            frame->timestamp    = hdl->timestamp;
#endif
            source_plug_put_output_frame_by_id(hdl->source_node, ch_idx, frame);
            hdl->frame[ch_idx] = NULL;
        }
    }
#else
    frame = source_plug_get_output_frame(hdl->source_node, len);
    if (!frame) {
        return;
    }
    frame->len          = len;
    frame->flags        = FRAME_FLAG_TIMESTAMP_ENABLE | FRAME_FLAG_PERIOD_SAMPLE | FRAME_FLAG_UPDATE_TIMESTAMP;
    frame->timestamp    = hdl->timestamp;
    memcpy(frame->data, data, len);
    source_plug_put_output_frame(hdl->source_node, frame);
#endif
    return;
}

static void *tdm_file_init(void *source_node, struct stream_node *node)
{
    struct tdm_file_hdl *hdl = zalloc(sizeof(*hdl));
    tdm_f.hdl = hdl;
    hdl->source_node = source_node;
    tdm_file_log("tdm ch_num %d\n", hdl->ch_num);
    hdl->ch_num = 1;
    node->type |= NODE_TYPE_IRQ;
    return hdl;
}

static void tdm_ioc_get_fmt(struct tdm_file_hdl *hdl, struct stream_fmt *fmt)
{
    fmt->coding_type = AUDIO_CODING_PCM;
    fmt->sample_rate    = TDM_SAMPLE_RATE;
#if TCFG_MULTI_CH_TDM_RX_NODE_ENABLE
    fmt->channel_mode   = AUDIO_CH_LR;
#else
    fmt->channel_mode   = AUDIO_CH_MIX;
#endif
    hdl->sample_rate = TDM_SAMPLE_RATE;
    fmt->bit_wide = TDM_32_BIT_EN;
}

static int tdm_ioc_set_fmt(struct tdm_file_hdl *hdl, struct stream_fmt *fmt)
{
    hdl->sample_rate = fmt->sample_rate;
    return 0;
}

static int tdm_ioctl(void *_hdl, int cmd, int arg)
{
    u32 i = 0;
    int ret = 0;
    struct tdm_file_hdl *hdl = (struct tdm_file_hdl *)_hdl;
    switch (cmd) {
    case NODE_IOC_GET_FMT:
        tdm_ioc_get_fmt(hdl, (struct stream_fmt *)arg);
        break;
    case NODE_IOC_SET_FMT:
        ret = tdm_ioc_set_fmt(hdl, (struct stream_fmt *)arg);
        break;
    case NODE_IOC_SET_PRIV_FMT:
        break;
    case NODE_IOC_START:
        if (hdl->start == 0) {
            audio_tdm_rx_open(NULL);
            hdl->start = 1;
        }
        break;
    case NODE_IOC_SUSPEND:
    case NODE_IOC_STOP:
        if (hdl->start) {
            hdl->start = 0;
            audio_tdm_close();
        }
        break;
    }

    return ret;
}

static void tdm_release(void *_hdl)
{
    struct tdm_file_hdl *hdl = (struct tdm_file_hdl *)_hdl;
    free(hdl);
    tdm_f.hdl = NULL;
}

#if TCFG_MULTI_CH_TDM_RX_NODE_ENABLE
REGISTER_SOURCE_NODE_PLUG(multi_ch_tdm_file_plug) = {
    .uuid       = NODE_UUID_MULTI_CH_TDM_RX,
    .init       = tdm_file_init,
    .ioctl      = tdm_ioctl,
    .release    = tdm_release,
};
#else
REGISTER_SOURCE_NODE_PLUG(tdm_file_plug) = {
    .uuid       = NODE_UUID_TDM_RX,
    .init       = tdm_file_init,
    .ioctl      = tdm_ioctl,
    .release    = tdm_release,
};
#endif
#endif/*TCFG_TDM_RX_ENABLE*/
