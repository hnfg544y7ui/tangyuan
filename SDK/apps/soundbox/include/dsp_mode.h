#ifndef _DSP_MODE_H_
#define _DSP_MODE_H_

#include "system/event.h"

struct app_mode *app_enter_dsp_mode(int arg);

int dsp_start(void);
void dsp_stop(void);
u8 dsp_get_status(void);

u8 get_dsp_source(void);

void set_dsp_source(u8 source);

int dsp_app_msg_handler(int *msg);

void app_plate_reverb_parm_switch(int bypass);
void app_llns_dns_parm_switch(int bypass);

int dsp_set_dvol(u8 vol, s16 mute_en);
void dsp_dvol_mute(bool mute);
void dsp_dvol_up(void);
void dsp_dvol_down(void);
void dsp_effect_status_init(void);
u8 get_dsp_llns1_bypass_status(void);
u8 get_dsp_llns2_bypass_status(void);
u8 get_dsp_reverb_bypass_status(void);
bool get_dsp_mute_status(void);
#endif


