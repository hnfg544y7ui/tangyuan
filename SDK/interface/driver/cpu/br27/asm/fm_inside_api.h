#ifndef __FM_INSIDE_API_H_
#define __FM_INSIDE_API_H_
#include "typedef.h"

void fm_inside_on(void);
void fm_inside_off(void);
u8   fm_inside_freq_set(u32 freq);
void fm_inside_int_set(u8 mute);
void fm_inside_start(void);
void fm_inside_pause(void);
u16  fm_inside_id_read(void);
void fm_inside_set_stereo(u8 set);
u32 fm_inside_get_stereo(void);
s32  fm_inside_rssi_read(void); //unit DB
s32 fm_inside_cnr_read(void);
//more inside fm api function, see file fm_inside_api.h

//Following function Call is valid Only after fm_inside_on();
u32 fm_inside_agc_trim(u8 gain_sel);
void fm_inside_agc_en_set(u8 enable);
u32 fm_inside_agc_en_get();
void fm_inside_mute_set(u8 mute);
u32 fm_inside_stereo_detect(void);
#endif

