#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".lp_nfc_tag.data.bss")
#pragma data_seg(".lp_nfc_tag.data")
#pragma const_seg(".lp_nfc_tag.text.const")
#pragma code_seg(".lp_nfc_tag.text")
#endif
#include "includes.h"
#include "app_config.h"
#include "asm/power/p11.h"
#include "asm/power_interface.h"
#include "asm/lp_nfc_tag.h"

#define LOG_TAG_CONST       LP_NFC
#define LOG_TAG             "[LP_NFC]"
/* #define LOG_ERROR_ENABLE */
/* #define LOG_DEBUG_ENABLE */
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug.h"

#if TCFG_LP_NFC_TAG_ENABLE

#define JIELI_TAG_ID        0x00//制造商ID

#define TAG_UID0            JIELI_TAG_ID
#define TAG_UID1            0x00
#define TAG_UID2            0x00
#define TAG_UID3            0x00
#define TAG_UID4            0x00
#define TAG_UID5            0x00
#define TAG_UID6            0x00

#define TAG_BUF_SIZE        256 //取值36~256
#define TAG_CC2             ((TAG_BUF_SIZE - 32) / 8)

static u8 nfc_tag_buf[TAG_BUF_SIZE] = {
    TAG_UID0, TAG_UID1, TAG_UID2,     0x00,
    TAG_UID3, TAG_UID4, TAG_UID5, TAG_UID6,
    0x00,     0x00,     0x00,     0x00,
    0xe1,     0x10,  TAG_CC2,     0x00,
    0x03,     0x00,     0xFE,     0x00,
};

extern const u8 *bt_get_mac_addr();
extern const char *bt_get_local_name();

static const struct lp_nfc_tag_platform_data *__this = NULL;

static u8 lp_nfc_softoff_mode = 0;


#define LP_NFC_IO_RX    IO_PORTB_02
#define LP_NFC_IO_TX    IO_PORTB_03
static void lp_nfc_tag_port_init(void)
{
    gpio_set_mode(LP_NFC_IO_RX / 16, BIT(LP_NFC_IO_RX % 16), PORT_HIGHZ);
    gpio_set_mode(LP_NFC_IO_TX / 16, BIT(LP_NFC_IO_TX % 16), PORT_HIGHZ);
}

static void lp_nfc_tag_send_cmd(enum NFC_M2P_CMD cmd)
{
    P2M_NFC_REPLY_CMD = 0;
    M2P_NFC_CMD = cmd;
    P11_SYSTEM->M2P_INT_SET = BIT(M2P_NFC_INDEX);
    while (!(P2M_NFC_REPLY_CMD == cmd));
}

void lp_nfc_tag_set_uid(const u8 *bt_mac, u8 set_only)
{
    u8 *tag_uid = (u8 *)&M2P_NFC_UID0;
    tag_uid[0] = JIELI_TAG_ID;
    for (u32 i = 0; i < 6; i ++) {
        tag_uid[1 + i] = bt_mac[i] + JL_RAND->R64L;
    }

    log_info("M2P_NFC_UID0 = 0x%x", M2P_NFC_UID0);
    log_info("M2P_NFC_UID1 = 0x%x", M2P_NFC_UID1);
    log_info("M2P_NFC_UID2 = 0x%x", M2P_NFC_UID2);
    log_info("M2P_NFC_UID3 = 0x%x", M2P_NFC_UID3);
    log_info("M2P_NFC_UID4 = 0x%x", M2P_NFC_UID4);
    log_info("M2P_NFC_UID5 = 0x%x", M2P_NFC_UID5);
    log_info("M2P_NFC_UID6 = 0x%x", M2P_NFC_UID6);

    if (set_only) {
        lp_nfc_tag_send_cmd(NFC_M2P_UP_UID);
    }
}

