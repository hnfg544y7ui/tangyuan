#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".iovdd_adaptive.data.bss")
#pragma data_seg(".iovdd_adaptive.data")
#pragma const_seg(".iovdd_adaptive.text.const")
#pragma code_seg(".iovdd_adaptive.text")
#endif
#include "asm/power_interface.h"
#include "asm/audio_common.h"
#include "gpadc.h"


static u8 cur_miovdd_level = 0;
static u8 new_miovdd_level = 0;

void check_set_miovdd_level_change(void)
{
    if (new_miovdd_level) {
        adc_update_vbg_value_restart(cur_miovdd_level, new_miovdd_level);
        vddiom_vol_sel(new_miovdd_level);
        new_miovdd_level = 0;
    }
}

u16 audio_power_level_to_voltage(u8 level)
{
    u16 vol = (level % 5) * 200 + 2700;
    return vol;
}

u8 miovdd_voltage_to_level(u16 target_vol)
{
    u8 level = VDDIOM_VOL_36V;
    for (; level > 0; level --) {
        if (target_vol >= (2100 + level * 100)) {
            return level;
        }
    }
    return 0;
}

void miovdd_adaptive_adjustment(void)
{
    cur_miovdd_level = get_vddiom_vol_sel();
    if (cur_miovdd_level >= VDDIOM_VOL_33V) {
        return;
    }
    u8 miovdd_level = 0;
    u16 vbat_vol = adc_get_voltage(AD_CH_PMU_VBAT) * 4;
    if (vbat_vol >= 3600) {
        miovdd_level = VDDIOM_VOL_33V;
    } else {
        audio_common_param_t *audio_param = audio_common_get_param();
        u16 audio_power_vol = audio_power_level_to_voltage(audio_param->power_level);
        u16 miovdd_target_vol = vbat_vol - 300;;
        miovdd_target_vol = miovdd_target_vol > audio_power_vol ? miovdd_target_vol : audio_power_vol;
        miovdd_level = miovdd_voltage_to_level(miovdd_target_vol);
    }
    cur_miovdd_level = get_vddiom_vol_sel();
    if (cur_miovdd_level != miovdd_level) {
        new_miovdd_level = miovdd_level;
    } else {
        new_miovdd_level = 0;
    }
}

