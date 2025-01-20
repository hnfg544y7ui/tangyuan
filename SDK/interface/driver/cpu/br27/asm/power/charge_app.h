#ifndef __CHARGE_APP_H__
#define __CHARGE_APP_H__

/************************P3_CHG_CON0*****************************/
#define CHARGE_EN(en)           	p33_fast_access(P3_CHG_CON0, BIT(0), en)

#define CHGGO_EN(en)            	p33_fast_access(P3_CHG_CON0, BIT(1), en)

#define IS_CHARGE_EN()				((P33_CON_GET(P3_CHG_CON0) & BIT(0)) ? 1: 0 )

#define CHG_HV_MODE(mode)       	p33_fast_access(P3_CHG_CON0, BIT(2), mode)

#define CHG_TRICKLE_EN(en)          p33_fast_access(P3_CHG_CON0, BIT(3), en)

#define CHG_CCLOOP_EN(en)           p33_fast_access(P3_CHG_CON0, BIT(4), en)

#define CHG_VILOOP_EN(en)           p33_fast_access(P3_CHG_CON0, BIT(5), en)

#define CHG_VINLOOP_SLT(sel)        p33_fast_access(P3_CHG_CON0, BIT(6), sel)

#define CHG_SEL_CHG_FULL            0
#define CHG_SEL_VBAT_DET            1
#define CHG_SSEL(sel)               p33_fast_access(P3_CHG_CON0, BIT(7), sel)

/************************P3_CHG_CON1*****************************/
#define CHARGE_FULL_V_SEL(a)		P33_CON_SET(P3_CHG_CON1, 0, 4, a)

#define CHARGE_mA_SEL(a)			P33_CON_SET(P3_CHG_CON1, 4, 4, a)

/************************P3_CHG_CON2*****************************/
#define CHARGE_FULL_mA_SEL(a)		P33_CON_SET(P3_CHG_CON2, 4, 2, a)

enum {
    CHARGE_DET_VOL_365V,
    CHARGE_DET_VOL_375V,
    CHARGE_DET_VOL_385V,
    CHARGE_DET_VOL_395V,
};
#define CHARGE_DET_VOL(a)			P33_CON_SET(P3_CHG_CON2, 1, 2, a)

#define CHARGE_DET_EN(en)			p33_fast_access(P3_CHG_CON2, BIT(0), en)

/************************P3_CHG_CON3*****************************/
#define CHARGE_CURR_TRIM(a)         P33_CON_SET(P3_CHG_CON3, 0, 4, a)

/************************P3_L5V_CON0*****************************/
#define L5V_IO_MODE(a)              p33_fast_access(P3_L5V_CON0, BIT(2), a)

#define IS_L5V_LOAD_EN()        	((P33_CON_GET(P3_L5V_CON0) & BIT(0)) ? 1: 0)

#define L5V_LOAD_EN(a)		    	p33_fast_access(P3_L5V_CON0, BIT(0), a)

/************************P3_L5V_CON1*****************************/
#define L5V_RES_DET_S_SEL(a)		P33_CON_SET(P3_L5V_CON1, 0, 2, a)

#define GET_L5V_RES_DET_S_SEL() 	(P33_CON_GET(P3_L5V_CON1) & 0x03)

/************************P3_CHG_WKUP*****************************/
#define CHARGE_LEVEL_DETECT_EN(a)	p33_fast_access(P3_CHG_WKUP, BIT(0), a)

#define CHARGE_EDGE_DETECT_EN(a)	p33_fast_access(P3_CHG_WKUP, BIT(1), a)

#define CHARGE_WKUP_SOURCE_SEL(a)	P33_CON_SET(P3_CHG_WKUP, 2, 2, a)

#define CHARGE_WKUP_EN(a)			p33_fast_access(P3_CHG_WKUP, BIT(4), a)

#define CHARGE_WKUP_EDGE_SEL(a)		p33_fast_access(P3_CHG_WKUP, BIT(5), a)

#define CHARGE_WKUP_PND_CLR()		p33_fast_access(P3_CHG_WKUP, BIT(6), 1)

/************************P3_AWKUP_LEVEL*****************************/
#define CHARGE_FULL_FILTER_GET()	((P33_CON_GET(P3_AWKUP_LEVEL) & BIT(2)) ? 1: 0)

#define LVCMP_DET_FILTER_GET()      ((P33_CON_GET(P3_AWKUP_LEVEL) & BIT(1)) ? 1: 0)

#define LDO5V_DET_FILTER_GET()      ((P33_CON_GET(P3_AWKUP_LEVEL) & BIT(0)) ? 1: 0)

/************************P3_ANA_READ*****************************/
#define CHARGE_FULL_FLAG_GET()		((P33_CON_GET(P3_ANA_READ) & BIT(0)) ? 1: 0 )

#define LVCMP_DET_GET()			    ((P33_CON_GET(P3_ANA_READ) & BIT(1)) ? 1: 0 )

#define LDO5V_DET_GET()			    ((P33_CON_GET(P3_ANA_READ) & BIT(2)) ? 1: 0 )



#endif
