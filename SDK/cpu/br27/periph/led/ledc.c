#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".ledc.data.bss")
#pragma data_seg(".ledc.data")
#pragma const_seg(".ledc.text.const")
#pragma code_seg(".ledc.text")
#endif
#include "asm/includes.h"



#define     MAX_HW_LEDC 3
static JL_LED_TypeDef *JL_LEDCx[MAX_HW_LEDC] = {
    JL_LED0,
    JL_LED1,
    JL_LED2,
};

static void (*ledc_irq_cbfun[MAX_HW_LEDC])();

static OS_SEM ledc_sem[MAX_HW_LEDC];


___interrupt
void ledc_isr(void)
{
    for (int i = 0; i < MAX_HW_LEDC; i++) {

        if (JL_LEDCx[i]->CON & BIT(7)) {
            JL_LEDCx[i]->CON |= BIT(6);
            os_sem_post(&ledc_sem[i]);
            if (ledc_irq_cbfun[i]) {
                ledc_irq_cbfun[i]();
            }
        }
    }
}

static u8 t_div[9] = {1, 2, 3, 6, 12, 24, 48, 96, 192};
void ledc_init(const struct ledc_platform_data *arg)
{
    gpio_set_mode(IO_PORT_SPILT(arg->port), PORT_OUTPUT_LOW);
    gpio_set_function(IO_PORT_SPILT(arg->port), PORT_FUNC_LEDC0_OUT + arg->index);

    //std_48M
    JL_LEDCK->CLK &= ~(0b11 << 0);
    JL_LEDCK->CLK |= (0b10 << 0);
    //set div
    JL_LEDCK->CLK &= ~(0xff << 8);
    JL_LEDCK->CLK |= ((t_div[arg->t_unit] - 1) << 8);

    JL_LEDCx[arg->index]->CON = BIT(6);

    os_sem_create(&ledc_sem[arg->index], 1);

    request_irq(IRQ_LEDC_IDX, 1, ledc_isr, 0);

    if (arg->cbfun) {
        ledc_irq_cbfun[arg->index] = arg->cbfun;
    }

    if (arg->idle_level) {
        JL_LEDCx[arg->index]->CON |= BIT(4);
    }
    if (arg->out_inv) {
        JL_LEDCx[arg->index]->CON |= BIT(3);
    }

    JL_LEDCx[arg->index]->CON |= (arg->bit_inv << 1);

    JL_LEDCx[arg->index]->TIX = 0;
    JL_LEDCx[arg->index]->TIX |= ((arg->t1h_cnt - 1) << 24);
    JL_LEDCx[arg->index]->TIX |= ((arg->t1l_cnt - 1) << 16);
    JL_LEDCx[arg->index]->TIX |= ((arg->t0h_cnt - 1) << 8);
    JL_LEDCx[arg->index]->TIX |= ((arg->t0l_cnt - 1) << 0);

    JL_LEDCx[arg->index]->RSTX = 0;
    JL_LEDCx[arg->index]->RSTX |= (arg->t_rest_cnt << 8);

    printf("JL_LEDCK->CLK = 0x%x\n", JL_LEDCK->CLK);
    printf("JL_LEDC%d->CON = 0x%x\n", arg->index, JL_LEDCx[arg->index]->CON);
    printf("JL_LEDC%d->TIX = 0x%x\n", arg->index, JL_LEDCx[arg->index]->TIX);
    printf("JL_LEDC%d->RSTX = 0x%x\n", arg->index, JL_LEDCx[arg->index]->RSTX);
}

void ledc_rgb_to_buf(u8 r, u8 g, u8 b, u8 *buf, int idx)
{
    buf = buf + idx * 3;
    buf[2] = b;
    buf[1] = r;
    buf[0] = g;
}

void ledc_send_rgbbuf(u8 index, u8 *rgbbuf, u32 led_num, u16 again_cnt)
{
    if (!led_num) {
        return;
    }
    JL_LEDCx[index]->ADR = (u32)rgbbuf;
    JL_LEDCx[index]->FD = led_num * 3 * 8;
    JL_LEDCx[index]->LP = again_cnt;
    JL_LEDCx[index]->CON |= BIT(0);//启动
    JL_LEDCx[index]->CON &= ~BIT(5);//关闭中断
    while (!(JL_LEDCx[index]->CON & BIT(7)));
    JL_LEDCx[index]->CON |= BIT(6);
}

