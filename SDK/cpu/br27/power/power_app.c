#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".power_app.data.bss")
#pragma data_seg(".power_app.data")
#pragma const_seg(".power_app.text.const")
#pragma code_seg(".power_app.text")
#endif
#include "asm/power_interface.h"
#include "app_config.h"
#include "includes.h"
#include "cpu/includes.h"
#include "gpadc.h"
#include "asm/efuse.h"
#include "gpio_config.h"
#include "rtc.h"

//--------------------------------------------------------
/*config
 */
#define CONFIG_UART_DEBUG_ENABLE	CONFIG_DEBUG_ENABLE
#ifdef TCFG_DEBUG_UART_TX_PIN
#define CONFIG_UART_DEBUG_PORT		TCFG_DEBUG_UART_TX_PIN
#else
#define CONFIG_UART_DEBUG_PORT		-1
#endif

#define DO_PLATFORM_UNINITCALL()	do_platform_uninitcall()

static u32 usb_io_con = 0;
void sleep_enter_callback()
{
    /* 此函数禁止添加打印 */
#if (defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)
    psram_flush_all_cache();
    if (psram_check_memory_used_status()) {
        psram_heap_reset();
        psram_enter_sleep(PSRAM_STATE_POWER_OFF);
    } else {
        psram_enter_sleep(PSRAM_STATE_POWER_STANDBY);
    }
#endif

    putchar('<');

    //USB IO打印引脚特殊处理
#if (CONFIG_DEBUG_ENABLE && ((TCFG_DEBUG_UART_TX_PIN == IO_PORT_DP) || (TCFG_DEBUG_UART_TX_PIN == IO_PORT_DM))) || \
    (TCFG_CFG_TOOL_ENABLE && (TCFG_COMM_TYPE == TCFG_UART_COMM) && ((TCFG_ONLINE_TX_PORT == IO_PORT_DP) || (TCFG_ONLINE_RX_PORT == IO_PORT_DM))) || \
    (TCFG_CFG_TOOL_ENABLE && (TCFG_COMM_TYPE == TCFG_USB_COMM)) || \
    TCFG_USB_HOST_ENABLE || TCFG_PC_ENABLE
    usb_io_con = JL_USB_IO->CON0;
#endif

    gpio_close(PORTUSB, 0xffff);
}

void sleep_exit_callback()
{
#if (defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)
    psram_exit_sleep();
#endif

#if (CONFIG_DEBUG_ENABLE && ((TCFG_DEBUG_UART_TX_PIN == IO_PORT_DP) || (TCFG_DEBUG_UART_TX_PIN == IO_PORT_DM))) || \
    (TCFG_CFG_TOOL_ENABLE && (TCFG_COMM_TYPE == TCFG_UART_COMM) && ((TCFG_ONLINE_TX_PORT == IO_PORT_DP) || (TCFG_ONLINE_RX_PORT == IO_PORT_DM))) || \
    (TCFG_CFG_TOOL_ENABLE && (TCFG_COMM_TYPE == TCFG_USB_COMM)) || \
    TCFG_USB_HOST_ENABLE || TCFG_PC_ENABLE
    JL_USB_IO->CON0 = usb_io_con;
#endif

    putchar('>');
}

static void __mask_io_cfg()
{
    struct boot_soft_flag_t boot_soft_flag = {0};
    boot_soft_flag.flag1.misc.usbdm = SOFTFLAG_HIGH_RESISTANCE;
    boot_soft_flag.flag1.misc.usbdp = SOFTFLAG_HIGH_RESISTANCE;

    boot_soft_flag.flag1.misc.ldoin = SOFTFLAG_HIGH_RESISTANCE;

    boot_soft_flag.flag2.pb12_pb13.pb12 = SOFTFLAG_HIGH_RESISTANCE;
    boot_soft_flag.flag2.pb12_pb13.pb13 = SOFTFLAG_HIGH_RESISTANCE;

    boot_soft_flag.flag3.pb14_res.pb14 = SOFTFLAG_HIGH_RESISTANCE;

    mask_softflag_config(&boot_soft_flag);
}

u8 power_soff_callback()
{
    DO_PLATFORM_UNINITCALL();

    poweroff_save_rtc_time();

    __mask_io_cfg();

    void gpio_config_soft_poweroff(void);
    gpio_config_soft_poweroff();

    gpio_config_uninit();

    return 0;
}

void power_early_flowing()
{
    // 默认关闭长按复位0，由key_driver配置
    /* gpio_longpress_pin0_reset_config(IO_PORTB_01, 0, 0, 1, 1); */
    // 不开充电功能，将长按复位关闭
#if (!TCFG_CHARGE_ENABLE)
    gpio_longpress_pin1_reset_config(IO_LDOIN_DET, 0, 0);
#endif

    power_early_init(0);
}

//early_initcall(_power_early_init);

static int power_later_init()
{
    pmu_trim(0, 0);

    return 0;
}

late_initcall(power_later_init);
