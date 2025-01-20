#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".gpadc.data.bss")
#pragma data_seg(".gpadc.data")
#pragma const_seg(".gpadc.text.const")
#pragma code_seg(".gpadc.text")
#endif
#include "typedef.h"
#include "gpadc.h"
#include "timer.h"
#include "init.h"
#include "asm/efuse.h"
#include "irq.h"
#include "asm/power/p33.h"
#include "asm/power/p11.h"
#include "asm/power_interface.h"
#include "jiffies.h"
#include "debug.h"
#include "gpio.h"
#include "clock.h"
#include "asm/power_interface.h"
//br27

static u8 adc_ch_io_table[16] = {  //gpio->adc_ch 表
    IO_PORTA_02,
    IO_PORTA_05,
    IO_PORTA_06,
    IO_PORTA_10,
    IO_PORTA_12,
    IO_PORTB_01,
    IO_PORTB_03,
    IO_PORTB_04,
    IO_PORTB_08,
    IO_PORTB_10,
    IO_PORTB_11,
    IO_PORTC_02,
    IO_PORTC_03,
    IO_PORTC_04,
    IO_PORTC_05,
    IO_PORTC_06,
};

static void clock_critical_enter(void)
{
}
static void adc_adjust_div(void)
{
    adc_clk_div = 0x7f; //adc_clk_div 占7bit
    const u32 lsb_clk = clk_get("lsb");
    for (int i = 1; i < 0x80; i++) {
        if (lsb_clk / i <= 2000000) {
            adc_clk_div = i;
            break;
        }
    }
}
CLOCK_CRITICAL_HANDLE_REG(saradc, clock_critical_enter, adc_adjust_div)


u32 adc_io2ch(int gpio)   //根据传入的GPIO，返回对应的ADC_CH
{
    for (u8 i = 0; i < ARRAY_SIZE(adc_ch_io_table); i++) {
        if (adc_ch_io_table[i] == gpio) {
            return (ADC_CH_TYPE_IO | i);
        }
    }
    printf("add_adc_ch io error!!! change other io_port!!!\n");
    return 0xffffffff; //未找到支持ADC的IO
}

void adc_io_ch_set(enum AD_CH ch, u32 mode) //adc io通道 模式设置
{

}

void adc_internal_signal_to_io(u32 analog_ch, u16 adc_io) //将内部通道信号，接到IO口上，输出
{
    gpio_set_mode(IO_PORT_SPILT(adc_io), PORT_HIGHZ);
    /* gpio_set_function(IO_PORT_SPILT(adc_io), PORT_FUNC_GPADC); */


    adc_sample(analog_ch, 0);
    u32 ch = adc_io2ch(adc_io);
    u16 adc_ch_sel = ch & ADC_CH_MASK_CH_SEL;
    SFR(JL_ADC->CON, 19, 4, adc_ch_sel);
    SFR(JL_ADC->CON, 2, 2, 0b11);

}

static void adc_audio_ch_select(u32 ch_sel) //Audio通道切换
{
    SFR(JL_ADDA->ADDA_CON1, 1, 1, 1);
    SFR(JL_ADDA->ADDA_CON0, 0, 12, 0);

    switch (ch_sel) {
    case AD_CH_AUDIO_VADADC:
        SFR(JL_ADDA->ADDA_CON0, 12, 1, 1);
        break;
    case AD_CH_AUDIO_VCM:
        SFR(JL_ADDA->ADDA_CON0, 11, 1, 1);
        break;
    case AD_CH_AUDIO_VBGLDO:
        SFR(JL_ADDA->ADDA_CON0, 10, 1, 1);
        break;
    case AD_CH_AUDIO_HPVDD:
        SFR(JL_ADDA->ADDA_CON0, 9, 1, 1);
        break;
    case AD_CH_AUDIO_RN:
        SFR(JL_ADDA->ADDA_CON0, 4, 1, 1);
        break;
    case AD_CH_AUDIO_RP:
        SFR(JL_ADDA->ADDA_CON0, 3, 1, 1);
        break;
    case AD_CH_AUDIO_LN:
        SFR(JL_ADDA->ADDA_CON0, 2, 1, 1);
        break;
    case AD_CH_AUDIO_LP:
        SFR(JL_ADDA->ADDA_CON0, 1, 1, 1);
        break;
    case AD_CH_AUDIO_DACVDD:
        SFR(JL_ADDA->ADDA_CON0, 0, 1, 1);
        break;
#if 0
    case AD_AUDIO_MICIN0:
        SFR(JL_ADDA->ADDA_CON0, 5, 4, 0);
        break;
    case AD_AUDIO_MICIN1:
        SFR(JL_ADDA->ADDA_CON0, 5, 4, 1);
        break;
    case AD_AUDIO_MICIN2:
        SFR(JL_ADDA->ADDA_CON0, 5, 4, 2);
        break;
    case AD_AUDIO_MICIN3:
        SFR(JL_ADDA->ADDA_CON0, 5, 4, 3);
        break;
#endif
    default:
        break;
    }
}

