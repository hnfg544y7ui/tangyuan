#include "key_driver.h"
#include "app_main.h"
#include "init.h"
#include "scene_switch.h"
#include "mic_effect.h"
#include "wireless_trans.h"
#include "le_broadcast.h"
#include "dsp_mode.h"

#if (TCFG_SPI_LCD_ENABLE)
#include "ui/ui_api.h"
#endif

#if TCFG_APP_DSP_EN
int dsp_app_msg_handler(int *msg)
{

    if (false == app_in_mode(APP_MODE_DSP)) {
        return 0;
    }
    switch (msg[0]) {
    case APP_MSG_DSP_OPEN:
        dsp_start();
        break;
    case APP_MSG_DSP_CLOSE:
        dsp_stop();
        break;
#if (TCFG_AS_WIRELESS_MIC_DSP_ENABLE)
    case APP_MSG_VOL_UP:
        dsp_dvol_up();
        break;
    case APP_MSG_VOL_DOWN:
        dsp_dvol_down();
        break;
    case APP_MSG_MUSIC_MUTE:
        if (get_dsp_mute_status()) {
            dsp_dvol_mute(0);
        } else {
            dsp_dvol_mute(1);
        }
        break;
    case APP_MSG_PLATE_REVERB_PARAM_SWITCH:
        app_plate_reverb_parm_switch(0xff);
        break;
    case APP_MSG_PLATE_REVERB_BYPASS:
        if (get_dsp_reverb_bypass_status()) {
            app_plate_reverb_parm_switch(0);
        } else {
            app_plate_reverb_parm_switch(1);
        }
        break;
    case APP_MSG_LLNS_DNS_PARAM_SWITCH:
        app_llns_dns_parm_switch(0xff);
        break;
    case APP_MSG_LLNS_DNS_BYPASS:
        if (get_dsp_llns1_bypass_status() && get_dsp_llns2_bypass_status()) {
            app_llns_dns_parm_switch(0);
        } else {
            app_llns_dns_parm_switch(1);
        }
        break;
#endif
    default:
        app_common_key_msg_handler(msg);
        break;
    }

    return 0;
}

#endif

