#ifndef _USB_PLL_TRIM_HW_H_
#define _USB_PLL_TRIM_HW_H_

#include "typedef.h"

#define USB_PLL_TRIM_EN   1

#define USB_PLL_TRIM_CLK    60

#define USB_TRIM_CON0       HUSB_TRIM_CON0
#define USB_TRIM_CON1       HUSB_TRIM_CON1
#define USB_TRIM_PND        HUSB_TRIM_PND
#define USB_FRQ_CNT         HUSB_FRQ_CNT
#define USB_FRQ_SCA         HUSB_FRQ_SCA
#define USB_PLL_CON0        HUSB_PLL_CON0
#define USB_PLL_CON1        HUSB_PLL_CON1
#define USB_PLL_NR          HUSB_PLL_NR
#define USB_PLL_CKSYN_CORE_ENABLE()     (USB_PLL_CON1 |= BIT(19))
#define USB_PLL_CKSYN_CORE_DISABLE()    (USB_PLL_CON1 &= ~BIT(19))

#endif