void ledc_send_rgbbuf_isr(u8 index, u8 *rgbbuf, u32 led_num, u16 again_cnt)
{
    if (!led_num) {
        return;
    }
    os_sem_pend(&ledc_sem[index], 0);
    JL_LEDCx[index]->ADR = (u32)rgbbuf;
    JL_LEDCx[index]->FD = led_num * 3 * 8;
    JL_LEDCx[index]->LP = again_cnt;
    JL_LEDCx[index]->CON |= BIT(0);//启动
    JL_LEDCx[index]->CON |= BIT(5);//使能中断
}

// *INDENT-OFF*
/*******************************    参考示例 ***********************************/

LEDC_PLATFORM_DATA_BEGIN(ledc0_data)
    .index = 0,
    .port = IO_PORTA_00,
    .idle_level = 0,
    .out_inv = 0,
    .bit_inv = 1,
    .t_unit = t_42ns,
    .t1h_cnt = 24,
    .t1l_cnt = 7,
    .t0h_cnt = 7,
    .t0l_cnt = 24,
    .t_rest_cnt = 20000,
    .cbfun = NULL,
LEDC_PLATFORM_DATA_END()

LEDC_PLATFORM_DATA_BEGIN(ledc1_data)
    .index = 1,
    .port = IO_PORTA_01,
    .idle_level = 0,
    .out_inv = 0,
    .bit_inv = 1,
    .t_unit = t_42ns,
    .t1h_cnt = 24,
    .t1l_cnt = 7,
    .t0h_cnt = 7,
    .t0l_cnt = 24,
    .t_rest_cnt = 20000,
    .cbfun = NULL,
LEDC_PLATFORM_DATA_END()

LEDC_PLATFORM_DATA_BEGIN(ledc2_data)
    .index = 2,
    .port = IO_PORTA_02,
    .idle_level = 0,
    .out_inv = 0,
    .bit_inv = 1,
    .t_unit = t_42ns,
    .t1h_cnt = 24,
    .t1l_cnt = 7,
    .t0h_cnt = 7,
    .t0l_cnt = 24,
    .t_rest_cnt = 20000,
    .cbfun = NULL,
LEDC_PLATFORM_DATA_END()

#define LED_NUM_MAX     5
static u8 ledc_test_buf[3 * LED_NUM_MAX] __attribute__((aligned(4)));
void ledc_test(void)
{
    printf("*************  ledc test  **************\n");

    ledc_init(&ledc0_data);
    ledc_init(&ledc1_data);
    ledc_init(&ledc2_data);

    u8 r_val = 0;
    u8 g_val = 85;
    u8 b_val = 175;
    u16 sec_num = 5;//循环发送的次数，用于一条大灯带又分为几条效果一样的小灯带
    extern void wdt_clear();
    while (1) {
        wdt_clear();
        os_time_dly(1);
        r_val += 1;
        g_val += 1;
        b_val += 1;
        ledc_rgb_to_buf(r_val, g_val, b_val, ledc_test_buf, 0);
#if 0
        ledc_send_rgbbuf(0, ledc_test_buf, 1, sec_num - 1);
        ledc_send_rgbbuf(1, ledc_test_buf, 1, sec_num - 1);
        ledc_send_rgbbuf(2, ledc_test_buf, 1, sec_num - 1);
#else
        JL_PORTA->DIR &= ~BIT(3);
        JL_PORTA->OUT &= ~BIT(3);
        JL_PORTA->OUT |=  BIT(3);
        ledc_send_rgbbuf_isr(0, ledc_test_buf, 1, sec_num - 1);
        JL_PORTA->OUT &= ~BIT(3);
        JL_PORTA->OUT |=  BIT(3);
        ledc_send_rgbbuf_isr(1, ledc_test_buf, 1, sec_num - 1);
        JL_PORTA->OUT &= ~BIT(3);
        JL_PORTA->OUT |=  BIT(3);
        ledc_send_rgbbuf_isr(2, ledc_test_buf, 1, sec_num - 1);
        JL_PORTA->OUT &= ~BIT(3);
#endif
    }
}

