/*********************************************************************************************
 *   Filename        : p33.h

 *   Description     :

 *   Author          : Bingquan

 *   Email           : caibingquan@zh-jieli.com

 *   Last modifiled  : 2019-12-09 10:42

 *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
 *********************************************************************************************/
#ifndef __P33_H__
#define __P33_H__

#include "p33_sfr.h"

//
//
//					for p33 access
//
//
//
/**************************************************************/

//ROM
u8 p33_buf(u8 buf);

#define p33_xor_1byte(addr, data0)      (*((volatile u8 *)&addr + 0x300*4)  = data0)
// #define p33_xor_1byte(addr, data0)      addr ^= (data0)

#define p33_or_1byte(addr, data0)       (*((volatile u8 *)&addr + 0x200*4)  = data0)
// #define p33_or_1byte(addr, data0)       addr |= (data0)

#define p33_and_1byte(addr, data0)      (*((volatile u8 *)&addr + 0x100*4)  = (data0))
//#define p33_and_1byte(addr, data0)      addr &= (data0)

// void p33_tx_1byte(u16 addr, u8 data0);
#define p33_tx_1byte(addr, data0)       addr = data0

// u8 p33_rx_1byte(u16 addr);
#define p33_rx_1byte(addr)              addr

#define P33_CON_SET(sfr, start, len, data)  (sfr = (sfr & ~((~(0xff << (len))) << (start))) | \
	 (((data) & (~(0xff << (len)))) << (start)))

#define P33_CON_GET(sfr)    (sfr)

#if 1

#define p33_fast_access(reg, data, en)           \
{ 												 \
    if (en) {                                    \
		p33_or_1byte(reg, (data));               \
    } else {                                     \
		p33_and_1byte(reg, (u8)~(data));             \
    }                                            \
}

#else

#define p33_fast_access(reg, data, en)         \
{                                              \
	if (en) {                                  \
       	reg |= (data);                         \
	} else {                                   \
		reg &= ~(data);                        \
    }                                          \
}

#endif



#include "charge_app.h"

#include "rtc_app.h"

#include "rtc/rtc_dev.h"

#endif
