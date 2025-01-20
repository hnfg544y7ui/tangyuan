

//===============================================================================//
//
//      output function define
//
//===============================================================================//
#define FO_GP_OCH0        ((0 << 2)|BIT(1))
#define FO_GP_OCH1        ((1 << 2)|BIT(1))
#define FO_GP_OCH2        ((2 << 2)|BIT(1))
#define FO_GP_OCH3        ((3 << 2)|BIT(1))
#define FO_GP_OCH4        ((4 << 2)|BIT(1))
#define FO_GP_OCH5        ((5 << 2)|BIT(1))
#define FO_GP_OCH6        ((6 << 2)|BIT(1))
#define FO_GP_OCH7        ((7 << 2)|BIT(1))
#define FO_GP_OCH8        ((8 << 2)|BIT(1))
#define FO_GP_OCH9        ((9 << 2)|BIT(1))
#define FO_SPI0_CLK        ((10 << 2)|BIT(1)|BIT(0))
#define FO_SPI0_DA0        ((11 << 2)|BIT(1)|BIT(0))
#define FO_SPI0_DA1        ((12 << 2)|BIT(1)|BIT(0))
#define FO_SPI0_DA2        ((13 << 2)|BIT(1)|BIT(0))
#define FO_SPI0_DA3        ((14 << 2)|BIT(1)|BIT(0))
#define FO_SPI1_CLK        ((15 << 2)|BIT(1)|BIT(0))
#define FO_SPI1_DA0        ((16 << 2)|BIT(1)|BIT(0))
#define FO_SPI1_DA1        ((17 << 2)|BIT(1)|BIT(0))
#define FO_SPI1_DA2        ((18 << 2)|BIT(1)|BIT(0))
#define FO_SPI1_DA3        ((19 << 2)|BIT(1)|BIT(0))
#define FO_SPI2_CLK        ((20 << 2)|BIT(1)|BIT(0))
#define FO_SPI2_DA0        ((21 << 2)|BIT(1)|BIT(0))
#define FO_SPI2_DA1        ((22 << 2)|BIT(1)|BIT(0))
#define FO_SPI2_DA2        ((23 << 2)|BIT(1)|BIT(0))
#define FO_SPI2_DA3        ((24 << 2)|BIT(1)|BIT(0))
#define FO_SD0_CLK        ((25 << 2)|BIT(1)|BIT(0))
#define FO_SD0_CMD        ((26 << 2)|BIT(1)|BIT(0))
#define FO_SD0_DA0        ((27 << 2)|BIT(1)|BIT(0))
#define FO_SD0_DA1        ((28 << 2)|BIT(1)|BIT(0))
#define FO_SD0_DA2        ((29 << 2)|BIT(1)|BIT(0))
#define FO_SD0_DA3        ((30 << 2)|BIT(1)|BIT(0))
#define FO_SD1_CLK        ((31 << 2)|BIT(1)|BIT(0))
#define FO_SD1_CMD        ((32 << 2)|BIT(1)|BIT(0))
#define FO_SD1_DA0        ((33 << 2)|BIT(1)|BIT(0))
#define FO_SD1_DA1        ((34 << 2)|BIT(1)|BIT(0))
#define FO_SD1_DA2        ((35 << 2)|BIT(1)|BIT(0))
#define FO_SD1_DA3        ((36 << 2)|BIT(1)|BIT(0))
#define FO_IIC_SCL        ((37 << 2)|BIT(1)|BIT(0))
#define FO_IIC_SDA        ((38 << 2)|BIT(1)|BIT(0))
#define FO_UART0_TX        ((39 << 2)|BIT(1)|BIT(0))
#define FO_UART0_RTS        ((40 << 2)|BIT(1)|BIT(0))
#define FO_UART1_TX        ((41 << 2)|BIT(1)|BIT(0))
#define FO_UART1_RTS        ((42 << 2)|BIT(1)|BIT(0))
#define FO_UART2_TX        ((43 << 2)|BIT(1)|BIT(0))
#define FO_UART2_RTS        ((44 << 2)|BIT(1)|BIT(0))
#define FO_TDM_M_MCK        ((45 << 2)|BIT(1)|BIT(0))
#define FO_TDM_M_WCK        ((46 << 2)|BIT(1)|BIT(0))
#define FO_TDM_M_BCK        ((47 << 2)|BIT(1)|BIT(0))
#define FO_TDM_S_DA        ((48 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_MCLK        ((49 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_LRCK        ((50 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_SCLK        ((51 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_DAT0        ((52 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_DAT1        ((53 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_DAT2        ((54 << 2)|BIT(1)|BIT(0))
#define FO_ALNK0_DAT3        ((55 << 2)|BIT(1)|BIT(0))
#define FO_ALNK1_MCLK        ((56 << 2)|BIT(1)|BIT(0))
#define FO_ALNK1_LRCK        ((57 << 2)|BIT(1)|BIT(0))
#define FO_ALNK1_SCLK        ((58 << 2)|BIT(1)|BIT(0))
#define FO_ALNK1_DAT0        ((59 << 2)|BIT(1)|BIT(0))
#define FO_ALNK1_DAT1        ((60 << 2)|BIT(1)|BIT(0))
#define FO_ALNK1_DAT2        ((61 << 2)|BIT(1)|BIT(0))
#define FO_ALNK1_DAT3        ((62 << 2)|BIT(1)|BIT(0))
#define FO_SPDIF_DO        ((63 << 2)|BIT(1)|BIT(0))
#define FO_PLNK_SCLK        ((64 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM0_H        ((65 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM1_H        ((66 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM2_H        ((67 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM3_H        ((68 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM4_H        ((69 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM5_H        ((70 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM0_L        ((71 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM1_L        ((72 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM2_L        ((73 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM3_L        ((74 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM4_L        ((75 << 2)|BIT(1)|BIT(0))
#define FO_MCPWM5_L        ((76 << 2)|BIT(1)|BIT(0))
#define FO_WL_ANT_ID0        ((77 << 2)|BIT(1)|BIT(0))
#define FO_WL_ANT_ID1        ((78 << 2)|BIT(1)|BIT(0))
#define FO_WL_ANT_ID2        ((79 << 2)|BIT(1)|BIT(0))
#define FO_WL_ANT_ID3        ((80 << 2)|BIT(1)|BIT(0))
#define FO_CHAIN_OUT0        ((81 << 2)|BIT(1)|BIT(0))
#define FO_CHAIN_OUT1        ((82 << 2)|BIT(1)|BIT(0))
#define FO_CHAIN_OUT2        ((83 << 2)|BIT(1)|BIT(0))
#define FO_CHAIN_OUT3        ((84 << 2)|BIT(1)|BIT(0))

