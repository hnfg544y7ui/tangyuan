#ifndef POWER_INTERFACE_H
#define POWER_INTERFACE_H

#include "gpio.h"

#include "generic/typedef.h"

//电源、低功耗
#include "asm/power/power_api.h"

//唤醒
#include "power/power_wakeup.h"

//复位
#include "power/power_reset.h"

//app
#include "power/power_app.h"

//校准
#include "asm/power/power_trim.h"

//p33寄存器
#include "power/p33/p33_sfr.h"
#include "power/p33/p33_access.h"
#include "power/p33/charge_hw.h"
#include "power/p33/p33_api.h"
#include "power/p33/rtc_hw.h"

//P11寄存器
#include "asm/power/p11.h"

//gpio相关
#include "asm/power/power_port.h"

//p11通信
#include "asm/power/lp_ipc.h"

#include "asm/power/iovdd_adaptive.h"

//兼容旧版本接口
#include "asm/power/power_compat.h"

#include "power/wdt.h"

#endif