u32 adc_add_sample_ch(enum AD_CH ch)   //添加一个指定的通道到采集队列
{
    u32 adc_type_sel = ch & ADC_CH_MASK_TYPE_SEL;
    u16 adc_ch_sel = ch & ADC_CH_MASK_CH_SEL;
    printf("type = %x,ch = %x\n", adc_type_sel, adc_ch_sel);
    u32 i = 0;
    for (i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            break;
        } else if (adc_queue[i].ch == -1) {
            adc_queue[i].ch = ch;
            adc_queue[i].v.value = (u16) - 1;
            adc_queue[i].sample_period = 0;
#if AD_CH_IO_VBAT_PORT
            if (ch == AD_CH_IO_VBAT_PORT) {
                adc_queue[i].adc_voltage_mode = 1;
                continue;
            }
#endif
            switch (ch) {
            case AD_CH_LDOREF:
                adc_queue[i].adc_voltage_mode = 0;
                break;
            case AD_CH_PMU_VBAT:
                adc_queue[i].adc_voltage_mode = 1;
                break;
            case AD_CH_PMU_VTEMP:
                adc_queue[i].adc_voltage_mode = 1;
                break;
            default:
                adc_queue[i].adc_voltage_mode = 0;
                break;
            }
            break;
        }
    }
    return i;
}

u32 adc_delete_ch(enum AD_CH ch)    //将一个指定的通道从采集队列中删除
{
    u32 i = 0;
    for (i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            adc_queue[i].ch = -1;
            break;
        }
    }
    return i;
}

void adc_sample(enum AD_CH ch, u32 ie) //启动一次cpu模式的adc采样
{
    u32 adc_con = 0;
    SFR(adc_con, 8, 7, adc_clk_div);
    adc_con |= (0x1 << 15); //启动延时控制，实际启动延时为此数值*8个ADC时钟
    adc_con |= BIT(31);//adc en
    if (ie) {
        adc_con |= BIT(1);
    }
    /* adc_con |= BIT(0);//adc ana always en */
    u32 adc_type_sel = ch & ADC_CH_MASK_TYPE_SEL;
    u16 adc_ch_sel = ch & ADC_CH_MASK_CH_SEL;
    SFR(adc_con, 2, 2, 0b10);//cpu adc test sel en
    SFR(adc_con, 23, 3, adc_type_sel >> 16);    //test sel
    switch (adc_type_sel) {
    case ADC_CH_TYPE_BT:
        break;
    case ADC_CH_TYPE_PMU:
        adc_pmu_ch_select(adc_ch_sel);
        break;
    case ADC_CH_TYPE_AUDIO:
        adc_audio_ch_select(adc_ch_sel);
        break;
    case ADC_CH_TYPE_LPCTM:
        break;
    case ADC_CH_TYPE_RTC:
        adc_rtc_ch_select(adc_ch_sel);
        /* adc_rtc_pd(1); */
        adc_rtc_detect_en(1);
        break;
    case ADC_CH_TYPE_PLL1:
        break;
    case ADC_CH_TYPE_FM:
        break;
    case ADC_CH_TYPE_HUSB:
        break;
    default:
        SFR(adc_con, 23, 3, 0);    //test sel
        SFR(adc_con,  2, 2, 0b01);//io_sel en
        SFR(adc_con, 19, 4, adc_ch_sel);    //io_sel
        break;
    }
    JL_ADC->CON = adc_con;
    JL_ADC->CON |= BIT(6);//kistart
}
static void adc_wait_idle_timeout()
{
    u32 time_out_us = 1000;
    while (time_out_us--) {
        asm("csync");
        if (JL_ADC->CON & BIT(7)) {
            break;
        }
        if ((JL_ADC->CON & BIT(31)) == 0) {
            break;
        }
        udelay(1);
    }
}
void adc_wait_enter_idle() //等待adc进入空闲状态，才可进行阻塞式采集
{
    if (JL_ADC->CON & BIT(31)) {
        adc_wait_idle_timeout();
        adc_close();
    } else {
        return ;
    }
}
void adc_set_enter_idle() //设置 adc cpu模式为空闲状态
{
    /* JL_ADC->CON &= ~BIT(31); */
}
u16 adc_wait_pnd()   //cpu采集等待pnd
{
    adc_wait_idle_timeout();
    u32 adc_res = JL_ADC->RES;
    adc_close();
    return adc_res;
}

