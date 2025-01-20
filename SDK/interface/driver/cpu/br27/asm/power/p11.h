/*********************************************************************************************
    *   Filename        : p11.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : caibingquan@zh-jieli.com

    *   Last modifiled  : 2019-12-09 10:21

    *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __P11_H__
#define __P11_H__

#include "p11_csfr.h"
#include "p11_sfr.h"

/*

 _______________ <-----P11 Message Acess End
| poweroff boot |
|_______________|
| m2p msg(0x40) |
|_______________|
| p2m msg(0x40) |
|_______________|<-----P11 Message Acess Begin
|               |
|    p11 use    |
|_______________|__

*/
#define P11_RAM_BASE 				0xF20000
#define P11_RAM_SIZE 				(0x6000)
#define P11_RAM_END 				(P11_RAM_BASE + P11_RAM_SIZE)

#define P11_POWEROFF_RAM_SIZE 		(0x14 + 0xc)
#define P11_POWEROFF_RAM_BEGIN  	(P11_RAM_END - P11_POWEROFF_RAM_SIZE)

#define P2M_MESSAGE_SIZE 			0x40
#define M2P_MESSAGE_SIZE 			0xe0

#define M2P_MESSAGE_RAM_BEGIN 		(P11_POWEROFF_RAM_BEGIN - M2P_MESSAGE_SIZE)
#define P2M_MESSAGE_RAM_BEGIN 		(M2P_MESSAGE_RAM_BEGIN - P2M_MESSAGE_SIZE)

#define P11_MESSAGE_RAM_BEGIN 		(P2M_MESSAGE_RAM_BEGIN)

#define P11_RAM_ACCESS(x)   		(*(volatile u8 *)(x))

#define P2M_MESSAGE_ACCESS(x)      	P11_RAM_ACCESS(P2M_MESSAGE_RAM_BEGIN + x)
#define M2P_MESSAGE_ACCESS(x)      	P11_RAM_ACCESS(M2P_MESSAGE_RAM_BEGIN + x)

#define P11_RAM_PROTECT_END 		(P11_MESSAGE_RAM_BEGIN)


#endif