void lp_nfc_jl_bt_tag_set_mac(const u8 *bt_mac, u8 set_only)
{
    u8 *tag_mac = (u8 *)&M2P_NFC_BT_MAC0;
    memcpy(tag_mac, bt_mac, 6);

    log_info("M2P_NFC_BT_MAC0 = 0x%x", M2P_NFC_BT_MAC0);
    log_info("M2P_NFC_BT_MAC1 = 0x%x", M2P_NFC_BT_MAC1);
    log_info("M2P_NFC_BT_MAC2 = 0x%x", M2P_NFC_BT_MAC2);
    log_info("M2P_NFC_BT_MAC3 = 0x%x", M2P_NFC_BT_MAC3);
    log_info("M2P_NFC_BT_MAC4 = 0x%x", M2P_NFC_BT_MAC4);
    log_info("M2P_NFC_BT_MAC5 = 0x%x", M2P_NFC_BT_MAC5);

    if (set_only) {
        lp_nfc_tag_send_cmd(NFC_M2P_UP_BT_MAC);
    }
}

void lp_nfc_jl_bt_tag_set_name(const char *bt_name, u8 set_only)
{
    char *tag_name = (char *)&M2P_NFC_BT_NAME_BEGIN;
    memset(tag_name, 0, 32);
    memcpy(tag_name, bt_name, strlen(bt_name));
    if (set_only) {
        lp_nfc_tag_send_cmd(NFC_M2P_UP_BT_NAME);
    }
}


void lp_nfc_tag_dump(void)
{
    log_info("P2M_NFC_BUF_ADDRH             = 0x%x", P2M_NFC_BUF_ADDRH);
    log_info("P2M_NFC_BUF_ADDRL             = 0x%x", P2M_NFC_BUF_ADDRL);
    log_info("M2P_NFC_TAG_TYPE              = 0x%x", M2P_NFC_TAG_TYPE);
    log_info("M2P_NFC_GAIN_CFG              = 0x%x", M2P_NFC_GAIN_CFG);
    log_info("M2P_NFC_CUR_TRIM_CFG0         = 0x%x", M2P_NFC_CUR_TRIM_CFG0);
    log_info("M2P_NFC_CUR_TRIM_CFG1         = 0x%x", M2P_NFC_CUR_TRIM_CFG1);
    log_info("M2P_NFC_RX_IO_MODE            = 0x%x", M2P_NFC_RX_IO_MODE);
    log_info("M2P_NFC_OP_MODE               = 0x%x", M2P_NFC_OP_MODE);
    log_info("M2P_NFC_BF_CFG                = 0x%x", M2P_NFC_BF_CFG);
    log_info("M2P_NFC_PAUSE_CFG             = 0x%x", M2P_NFC_PAUSE_CFG);
    log_info("M2P_NFC_IDLE_TIMEOUT_TIME     = 0x%x", M2P_NFC_IDLE_TIMEOUT_TIME);
    log_info("M2P_NFC_CTL_WKUP_CYC_SEL      = 0x%x", M2P_NFC_CTL_WKUP_CYC_SEL);
    log_info("M2P_NFC_CTL_INVALID_CYC_SEL   = 0x%x", M2P_NFC_CTL_INVALID_CYC_SEL);
    log_info("M2P_NFC_CTL_VALID_TIME        = 0x%x", M2P_NFC_CTL_VALID_TIME);
    log_info("M2P_NFC_CTL_SCAN_TIME         = 0x%x", M2P_NFC_CTL_SCAN_TIME);
}

void lp_nfc_tag_save_buf_to_vm(void *data_from)
{
    int ret = syscfg_write(VM_LP_NFC_TAG_BUF_DATA, data_from, sizeof(nfc_tag_buf));
    if (ret != sizeof(nfc_tag_buf)) {
        log_info("write vm nfc_tag_buf error !\n");
    }
}