void adc_dma_sample(enum AD_CH ch, u16 *buf, u32 sample_times)
{
    /* JL_PORTA->DIR &= ~BIT(7); */
    /* JL_PORTA->OUT |= BIT(7); */

    adc_queue[ADC_MAX_CH].ch = ch;

    adc_wait_enter_idle();

    local_irq_disable();

#if 1
    adc_sample(ch, 0);
    for (u32 i = 0; i < sample_times; i++) {
        adc_wait_idle_timeout();
        buf[i] = JL_ADC->RES;
        /* printf("res %d\n", buf[i]); */
        JL_ADC->CON |= BIT(6);
    }
#else
    u32 adc_con = 0;
    SFR(adc_con, 8, 7, adc_clk_div);
    /* adc_con |= (0x1 << 15); //启动延时控制，实际启动延时为此数值*8个ADC时钟 */
    adc_con |= BIT(31);//adc en
    u32 adc_type_sel = ch & ADC_CH_MASK_TYPE_SEL;
    u16 adc_ch_sel = ch & ADC_CH_MASK_CH_SEL;
    if (adc_type_sel == ADC_CH_TYPE_IO) {
        adc_con &= ~BIT(4); //IO通道
    } else {
        adc_con |= BIT(4); //内部测试通道
    }
    JL_ADC->CON = adc_con;
    JL_ADC->DMA_BADR = (u32)buf;
    JL_ADC->DMA_CON0 = (BIT(9) | BIT(6)); //clr_buf, clr_dma_pnd
    SFR(JL_ADC->DMA_CON0, 16, 16, sample_times);//DMA长度
    SFR(JL_ADC->DMA_CON1, 16, 1, 0);//dama_data = 10bit_res + 4bit_ch_num
    SFR(JL_ADC->DMA_CON1, 17, 15, 32);
    JL_ADC->DMA_CON1 |= BIT(ch & ADC_CH_MASK_CH_SEL);//使能对应DMA通道

    while (!(JL_ADC->DMA_CON0 & BIT(7)));

    JL_ADC->DMA_CON0 |= BIT(6);
    JL_ADC->DMA_CON1 &= ~BIT(ch & ADC_CH_MASK_CH_SEL);//关闭通道
#endif
    adc_close();

    adc_queue[ADC_MAX_CH].ch = -1;

    adc_set_enter_idle();

    local_irq_enable();

    /* JL_PORTA->OUT &= ~BIT(7); */
}

void adc_close()     //adc close
{
    JL_ADC->CON = BIT(17);//clock_en
    JL_ADC->CON = BIT(17) | BIT(6);
    JL_ADC->CON = BIT(6);

    check_set_miovdd_level_change();
}

___interrupt
static void adc_isr()   //中断函数
{
    if (adc_queue[ADC_MAX_CH].ch != -1) {
        adc_close();
        return;
    }

    if (JL_ADC->CON & BIT(7)) { //cpu模式
        u32 adc_value = JL_ADC->RES;
        if (adc_cpu_mode_process(adc_value)) {
            return;
        }
        cur_ch++;
        cur_ch = adc_get_next_ch();
        if (cur_ch == ADC_MAX_CH) {
            adc_close();
        } else {
            adc_sample(adc_queue[cur_ch].ch, 1);
        }
    }
}

static void adc_scan()  //定时函数，每 x ms启动一轮cpu模式采集
{
    if (adc_queue[ADC_MAX_CH].ch != -1) {
        return;
    }
    if (JL_ADC->CON & BIT(31)) {
        return;
    }
    cur_ch = adc_get_next_ch();
    if (cur_ch == ADC_MAX_CH) {
        return;
    }
    adc_sample(adc_queue[cur_ch].ch, 1);
}

