/**@file  		power_port.h
* @brief
* @details
* @author
* @date     	2021-10-13
* @version    	V1.0
* @copyright  	Copyright(c)2010-2021  JIELI
 */
#ifndef __POWER_PORT_H__
#define __POWER_PORT_H__

enum {
    PORTA_GROUP = 0,
    PORTB_GROUP,
    PORTC_GROUP,
    PORTD_GROUP,
    PORTP_GROUP,
};

struct gpio_value {
    u16 gpioa;
    u16 gpiob;
    u16 gpioc;
    u16 gpiod;
    u16 gpiop;
    u16 gpiousb;
};

#define GET_SFC_PORT()		((JL_IOMAP->CON0 >> 16) & 0x1)

#define READ_NORMAL_MODE                        0b0000  //CMD 1bit, ADDR 1bit clk < 30MHz
#define FAST_READ_NORMAL_MODE                   0b0001  //CMD 1bit, ADDR 1bit
#define FAST_READ_DUAL_IO_NORMAL_READ_MODE      0b0100  //CMD 1bit, ADDR 2bit
#define FAST_READ_DUAL_IO_CONTINUOUS_READ_MODE  0b0110  // no cmd addr 2bit
#define FAST_READ_DUAL_OUTPUT_MODE              0b0010  //cmd 1bit addr 1bit
#define FAST_READ_QUAD_OUTPUT_MODE              0b0011  //cmd 1bit addr 1bit
#define FAST_READ_QUAD_IO_NORMAL_READ_MODE      0b0101  //cmd 1bit addr 4bit
#define FAST_READ_QUAD_IO_CONTINUOUS_READ_MODE  0b0111  //no cmd addr 4bit

#define GET_SFC_MODE()		((JL_SFC->CON >> 8) & 0xf)

#define     _PORT(p)            JL_PORT##p
#define     _PORT_IN(p,b)       P##p##b##_IN
#define     _PORT_OUT(p,b)      JL_OMAP->P##p##b##_OUT

#define     SPI_PORT(p)         _PORT(p)
#define     SPI0_FUNC_OUT(p,b)  _PORT_OUT(p,b)
#define     SPI0_FUNC_IN(p,b)   _PORT_IN(p,b)

// | func\port |  A   |  B   |
// |-----------|------|------|
// | CS        | PD3  | PB14 |
// | CLK       | PD0  | PD0  |
// | DO(D0)    | PD1  | PD1  |
// | DI(D1)    | PD2  | PB13 |
// | WP(D2)    | PD5  | PB12 |
// | HOLD(D3)  | PD6  | PD6  |


#define     FLASH_PORTAB_VCC    D
#define     PORTAB_VCC          4

////////////////////////////////////////////////////////////////////////////////
#define     PORT_SPI0_CSA       D
#define     SPI0_CSA            3

#define     PORT_SPI0_CLKA      D
#define     SPI0_CLKA           0

#define     PORT_SPI0_DOA       D
#define     SPI0_DOA            1

#define     PORT_SPI0_DIA       D
#define     SPI0_DIA            2

#define     PORT_SPI0_D2A       D
#define     SPI0_D2A            5

#define     PORT_SPI0_D3A       D
#define     SPI0_D3A            6

////////////////////////////////////////////////////////////////////////////////
#define     PORT_SPI0_CSB       B
#define     SPI0_CSB            14

#define     PORT_SPI0_CLKB      D
#define     SPI0_CLKB           0

#define     PORT_SPI0_DOB       D
#define     SPI0_DOB            1

#define     PORT_SPI0_DIB       B
#define     SPI0_DIB            13

#define     PORT_SPI0_D2B       B
#define     SPI0_D2B            12

#define     PORT_SPI0_D3B       D
#define     SPI0_D3B            6

////////////////////////////////////////////////////////////////////////////////
#define		SPI0_PWR_A		IO_PORTD_04
#define		SPI0_CS_A		IO_PORTD_03
#define 	SPI0_CLK_A		IO_PORTD_00
#define 	SPI0_DO_D0_A	IO_PORTD_01
#define 	SPI0_DI_D1_A	IO_PORTD_02
#define 	SPI0_WP_D2_A	IO_PORTD_05
#define 	SPI0_HOLD_D3_A	IO_PORTD_06

////////////////////////////////////////////////////////////////////////////////

#define		SPI0_PWR_B		IO_PORTD_04
#define		SPI0_CS_B		IO_PORTB_14
#define 	SPI0_CLK_B		IO_PORTD_00
#define 	SPI0_DO_D0_B	IO_PORTD_01
#define 	SPI0_DI_D1_B	IO_PORTB_13
#define 	SPI0_WP_D2_B	IO_PORTB_12
#define 	SPI0_HOLD_D3_B	IO_PORTD_06

////////////////////////////////////////////////////////////////////////

#define		PSRAM_D0A		IO_PORTC_00
#define		PSRAM_D1A		IO_PORTC_02
#define		PSRAM_D2A		IO_PORTC_01
#define		PSRAM_D3A		IO_PORTA_12


u8 get_sfc_bit_mode();
void port_protect(u16 *port_group, u32 port_num);

void port_init(void);
void port_hd_init(u32 hd_lev);

void gpio_close(u32 port, u32 val);

#endif

