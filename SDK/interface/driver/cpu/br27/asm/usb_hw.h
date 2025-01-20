#ifndef _USB_HW_H_
#define _USB_HW_H_
#include "typedef.h"
#include "generic/ioctl.h"

//使用usb2.0 ip的芯片需要定义
#define USB_HW_20

#define USB_MAX_HW_EPNUM        5



//HUSB_SIE_CON
#define HUSB_SYS_NRST               0
#define HUSB_SIE_EN                 1
#define HUSB_TM1                    2
#define HUSB_SIE_NRST               4
#define HUSB_SIE_PND                5
#define HUSB_CLR_SOF_PND            6
#define HUSB_SOF_PND                7
#define HUSB_SOF_IE                 10
#define HUSB_RD_BLOCK               11
#define HUSB_INT_MODE               12
#define HUSB_CLR_INTR_USB           13
#define HUSB_CLR_INTR_TX            14
#define HUSB_CLR_INTR_RX            15
#define HUSB_CPU_IDDIG              16
#define HUSB_CPU_AVALID             17
#define HUSB_CPU_SESSEND            18
#define HUSB_CPU_VBUSVALID          19

//HUSB_PHY0_CON2
#define HUSB_TRAN_EN                0
#define HUSB_TX_BIAS                1
#define HUSB_RX_BIAS                2
#define HUSB_HS_PREEMP_EN           3
#define HUSB_PD_CHK_EN              4
#define HUSB_PD_CHK_DM              5
#define HUSB_PD_CHK_DP              6
#define HUSB_TX_HSFS_RTUNE_EN       14
#define HUSB_TX_LS_EN               15
#define HUSB_RX_TOE                 16

//HUSB_PHY0_CON3
#define HUSB_CLK_EN                 0
#define HUSB_UTM_EN                 1
#define HUSB_HD_HS_EN               2
#define HUSB_HD_HS_DAT              3
#define HUSB_HD_HS_TOG              4
#define HUSB_CMP_EN                 5
#define HUSB_CMP_CLK                6
#define HUSB_CMP_RTUNE              7
#define HUSB_CMP_DP                 8
#define HUSB_CMP_DM                 9
#define HUSB_CMP_ANA                10

//HUSB_PHY0_READ
#define HUSB_RD_CHK_DPO             0
#define HUSB_RD_CHK_DMO             1
#define HUSB_RD_DP_SE               4
#define HUSB_RD_DM_SE               5



enum {
    USB0,
};
#define     USB_MAX_HW_NUM  1



#endif
