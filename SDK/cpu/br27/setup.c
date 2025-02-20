#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".setup.data.bss")
#pragma data_seg(".setup.data")
#pragma const_seg(".setup.text.const")
#pragma code_seg(".setup.text")
#endif
#include "cpu/includes.h"
#include "system/includes.h"
#include "cpu/includes.h"
#include "app_config.h"
#include "asm/power_interface.h"
#include "uart.h"


#define LOG_TAG             "[SETUP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_CLI_ENABLE
#include "debug.h"

extern void sys_timer_init(void);
extern void tick_timer_init(void);
extern void vPortSysSleepInit(void);
extern u8 power_reset_source_dump(void);
extern void exception_irq_handler(void);
extern int __crc16_mutex_init();
extern void psram_early_init(void);



#if 0
___interrupt
void exception_irq_handler(void)
{
    ___trig;

    exception_analyze();

    log_flush();
    while (1);
}
#endif



/*
 * 此函数在cpu0上电后首先被调用,负责初始化cpu内部模块
 *
 * 此函数返回后，操作系统才开始初始化并运行
 *
 */

#if 0
static void early_putchar(char a)
{
    if (a == '\n') {
        UT2_BUF = '\r';
        __asm_csync();
        while ((UT2_CON & BIT(15)) == 0);
    }
    UT2_BUF = a;
    __asm_csync();
    while ((UT2_CON & BIT(15)) == 0);
}

void early_puts(char *s)
{
    do {
        early_putchar(*s);
    } while (*(++s));
}
#endif

void cpu_assert_debug()
{
    asm("[--sp] = {rets}");
#if CONFIG_DEBUG_ENABLE
    log_flush();
    local_irq_disable();
    while (1);
#else
    system_reset(ASSERT_FLAG);
#endif
}

_NOINLINE_
void cpu_assert(char *file, int line, bool condition, char *cond_str)
{
    if (config_asser) {
        if (!(condition)) {
            printf("cpu %d file:%s, line:%d\n", current_cpu_id(), file, line);
            printf("ASSERT-FAILD: %s\n", cond_str);
            cpu_assert_debug();
        }
    } else {
        if (!(condition)) {
            system_reset(ASSERT_FLAG);
        }
    }
}

void timer(void *p)
{
#if 1
    extern void mem_unfree_dump();
    mem_unfree_dump();
#else
    /* DEBUG_SINGAL_1S(1); */
    sys_timer_dump_time();
    printf("cpu %d run", current_cpu_id());
    /* DEBUG_SINGAL_1S(0);*/
#endif
}

extern void sputchar(char c);
extern void sput_buf(const u8 *buf, int len);
void sput_u32hex(u32 dat);
void *vmem_get_phy_adr(void *vaddr);



__attribute__((weak))
void maskrom_init(void)
{
    return;
}


#if (CPU_CORE_NUM > 1)
void cpu1_setup_arch()
{
    q32DSP(core_num())->PMU_CON1 &= ~BIT(8); //open bpu

    request_irq(IRQ_EXCEPTION_IDX, 7, exception_irq_handler, 1);

    //用于控制其他核进入停止状态。
    extern void cpu_suspend_handle(void);
    request_irq(IRQ_SOFT0_IDX, 7, cpu_suspend_handle, 0);
    request_irq(IRQ_SOFT1_IDX, 7, cpu_suspend_handle, 1);
    irq_unmask_set(IRQ_SOFT0_IDX, 0, 0); //设置CPU0软中断0为不可屏蔽中断
    irq_unmask_set(IRQ_SOFT1_IDX, 0, 1); //设置CPU1软中断1为不可屏蔽中断

    debug_init();
}
void cpu1_main()
{
    extern void cpu1_run_notify(void);
    cpu1_run_notify();

    interrupt_init();

    cpu1_setup_arch();

    os_start();

    log_e("os err \r\n") ;
    while (1) {
        __asm__ volatile("idle");
    }
}
#else
void cpu1_setup_arch()
{
    return;
}
void cpu1_main()
{

}
#endif /* #if (OS_CPU_NUM > 1) */
//==================================================//

void memory_init(void);

extern int ISR_BASE;
AT(.volatile_ram_code)
void *ota_jump_mode_param_prepare(u32 spi_port)
{
    struct boot_soft_flag_t *boot_soft_flag = (struct boot_soft_flag_t *)&ISR_BASE;
    memset((u8 *)boot_soft_flag, 0x0, sizeof(struct boot_soft_flag_t));
    boot_soft_flag->flag4.fast_boot_ctrl.flash_spi_baud = 1;
    /* boot_soft_flag.flag0.boot_ctrl.flash_port = spi_port; */
    boot_soft_flag->flag0.boot_ctrl.flash_port = ((JL_IOMAP->CON0 & BIT(16)) ? 1 : 0) + 1;
    return (void *)boot_soft_flag;
}

__attribute__((weak))
void app_main()
{
    while (1) {
        asm("idle");
    }
}

void setup_arch()
{
    //关闭所有timer的ie使能
    bit_clr_ie(IRQ_TIME0_IDX);
    bit_clr_ie(IRQ_TIME1_IDX);
    bit_clr_ie(IRQ_TIME2_IDX);
    bit_clr_ie(IRQ_TIME3_IDX);
    bit_clr_ie(IRQ_TIME4_IDX);
    bit_clr_ie(IRQ_TIME5_IDX);

#if TCFG_LONG_PRESS_RESET_ENABLE
    gpio_longpress_pin0_reset_config(TCFG_LONG_PRESS_RESET_PORT, TCFG_LONG_PRESS_RESET_LEVEL, TCFG_LONG_PRESS_RESET_TIME, 1, TCFG_LONG_PRESS_RESET_INSIDE_PULL_UP_DOWN);
#else
    gpio_longpress_pin0_reset_config(IO_PORTB_01, 0, 0, 1, 1);
#endif

    memory_init();

    wdt_init(WDT_8S);
    /* wdt_close(); */

    efuse_init();

    clk_voltage_init(TCFG_CLOCK_MODE, DVDD_VOL_SEL_1025V);
    clk_early_init(PLL_REF_XOSC_DIFF, TCFG_CLOCK_OSC_HZ, 320 * MHz);

    os_init();
    tick_timer_init();

    //上电初始所有io
    port_init();
    port_hd_init(1);

    psram_early_init();

#if CONFIG_DEBUG_ENABLE || CONFIG_DEBUG_LITE_ENABLE
    void debug_uart_init();
    debug_uart_init();

#if CONFIG_DEBUG_ENABLE
    log_early_init(1024);
#endif
#endif

    log_i("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    log_i("         setup_arch");
    log_i("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");


    clock_dump();

    power_early_flowing();

    //Register debugger interrupt
    request_irq(0, 2, exception_irq_handler, 0);
    request_irq(1, 2, exception_irq_handler, 0);

    code_movable_init();

    sys_timer_init();

    debug_init();

    __crc16_mutex_init();

    /* sys_timer_add(NULL, timer, 1 * 1000); */

    app_main();
}

