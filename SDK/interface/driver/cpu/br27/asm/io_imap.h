

//===============================================================================//
//
//      input IO define
//
//===============================================================================//
#define PA0_IN  1
#define PA1_IN  2
#define PA2_IN  3
#define PA3_IN  4
#define PA4_IN  5
#define PA5_IN  6
#define PA6_IN  7
#define PA7_IN  8
#define PA8_IN  9
#define PA9_IN  10
#define PA10_IN  11
#define PA11_IN  12
#define PA12_IN  13
#define PA13_IN  14
#define PA14_IN  15
#define PA15_IN  16
#define PB0_IN  17
#define PB1_IN  18
#define PB2_IN  19
#define PB3_IN  20
#define PB4_IN  21
#define PB5_IN  22
#define PB6_IN  23
#define PB7_IN  24
#define PB8_IN  25
#define PB9_IN  26
#define PB10_IN  27
#define PB11_IN  28
#define PB12_IN  29
#define PB13_IN  30
#define PB14_IN  31
#define PB15_IN  32
#define PC0_IN  33
#define PC1_IN  34
#define PC2_IN  35
#define PC3_IN  36
#define PC4_IN  37
#define PC5_IN  38
#define PC6_IN  39
#define PC7_IN  40
#define PC8_IN  41
#define PC9_IN  42
#define PC10_IN  43
#define PC11_IN  44
#define PC12_IN  45
#define USBDP_IN  46
#define USBDM_IN  47
#define PP0_IN  48

//===============================================================================//
//
//      function input select sfr
//
//===============================================================================//
typedef struct {
    __RW __u8 FI_GP_ICH0;
    __RW __u8 FI_GP_ICH1;
    __RW __u8 FI_GP_ICH2;
    __RW __u8 FI_GP_ICH3;
    __RW __u8 FI_GP_ICH4;
    __RW __u8 FI_GP_ICH5;
    __RW __u8 FI_GP_ICH6;
    __RW __u8 FI_GP_ICH7;
    __RW __u8 FI_GP_ICH8;
    __RW __u8 FI_GP_ICH9;
    __RW __u8 FI_SPI0_CLK;
    __RW __u8 FI_SPI0_DA0;
    __RW __u8 FI_SPI0_DA1;
    __RW __u8 FI_SPI0_DA2;
    __RW __u8 FI_SPI0_DA3;
    __RW __u8 FI_SPI1_CLK;
    __RW __u8 FI_SPI1_DA0;
    __RW __u8 FI_SPI1_DA1;
    __RW __u8 FI_SPI1_DA2;
    __RW __u8 FI_SPI1_DA3;
    __RW __u8 FI_SPI2_CLK;
    __RW __u8 FI_SPI2_DA0;
    __RW __u8 FI_SPI2_DA1;
    __RW __u8 FI_SPI2_DA2;
    __RW __u8 FI_SPI2_DA3;
    __RW __u8 FI_SD0_CMD;
    __RW __u8 FI_SD0_DA0;
    __RW __u8 FI_SD0_DA1;
    __RW __u8 FI_SD0_DA2;
    __RW __u8 FI_SD0_DA3;
    __RW __u8 FI_SD1_CMD;
    __RW __u8 FI_SD1_DA0;
    __RW __u8 FI_SD1_DA1;
    __RW __u8 FI_SD1_DA2;
    __RW __u8 FI_SD1_DA3;
    __RW __u8 FI_IIC_SCL;
    __RW __u8 FI_IIC_SDA;
    __RW __u8 FI_UART0_RX;
    __RW __u8 FI_UART0_CTS;
    __RW __u8 FI_UART1_RX;
    __RW __u8 FI_UART1_CTS;
    __RW __u8 FI_UART2_RX;
    __RW __u8 FI_UART2_CTS;
    __RW __u8 FI_TDM_S_WCK;
    __RW __u8 FI_TDM_S_BCK;
    __RW __u8 FI_TDM_M_DA;
    __RW __u8 FI_QDEC0_DAT0;
    __RW __u8 FI_QDEC0_DAT1;
    __RW __u8 FI_QDEC1_DAT0;
    __RW __u8 FI_QDEC1_DAT1;
    __RW __u8 FI_QDEC2_DAT0;
    __RW __u8 FI_QDEC2_DAT1;
    __RW __u8 FI_ALNK0_MCLK;
    __RW __u8 FI_ALNK0_LRCK;
    __RW __u8 FI_ALNK0_SCLK;
    __RW __u8 FI_ALNK0_DAT0;
    __RW __u8 FI_ALNK0_DAT1;
    __RW __u8 FI_ALNK0_DAT2;
    __RW __u8 FI_ALNK0_DAT3;
    __RW __u8 FI_ALNK1_MCLK;
    __RW __u8 FI_ALNK1_LRCK;
    __RW __u8 FI_ALNK1_SCLK;
    __RW __u8 FI_ALNK1_DAT0;
    __RW __u8 FI_ALNK1_DAT1;
    __RW __u8 FI_ALNK1_DAT2;
    __RW __u8 FI_ALNK1_DAT3;
    __RW __u8 FI_SPDIF_DIA;
    __RW __u8 FI_SPDIF_DIB;
    __RW __u8 FI_SPDIF_DIC;
    __RW __u8 FI_SPDIF_DID;
    __RW __u8 FI_PLNK_DAT0;
    __RW __u8 FI_PLNK_DAT1;
    __RW __u8 FI_MCPWM0_CK;
    __RW __u8 FI_MCPWM1_CK;
    __RW __u8 FI_MCPWM2_CK;
    __RW __u8 FI_MCPWM3_CK;
    __RW __u8 FI_MCPWM4_CK;
    __RW __u8 FI_MCPWM5_CK;
    __RW __u8 FI_MCPWM0_FP;
    __RW __u8 FI_MCPWM1_FP;
    __RW __u8 FI_MCPWM2_FP;
    __RW __u8 FI_MCPWM3_FP;
    __RW __u8 FI_MCPWM4_FP;
    __RW __u8 FI_MCPWM5_FP;
    __RW __u8 FI_CHAIN_IN0;
    __RW __u8 FI_CHAIN_IN1;
    __RW __u8 FI_CHAIN_IN2;
    __RW __u8 FI_CHAIN_IN3;
    __RW __u8 FI_CHAIN_RST;
    __RW __u8 FI_TOTAL;
} JL_IMAP_TypeDef;

#define JL_IMAP_BASE      (ls_base + map_adr(0x5c, 0x00))
#define JL_IMAP           ((JL_IMAP_TypeDef   *)JL_IMAP_BASE)
