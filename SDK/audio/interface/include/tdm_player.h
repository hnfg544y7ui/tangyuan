#ifndef TDM_PLAYER_H
#define TDM_PLAYER_H

#include "effect/effects_default_param.h"

int tdm_player_open();

void tdm_player_close();

bool tdm_player_runing();

int tdm_file_pitch_up();

int tdm_file_pitch_down();

int tdm_file_set_pitch(enum _pitch_level pitch_mode);

void tdm_file_pitch_mode_init(enum _pitch_level pitch_mode);
#endif
