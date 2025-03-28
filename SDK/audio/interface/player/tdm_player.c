#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".tdm_player.data.bss")
#pragma data_seg(".tdm_player.data")
#pragma const_seg(".tdm_player.text.const")
#pragma code_seg(".tdm_player.text")
#endif
#include "jlstream.h"
#include "tdm_player.h"
#include "sdk_config.h"
#include "effects/audio_pitchspeed.h"
#include "scene_switch.h"
#include "audio_config_def.h"
#include "effects/audio_vbass.h"
#include "audio_dai/audio_tdm.h"
#if AUDIO_EQ_LINK_VOLUME
#include "effects/eq_config.h"
#endif
struct tdm_player {
    struct jlstream *stream;
    s8 tdm_pitch_mode;
};
static struct tdm_player *g_tdm_player = NULL;

static void tdm_player_callback(void *private_data, int event)
{
    struct tdm_player *player = g_tdm_player;
    struct jlstream *stream = (struct jlstream *)private_data;

    switch (event) {
    case STREAM_EVENT_START:
#if TCFG_VOCAL_REMOVER_NODE_ENABLE
        musci_vocal_remover_update_parm();
#endif
#if AUDIO_VBASS_LINK_VOLUME
        vbass_link_volume();
#endif
#if AUDIO_EQ_LINK_VOLUME
        eq_link_volume();
#endif
        break;
    }
}

int tdm_player_open()
{
    int err = 0;
    struct tdm_player *player = g_tdm_player;
    if (player) {
        return -EFAULT;
    }

    u16 uuid = jlstream_event_notify(STREAM_EVENT_GET_PIPELINE_UUID, (int)"tdm");
    if (uuid == 0) {
        return -EFAULT;
    }

    player = malloc(sizeof(*player));
    if (!player) {
        return -ENOMEM;
    }

    player->tdm_pitch_mode = PITCH_0;

#if TCFG_MULTI_CH_TDM_RX_NODE_ENABLE
    player->stream = jlstream_pipeline_parse(uuid, NODE_UUID_MULTI_CH_TDM_RX);
#else
    player->stream = jlstream_pipeline_parse(uuid, NODE_UUID_TDM_RX);
#endif
    if (!player->stream) {
        err = -ENOMEM;
        goto __exit0;
    }

    //设置中断点数
    jlstream_node_ioctl(player->stream, NODE_UUID_SOURCE, NODE_IOC_SET_PRIV_FMT, TDM_IRQ_POINTS);

    jlstream_set_callback(player->stream, player->stream, tdm_player_callback);
    jlstream_set_scene(player->stream, STREAM_SCENE_TDM);
    err = jlstream_start(player->stream);
    if (err) {
        goto __exit1;
    }

    g_tdm_player = player;

    return 0;

__exit1:
    jlstream_release(player->stream);
__exit0:
    free(player);
    return err;
}

bool tdm_player_runing()
{
    return g_tdm_player != NULL;
}

void tdm_player_close()
{
    struct tdm_player *player = g_tdm_player;

    if (!player) {
        return;
    }
    jlstream_stop(player->stream, 50);
    jlstream_release(player->stream);

    free(player);
    g_tdm_player = NULL;

    jlstream_event_notify(STREAM_EVENT_CLOSE_PLAYER, (int)"tdm");
}


//变调接口
int tdm_file_pitch_up()
{
    struct tdm_player *player = g_tdm_player;
    if (!player) {
        return -1;
    }
    float pitch_param_table[] = {-12, -10, -8, -6, -4, -2, 0, 2, 4, 6, 8, 10, 12};
    player->tdm_pitch_mode++;
    if (player->tdm_pitch_mode > ARRAY_SIZE(pitch_param_table) - 1) {
        player->tdm_pitch_mode = ARRAY_SIZE(pitch_param_table) - 1;
    }
    printf("play pitch up+++%d\n", player->tdm_pitch_mode);
    int ret = tdm_file_set_pitch(player->tdm_pitch_mode);
    ret = (ret == true) ? player->tdm_pitch_mode : -1;
    return ret;
}

int tdm_file_pitch_down()
{
    struct tdm_player *player = g_tdm_player;
    if (!player) {
        return -1;
    }
    player->tdm_pitch_mode--;
    if (player->tdm_pitch_mode < 0) {
        player->tdm_pitch_mode = 0;
    }
    printf("play pitch down---%d\n", player->tdm_pitch_mode);
    int ret = tdm_file_set_pitch(player->tdm_pitch_mode);
    ret = (ret == true) ? player->tdm_pitch_mode : -1;
    return ret;
}

int tdm_file_set_pitch(enum _pitch_level pitch_mode)
{
    struct tdm_player *player = g_tdm_player;
    float pitch_param_table[] = {-12, -10, -8, -6, -4, -2, 0, 2, 4, 6, 8, 10, 12};
    if (pitch_mode > ARRAY_SIZE(pitch_param_table) - 1) {
        pitch_mode = ARRAY_SIZE(pitch_param_table) - 1;
    }
    pitch_speed_param_tool_set pitch_param = {
        .pitch = pitch_param_table[pitch_mode],
        .speed = 1,
    };
    if (player) {
        player->tdm_pitch_mode = pitch_mode;
        return jlstream_node_ioctl(player->stream, NODE_UUID_PITCH_SPEED, NODE_IOC_SET_PARAM, (int)&pitch_param);
    }
    return -1;
}

void tdm_file_pitch_mode_init(enum _pitch_level pitch_mode)
{
    struct tdm_player *player = g_tdm_player;
    if (player) {
        player->tdm_pitch_mode = pitch_mode;
    }
}
