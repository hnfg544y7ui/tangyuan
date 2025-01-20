#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".board_config.data.bss")
#pragma data_seg(".board_config.data")
#pragma const_seg(".board_config.text.const")
#pragma code_seg(".board_config.text")
#endif
// V300挪去sdk_board_config.c
/* #include "asm/power_interface.h" */
/* #include "gpadc.h" */
/* #include "app_config.h" */
/* #include "app_power_config.h" */
/*  */
/* #if TCFG_APP_FM_EN */
/* #include "fm_manage.h" */
/*  */
/* FM_DEV_PLATFORM_DATA_BEGIN(fm_dev_data) */
/* .iic_hdl = 0, */
/*  .iic_delay = 50, */
/*   FM_DEV_PLATFORM_DATA_END(); */
/*  */
/* #endif */
/*  */
/* extern void bt_wl_sync_clk_en(void); */
/*  */
/* #if (defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE) */
/* #include "asm/psram_api.h" */
/* #include "gpio.h" */
/* PSRAM_PLATFORM_DATA_BEGIN(psram_config) */
/* .power_port = TCFG_PSRAM_POWER_PORT, */
/*  .port = TCFG_PSRAM_PORT_SEL, */
/*   .psram_clk_mhz = TCFG_PSRAM_INIT_CLK, */
/*    .data_width = TCFG_PSRAM_MODE, */
/*     PSRAM_PLATFORM_DATA_END() */
/*  */
/*     void psram_early_init(void) */
/* { */
/*     psram_init(&psram_config); */
/* } */
/*  */
/* #else */
/* void psram_early_init(void) */
/* { */
/*     return; */
/* } */
/* #endif */
/*  */
/* void board_init() */
/* { */
/*     board_power_init(); */
/*  */
/*     #<{(| #if (defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE) |)}># */
/*     #<{(|     psram_init(&psram_config); |)}># */
/*     #<{(| #endif #<{(| #if TCFG_PSRAM_DEV_ENABLE |)}># |)}># */
/*  */
/* #if TCFG_UPDATE_UART_IO_EN */
/*     { */
/* #include "uart_update.h" */
/*         uart_update_cfg  update_cfg = { */
/*             .rx = TCFG_UART_UPDATE_TX_PIN, */
/*             .tx = TCFG_UART_UPDTAE_RX_PIN, */
/*         }; */
/*         y_printf(">>>[test]:old uart update\n"); */
/*         uart_update_init(&update_cfg); */
/*     } */
/* #endif */
/*  */
/* #if CONFIG_UPDATE_MUTIL_CPU_UART */
/*     { */
/* #include "update_interactive_uart.h" */
/*         update_interactive_uart_cfg  update_interactive_cfg = { */
/*             .rx = CONFIG_UPDATE_MUTIL_CPU_UART_TX_PIN, */
/*             .tx = CONFIG_UPDATE_MUTIL_CPU_UART_RX_PIN, */
/*         }; */
/*         y_printf(">>>[test]:new uart update\n"); */
/*         update_interactive_uart_init(&update_interactive_cfg); */
/*     } */
/* #endif */
/*  */
/*     adc_init(); */
/*  */
/*     bt_wl_sync_clk_en(); */
/*  */
/* #if TCFG_APP_FM_EN */
/*     y_printf(">> Func:%s, Line:%d, call: fm_dev_init Func!\n", __func__, __LINE__); */
/*     fm_dev_init((void *)(&fm_dev_data)); */
/* #endif */
/*  */
/* } */
