#ifndef __IOVDD_ADAPTIVE_H__
#define __IOVDD_ADAPTIVE_H__

#include "generic/typedef.h"



void check_set_miovdd_level_change(void);//检测iovdd的挡位变化，如果有变化则设置miovdd的挡位,并且更新vbg的ad值

void miovdd_adaptive_adjustment(void);//miovd挡位自适应调节接口


#endif