void adc_hw_init(void)    //adc初始化子函数
{
    memset(adc_queue, 0xff, sizeof(adc_queue));

    adc_close();

    adc_adjust_div();

    adc_add_sample_ch(AD_CH_LDOREF);
    adc_set_sample_period(AD_CH_LDOREF, PMU_CH_SAMPLE_PERIOD);

    adc_add_sample_ch(AD_CH_PMU_VBAT);
    adc_set_sample_period(AD_CH_PMU_VBAT, PMU_CH_SAMPLE_PERIOD);

    adc_add_sample_ch(AD_CH_PMU_VTEMP);
    adc_set_sample_period(AD_CH_PMU_VTEMP, PMU_CH_SAMPLE_PERIOD);

#if AD_CH_IO_VBAT_PORT
    adc_add_sample_ch(adc_io2ch(AD_CH_IO_VBAT_PORT));
    adc_set_sample_period(adc_io2ch(AD_CH_IO_VBAT_PORT), PMU_CH_SAMPLE_PERIOD);
#endif

    u32 vbg_adc_value = 0;
    u32 vbg_min_value = -1;
    u32 vbg_max_value = 0;

    for (int i = 0; i < AD_CH_PMU_VBG_TRIM_NUM; i++) {
        u32 adc_value = adc_get_value_blocking(AD_CH_LDOREF);
        if (adc_value > vbg_max_value) {
            vbg_max_value = adc_value;
        } else if (adc_value < vbg_min_value) {
            vbg_min_value = adc_value;
        }
        vbg_adc_value += adc_value;
    }
    vbg_adc_value -= vbg_max_value;
    vbg_adc_value -= vbg_min_value;

    vbg_adc_value /= (AD_CH_PMU_VBG_TRIM_NUM - 2);
    adc_queue[0].v.value = vbg_adc_value;
    printf("LDOREF = %d\n", vbg_adc_value);

    u32 voltage = adc_get_voltage_blocking(AD_CH_PMU_VBAT);
    adc_queue[1].v.voltage = voltage;
    printf("vbat = %d mv\n", adc_get_voltage(AD_CH_PMU_VBAT) * 4);

    voltage = adc_get_voltage_blocking(AD_CH_PMU_VTEMP);
    adc_queue[2].v.voltage = voltage;
    printf("dtemp = %d mv\n", voltage);

#if AD_CH_IO_VBAT_PORT
    voltage = adc_get_voltage_blocking(adc_io2ch(AD_CH_IO_VBAT_PORT));
    adc_queue[3].v.voltage = voltage;
    printf("io_vbat = %d mv\n", voltage);
#endif

    /* read_hpvdd_voltage(); */
    //切换到vbg通道


    request_irq(IRQ_GPADC_IDX, 1, adc_isr, 0);//注册中断函数
    usr_timer_add(NULL, adc_scan,  5, 0);

    /* adc_add_sample_ch(adc_io2ch(IO_PORTA_02)); */
    /* adc_set_sample_period(adc_io2ch(IO_PORTA_02), PMU_CH_SAMPLE_PERIOD); */
    /* void adc_test_demo(); */
    /* usr_timer_add(NULL, adc_test_demo, 1000, 0); //打印信息 */
}

static void adc_test_demo()  //adc测试函数，根据需求搭建
{
    /* printf("\n\n%s() CHIP_ID :%x\n", __func__, get_chip_version()); */
    /* printf("%s() VBG:%d\n", __func__, adc_get_value(AD_CH_LDOREF)); */
    printf("%s() VBAT:%d mv\n", __func__, adc_get_voltage(AD_CH_PMU_VBAT) * 4);
    /* printf("%s() PA2:%d mv\n", __func__, adc_get_voltage(adc_io2ch(AD_CH_IO_VBAT_PORT))); */
    /* printf("%s() DTEMP:%d\n", __func__, adc_get_voltage(AD_CH_PMU_VTEMP)); */
    /* printf("%s() PA2:%d\n", __func__, adc_get_value(AD_CH_IO_PA2)); */
    /* printf("%s() PA2:%dmv\n", __func__, adc_get_voltage_blocking(AD_CH_IO_PA2)); */
    /* printf("%s() PA10:%dmv\n", __func__, adc_get_voltage_blocking(AD_CH_IO_PA10)); */
    /* printf("%s() PA6:%dmv\n", __func__, adc_get_voltage_blocking(AD_CH_IO_PA6)); */
    /* printf("%s() PB1:%d\n", __func__, adc_get_value(AD_CH_IO_PB1)); */
    /* printf("%s() PB1:%dmv\n", __func__, adc_get_voltage(AD_CH_IO_PB1)); */
}

static u8 gpadc_idle_query(void)
{
    if (JL_ADC->CON & BIT(31)) {
        return 0; //不可以进入休眠
    }
    return 1; //可以进入休眠
}
REGISTER_LP_TARGET(gpadc_driver_target) = {
    .name = "gpadc",
    .is_idle = gpadc_idle_query,
};