void lp_nfc_tag_init(const struct lp_nfc_tag_platform_data *pdata)
{
    log_info("*****************  lp nfc tag init ****************");
    __this = pdata;

    lp_nfc_softoff_mode = LP_NFC_SOFTOFF_MODE_ADVANCE;

    lp_nfc_tag_port_init();

    int ret = syscfg_read(VM_LP_NFC_TAG_BUF_DATA, nfc_tag_buf, sizeof(nfc_tag_buf));
    if (ret != sizeof(nfc_tag_buf)) {
        log_info("read vm nfc_tag_buf error !\n");
        ret = syscfg_write(VM_LP_NFC_TAG_BUF_DATA, nfc_tag_buf, sizeof(nfc_tag_buf));
        if (ret != sizeof(nfc_tag_buf)) {
            log_info("write vm nfc_tag_buf error !\n");
        }
    }

    if (is_reset_source(MSYS_P2M_RST)) {//软关机起来，不重新初始化nfc
        log_info("lp nfc continue work !\n");
        lp_nfc_tag_dump();
        if (__this->tag_type == CUSTOM_TAG) {
            u8 *p11_nfc_tag_buf_addr = (u8 *)(((P2M_NFC_BUF_ADDRH << 8) | P2M_NFC_BUF_ADDRL) + 0xf20000);
            if (memcmp(p11_nfc_tag_buf_addr, nfc_tag_buf, sizeof(nfc_tag_buf))) {
                lp_nfc_tag_save_buf_to_vm((void *)p11_nfc_tag_buf_addr);
            }
        }
        return;
    }

    if (__this->tag_type == JL_BT_TAG) {
        lp_nfc_jl_bt_tag_set_mac(bt_get_mac_addr(), 0);
        lp_nfc_jl_bt_tag_set_name(bt_get_local_name(), 0);
    } else {
        lp_nfc_tag_send_cmd(NFC_M2P_UP_BUF_ADDR);
        u8 *p11_nfc_tag_buf_addr = (u8 *)(((P2M_NFC_BUF_ADDRH << 8) | P2M_NFC_BUF_ADDRL) + 0xf20000);
        memcpy(p11_nfc_tag_buf_addr, nfc_tag_buf, sizeof(nfc_tag_buf));
    }

    lp_nfc_tag_set_uid(bt_get_mac_addr(), 0);

    M2P_NFC_TAG_TYPE            =  __this->tag_type;
    M2P_NFC_GAIN_CFG            = (__this->gain_cfg1 << 4) | __this->gain_cfg0;
    M2P_NFC_CUR_TRIM_CFG0       =  __this->cur_trim_cfg0;
    M2P_NFC_CUR_TRIM_CFG1       =  __this->cur_trim_cfg1;
    M2P_NFC_RX_IO_MODE          =  __this->rx_io_mode;
    M2P_NFC_OP_MODE             =  __this->op_mode;
    M2P_NFC_BF_CFG              = (__this->bf_cfg > 0b100) ? 0b100 : __this->bf_cfg;
    M2P_NFC_PAUSE_CFG           =  __this->pause_cfg;
    M2P_NFC_IDLE_TIMEOUT_TIME   =  __this->idle_timeout_time;
    M2P_NFC_CTL_WKUP_CYC_SEL    = (__this->ctl_wkup_cyc_sel > 0b100) ? 0b100 : __this->ctl_wkup_cyc_sel;
    M2P_NFC_CTL_INVALID_CYC_SEL	=  __this->ctl_invalid_cyc_sel;
    M2P_NFC_CTL_VALID_TIME      =  __this->ctl_valid_time;
    M2P_NFC_CTL_SCAN_TIME       =  __this->ctl_scan_time;

    lp_nfc_tag_dump();

    lp_nfc_tag_send_cmd(NFC_M2P_INIT);
}

void lp_nfc_tag_enable(void)
{
    lp_nfc_tag_send_cmd(NFC_M2P_ENABLE);
    lp_nfc_softoff_mode = LP_NFC_SOFTOFF_MODE_ADVANCE;
}

void lp_nfc_tag_disable(void)
{
    lp_nfc_tag_send_cmd(NFC_M2P_DISABLE);
    lp_nfc_softoff_mode = LP_NFC_SOFTOFF_MODE_LEGACY;
}

u8 lp_nfc_tag_softoff_mode_query(void)
{
    return lp_nfc_softoff_mode;
}

void lp_nfc_tag_event_irq_handler(void)
{
    u8 nfc_event = P2M_NFC_TAG_EVENT;
    P2M_NFC_TAG_EVENT = 0;
    log_info("nfc wakeup event = 0x%x\n", nfc_event);
    switch (nfc_event) {
    case NFC_P2M_CHANGE_BUF_EVENT:
        if (__this->tag_type == CUSTOM_TAG) {
            u8 *p11_nfc_tag_buf_addr = (u8 *)(((P2M_NFC_BUF_ADDRH << 8) | P2M_NFC_BUF_ADDRL) + 0xf20000);
            sys_timeout_add((void *)p11_nfc_tag_buf_addr, lp_nfc_tag_save_buf_to_vm, 1);
        }
        break;
    default:
        break;
    }
}


#endif

