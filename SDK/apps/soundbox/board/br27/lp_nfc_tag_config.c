#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".lp_nfc_tag_config.data.bss")
#pragma data_seg(".lp_nfc_tag_config.data")
#pragma const_seg(".lp_nfc_tag_config.text.const")
#pragma code_seg(".lp_nfc_tag_config.text")
#endif
#include "app_config.h"
#include "syscfg_id.h"
#include "key_driver.h"
#include "cpu/includes.h"
#include "system/init.h"
#include "asm/lp_nfc_tag.h"
#include "sdk_config.h"


//*********************************************************************************//
//                             lp nfc bt tag 配置                                  //
//*********************************************************************************//

#if TCFG_LP_NFC_TAG_ENABLE

// *INDENT-OFF*
LP_NFC_TAG_PLATFORM_DATA_BEGIN(lp_nfc_tag_pdata)
    .tag_type = TCFG_LP_NFC_TAG_TYPE,
    .rx_io_mode = TCFG_LP_NFC_TAG_RX_IO_MODE,
    .ctl_wkup_cyc_sel = TCFG_LP_NFC_TAG_WKUP_SENSITIVITY,
LP_NFC_TAG_PLATFORM_DATA_END()


int board_lp_nfc_tag_config()
{
    lp_nfc_tag_init(&lp_nfc_tag_pdata);

    return 0;
}
late_initcall(board_lp_nfc_tag_config);

#endif