//===============================================================================//
//
//      IO output select sfr
//
//===============================================================================//
typedef struct {
    __RW __u8 PA0_OUT;
    __RW __u8 PA1_OUT;
    __RW __u8 PA2_OUT;
    __RW __u8 PA3_OUT;
    __RW __u8 PA4_OUT;
    __RW __u8 PA5_OUT;
    __RW __u8 PA6_OUT;
    __RW __u8 PA7_OUT;
    __RW __u8 PA8_OUT;
    __RW __u8 PA9_OUT;
    __RW __u8 PA10_OUT;
    __RW __u8 PA11_OUT;
    __RW __u8 PA12_OUT;
    __RW __u8 PA13_OUT;
    __RW __u8 PA14_OUT;
    __RW __u8 PA15_OUT;
    __RW __u8 PB0_OUT;
    __RW __u8 PB1_OUT;
    __RW __u8 PB2_OUT;
    __RW __u8 PB3_OUT;
    __RW __u8 PB4_OUT;
    __RW __u8 PB5_OUT;
    __RW __u8 PB6_OUT;
    __RW __u8 PB7_OUT;
    __RW __u8 PB8_OUT;
    __RW __u8 PB9_OUT;
    __RW __u8 PB10_OUT;
    __RW __u8 PB11_OUT;
    __RW __u8 PB12_OUT;
    __RW __u8 PB13_OUT;
    __RW __u8 PB14_OUT;
    __RW __u8 PB15_OUT;
    __RW __u8 PC0_OUT;
    __RW __u8 PC1_OUT;
    __RW __u8 PC2_OUT;
    __RW __u8 PC3_OUT;
    __RW __u8 PC4_OUT;
    __RW __u8 PC5_OUT;
    __RW __u8 PC6_OUT;
    __RW __u8 PC7_OUT;
    __RW __u8 PC8_OUT;
    __RW __u8 PC9_OUT;
    __RW __u8 PC10_OUT;
    __RW __u8 PC11_OUT;
    __RW __u8 PC12_OUT;
    __RW __u8 USBDP_OUT;
    __RW __u8 USBDM_OUT;
    __RW __u8 PP0_OUT;
} JL_OMAP_TypeDef;

#define JL_OMAP_BASE      (ls_base + map_adr(0x58, 0x00))
#define JL_OMAP           ((JL_OMAP_TypeDef   *)JL_OMAP_BASE)
