#ifndef __LP_IPC_H__
#define __LP_IPC_H__

//===========================================================================//
//                              P2M MESSAGE TABLE                            //
//===========================================================================//
#define P2M_WKUP_SRC                                    P2M_MESSAGE_ACCESS(0x000)
#define P2M_WKUP_PND0                                   P2M_MESSAGE_ACCESS(0x001)
#define P2M_WKUP_PND1                                   P2M_MESSAGE_ACCESS(0x002)
#define P2M_REPLY_SYNC_CMD                            	P2M_MESSAGE_ACCESS(0x003)
#define P2M_MESSAGE_VAD_CMD                             P2M_MESSAGE_ACCESS(0x004)
#define P2M_MESSAGE_VAD_CBUF_WPTR                       P2M_MESSAGE_ACCESS(0x005)
#define P2M_MESSAGE_BANK_ADR_L                          P2M_MESSAGE_ACCESS(0x006)
#define P2M_MESSAGE_BANK_ADR_H                          P2M_MESSAGE_ACCESS(0x007)
#define P2M_MESSAGE_BANK_INDEX                          P2M_MESSAGE_ACCESS(0x008)
#define P2M_MESSAGE_BANK_ACK                            P2M_MESSAGE_ACCESS(0x009)
#define P2M_P11_HEAP_BEGIN_ADDR_L    					P2M_MESSAGE_ACCESS(0x00A)
#define P2M_P11_HEAP_BEGIN_ADDR_H    					P2M_MESSAGE_ACCESS(0x00B)
#define P2M_P11_HEAP_SIZE_L    							P2M_MESSAGE_ACCESS(0x00C)
#define P2M_P11_HEAP_SIZE_H    							P2M_MESSAGE_ACCESS(0x00D)

#define P2M_CTMU_KEY_EVENT                              P2M_MESSAGE_ACCESS(0x010)
#define P2M_CTMU_KEY_CNT                                P2M_MESSAGE_ACCESS(0x011)
#define P2M_CTMU_WKUP_MSG                               P2M_MESSAGE_ACCESS(0x012)
#define P2M_CTMU_EARTCH_EVENT                           P2M_MESSAGE_ACCESS(0x013)

#define P2M_MASSAGE_CTMU_CH0_L_RES                                         0x014
#define P2M_MASSAGE_CTMU_CH0_H_RES                                         0x015
#define P2M_CTMU_CH0_L_RES                              P2M_MESSAGE_ACCESS(0x014)
#define P2M_CTMU_CH0_H_RES                              P2M_MESSAGE_ACCESS(0x015)
#define P2M_CTMU_CH1_L_RES                              P2M_MESSAGE_ACCESS(0x016)
#define P2M_CTMU_CH1_H_RES                              P2M_MESSAGE_ACCESS(0x017)
#define P2M_CTMU_CH2_L_RES                              P2M_MESSAGE_ACCESS(0x018)
#define P2M_CTMU_CH2_H_RES                              P2M_MESSAGE_ACCESS(0x019)
#define P2M_CTMU_CH3_L_RES                              P2M_MESSAGE_ACCESS(0x01a)
#define P2M_CTMU_CH3_H_RES                              P2M_MESSAGE_ACCESS(0x01b)
#define P2M_CTMU_CH4_L_RES                              P2M_MESSAGE_ACCESS(0x01c)
#define P2M_CTMU_CH4_H_RES                              P2M_MESSAGE_ACCESS(0x01d)
#define P2M_CTMU_CH5_L_RES                              P2M_MESSAGE_ACCESS(0x01e)
#define P2M_CTMU_CH5_H_RES                              P2M_MESSAGE_ACCESS(0x01f)
#define P2M_CTMU_CH6_L_RES                              P2M_MESSAGE_ACCESS(0x020)
#define P2M_CTMU_CH6_H_RES                              P2M_MESSAGE_ACCESS(0x021)
#define P2M_CTMU_CH7_L_RES                              P2M_MESSAGE_ACCESS(0x022)
#define P2M_CTMU_CH7_H_RES                              P2M_MESSAGE_ACCESS(0x023)
#define P2M_CTMU_CH8_L_RES                              P2M_MESSAGE_ACCESS(0x024)
#define P2M_CTMU_CH8_H_RES                              P2M_MESSAGE_ACCESS(0x025)
#define P2M_CTMU_CH9_L_RES                              P2M_MESSAGE_ACCESS(0x026)
#define P2M_CTMU_CH9_H_RES                              P2M_MESSAGE_ACCESS(0x027)

#define P2M_CTMU_EARTCH_L_IIR_VALUE                     P2M_MESSAGE_ACCESS(0x028)
#define P2M_CTMU_EARTCH_H_IIR_VALUE                     P2M_MESSAGE_ACCESS(0x029)
#define P2M_CTMU_EARTCH_L_TRIM_VALUE                    P2M_MESSAGE_ACCESS(0x02a)
#define P2M_CTMU_EARTCH_H_TRIM_VALUE                    P2M_MESSAGE_ACCESS(0x02b)
#define P2M_CTMU_EARTCH_L_DIFF_VALUE                    P2M_MESSAGE_ACCESS(0x02c)
#define P2M_CTMU_EARTCH_H_DIFF_VALUE                    P2M_MESSAGE_ACCESS(0x02d)

#define P2M_AWKUP_P_PND                                 P2M_MESSAGE_ACCESS(0x02e)
#define P2M_AWKUP_N_PND                                 P2M_MESSAGE_ACCESS(0x02f)
#define P2M_WKUP_RTC                                    P2M_MESSAGE_ACCESS(0x030)

#define P2M_NFC_REPLY_CMD								P2M_MESSAGE_ACCESS(0x031)
#define P2M_NFC_TAG_EVENT	    						P2M_MESSAGE_ACCESS(0x032)
#define P2M_NFC_BUF_ADDRL   							P2M_MESSAGE_ACCESS(0x033)
#define P2M_NFC_BUF_ADDRH	    						P2M_MESSAGE_ACCESS(0x034)

#define P2M_CBUF_ADDR0									P2M_MESSAGE_ACCESS(0x035)
#define P2M_CBUF_ADDR1                                  P2M_MESSAGE_ACCESS(0x036)
#define P2M_CBUF_ADDR2                                  P2M_MESSAGE_ACCESS(0x037)
#define P2M_CBUF_ADDR3                                  P2M_MESSAGE_ACCESS(0x038)

#define P2M_CBUF1_ADDR0                                 P2M_MESSAGE_ACCESS(0x039)
#define P2M_CBUF1_ADDR1                                 P2M_MESSAGE_ACCESS(0x03a)
#define P2M_CBUF1_ADDR2                                 P2M_MESSAGE_ACCESS(0x03b)
#define P2M_CBUF1_ADDR3                                 P2M_MESSAGE_ACCESS(0x03c)





//===========================================================================//
//                              M2P MESSAGE TABLE                            //
//===========================================================================//
#define M2P_LRC_PRD                                     M2P_MESSAGE_ACCESS(0x000)
#define M2P_WDVDD                                		M2P_MESSAGE_ACCESS(0x001)
#define M2P_LRC_TMR_50us                                M2P_MESSAGE_ACCESS(0x002)
#define M2P_LRC_TMR_200us                               M2P_MESSAGE_ACCESS(0x003)
#define M2P_LRC_TMR_600us                               M2P_MESSAGE_ACCESS(0x004)
#define M2P_VDDIO_KEEP                                  M2P_MESSAGE_ACCESS(0x005)
#define M2P_LRC_KEEP                                    M2P_MESSAGE_ACCESS(0x006)
#define M2P_SYNC_CMD                                  	M2P_MESSAGE_ACCESS(0x007)
#define M2P_MESSAGE_VAD_CMD                             M2P_MESSAGE_ACCESS(0x008)
#define M2P_MESSAGE_VAD_CBUF_RPTR                       M2P_MESSAGE_ACCESS(0x009)
#define M2P_RCH_FEQ_L                                   M2P_MESSAGE_ACCESS(0x00a)
#define M2P_RCH_FEQ_H                                   M2P_MESSAGE_ACCESS(0x00b)
#define M2P_MEM_CONTROL                                 M2P_MESSAGE_ACCESS(0x00c)
#define M2P_BTOSC_KEEP                                  M2P_MESSAGE_ACCESS(0x00d)
#define M2P_CTMU_KEEP									M2P_MESSAGE_ACCESS(0x00e)
#define M2P_NFC_KEEP									M2P_MESSAGE_ACCESS(0x00f)
#define M2P_RTC_KEEP                                    M2P_MESSAGE_ACCESS(0x010)
#define M2P_WDT_SYNC                                   	M2P_MESSAGE_ACCESS(0x011)
#define M2P_KEEP_NVDD									M2P_MESSAGE_ACCESS(0x012)

/*触摸所有通道配置*/
#define M2P_CTMU_CMD  									M2P_MESSAGE_ACCESS(0x18)
#define M2P_CTMU_MSG                                    M2P_MESSAGE_ACCESS(0x19)
#define M2P_CTMU_PRD0               		            M2P_MESSAGE_ACCESS(0x1a)
#define M2P_CTMU_PRD1                                   M2P_MESSAGE_ACCESS(0x1b)
#define M2P_CTMU_CH_ENABLE_L							M2P_MESSAGE_ACCESS(0x1c)
#define M2P_CTMU_CH_ENABLE_H							M2P_MESSAGE_ACCESS(0x1d)
#define M2P_CTMU_CH_DEBUG_L								M2P_MESSAGE_ACCESS(0x1e)
#define M2P_CTMU_CH_DEBUG_H								M2P_MESSAGE_ACCESS(0x1f)
#define M2P_CTMU_CH_WAKEUP_EN_L					        M2P_MESSAGE_ACCESS(0x20)
#define M2P_CTMU_CH_WAKEUP_EN_H					        M2P_MESSAGE_ACCESS(0x21)
#define M2P_CTMU_CH_CFG						            M2P_MESSAGE_ACCESS(0x22)
#define M2P_CTMU_EARTCH_CH						        M2P_MESSAGE_ACCESS(0x23)
#define M2P_CTMU_TIME_BASE          					M2P_MESSAGE_ACCESS(0x24)

#define M2P_CTMU_LONG_TIMEL                             M2P_MESSAGE_ACCESS(0x25)
#define M2P_CTMU_LONG_TIMEH                             M2P_MESSAGE_ACCESS(0x26)
#define M2P_CTMU_HOLD_TIMEL                             M2P_MESSAGE_ACCESS(0x27)
#define M2P_CTMU_HOLD_TIMEH                             M2P_MESSAGE_ACCESS(0x28)
#define M2P_CTMU_SOFTOFF_LONG_TIMEL                     M2P_MESSAGE_ACCESS(0x29)
#define M2P_CTMU_SOFTOFF_LONG_TIMEH                     M2P_MESSAGE_ACCESS(0x2a)
#define M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_L  		M2P_MESSAGE_ACCESS(0x2b)//长按复位
#define M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_H  		M2P_MESSAGE_ACCESS(0x2c)//长按复位

#define M2P_CTMU_INEAR_VALUE_L                          M2P_MESSAGE_ACCESS(0x2d)
#define M2P_CTMU_INEAR_VALUE_H							M2P_MESSAGE_ACCESS(0x2e)
#define M2P_CTMU_OUTEAR_VALUE_L                         M2P_MESSAGE_ACCESS(0x2f)
#define M2P_CTMU_OUTEAR_VALUE_H                         M2P_MESSAGE_ACCESS(0x30)
#define M2P_CTMU_EARTCH_TRIM_VALUE_L                    M2P_MESSAGE_ACCESS(0x31)
#define M2P_CTMU_EARTCH_TRIM_VALUE_H                    M2P_MESSAGE_ACCESS(0x32)

#define M2P_MASSAGE_CTMU_CH0_CFG0L                                         0x33
#define M2P_MASSAGE_CTMU_CH0_CFG0H                                         0x34
#define M2P_MASSAGE_CTMU_CH0_CFG1L                                         0x35
#define M2P_MASSAGE_CTMU_CH0_CFG1H                                         0x36
#define M2P_MASSAGE_CTMU_CH0_CFG2L                                         0x37
#define M2P_MASSAGE_CTMU_CH0_CFG2H                                         0x38

#define M2P_CTMU_CH0_CFG0L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0L + 0 * 6))
#define M2P_CTMU_CH0_CFG0H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0H + 0 * 6))
#define M2P_CTMU_CH0_CFG1L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1L + 0 * 6))
#define M2P_CTMU_CH0_CFG1H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1H + 0 * 6))
#define M2P_CTMU_CH0_CFG2L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2L + 0 * 6))
#define M2P_CTMU_CH0_CFG2H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2H + 0 * 6))

#define M2P_CTMU_CH1_CFG0L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0L + 1 * 6))
#define M2P_CTMU_CH1_CFG0H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0H + 1 * 6))
#define M2P_CTMU_CH1_CFG1L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1L + 1 * 6))
#define M2P_CTMU_CH1_CFG1H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1H + 1 * 6))
#define M2P_CTMU_CH1_CFG2L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2L + 1 * 6))
#define M2P_CTMU_CH1_CFG2H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2H + 1 * 6))

#define M2P_CTMU_CH2_CFG0L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0L + 2 * 6))
#define M2P_CTMU_CH2_CFG0H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0H + 2 * 6))
#define M2P_CTMU_CH2_CFG1L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1L + 2 * 6))
#define M2P_CTMU_CH2_CFG1H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1H + 2 * 6))
#define M2P_CTMU_CH2_CFG2L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2L + 2 * 6))
#define M2P_CTMU_CH2_CFG2H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2H + 2 * 6))

#define M2P_CTMU_CH3_CFG0L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0L + 3 * 6))
#define M2P_CTMU_CH3_CFG0H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0H + 3 * 6))
#define M2P_CTMU_CH3_CFG1L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1L + 3 * 6))
#define M2P_CTMU_CH3_CFG1H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1H + 3 * 6))
#define M2P_CTMU_CH3_CFG2L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2L + 3 * 6))
#define M2P_CTMU_CH3_CFG2H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2H + 3 * 6))

#define M2P_CTMU_CH4_CFG0L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0L + 4 * 6))
#define M2P_CTMU_CH4_CFG0H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0H + 4 * 6))
#define M2P_CTMU_CH4_CFG1L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1L + 4 * 6))
#define M2P_CTMU_CH4_CFG1H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1H + 4 * 6))
#define M2P_CTMU_CH4_CFG2L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2L + 4 * 6))
#define M2P_CTMU_CH4_CFG2H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2H + 4 * 6))

#define M2P_CTMU_CH5_CFG0L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0L + 5 * 6))
#define M2P_CTMU_CH5_CFG0H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0H + 5 * 6))
#define M2P_CTMU_CH5_CFG1L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1L + 5 * 6))
#define M2P_CTMU_CH5_CFG1H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1H + 5 * 6))
#define M2P_CTMU_CH5_CFG2L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2L + 5 * 6))
#define M2P_CTMU_CH5_CFG2H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2H + 5 * 6))

#define M2P_CTMU_CH6_CFG0L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0L + 6 * 6))
#define M2P_CTMU_CH6_CFG0H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0H + 6 * 6))
#define M2P_CTMU_CH6_CFG1L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1L + 6 * 6))
#define M2P_CTMU_CH6_CFG1H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1H + 6 * 6))
#define M2P_CTMU_CH6_CFG2L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2L + 6 * 6))
#define M2P_CTMU_CH6_CFG2H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2H + 6 * 6))

#define M2P_CTMU_CH7_CFG0L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0L + 7 * 6))
#define M2P_CTMU_CH7_CFG0H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0H + 7 * 6))
#define M2P_CTMU_CH7_CFG1L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1L + 7 * 6))
#define M2P_CTMU_CH7_CFG1H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1H + 7 * 6))
#define M2P_CTMU_CH7_CFG2L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2L + 7 * 6))
#define M2P_CTMU_CH7_CFG2H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2H + 7 * 6))

#define M2P_CTMU_CH8_CFG0L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0L + 8 * 6))
#define M2P_CTMU_CH8_CFG0H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0H + 8 * 6))
#define M2P_CTMU_CH8_CFG1L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1L + 8 * 6))
#define M2P_CTMU_CH8_CFG1H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1H + 8 * 6))
#define M2P_CTMU_CH8_CFG2L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2L + 8 * 6))
#define M2P_CTMU_CH8_CFG2H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2H + 8 * 6))

#define M2P_CTMU_CH9_CFG0L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0L + 9 * 6))
#define M2P_CTMU_CH9_CFG0H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG0H + 9 * 6))
#define M2P_CTMU_CH9_CFG1L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1L + 9 * 6))
#define M2P_CTMU_CH9_CFG1H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG1H + 9 * 6))
#define M2P_CTMU_CH9_CFG2L                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2L + 9 * 6))
#define M2P_CTMU_CH9_CFG2H                              M2P_MESSAGE_ACCESS((M2P_MASSAGE_CTMU_CH0_CFG2H + 9 * 6))//0x6e


/*NFC相关配置*/
#define M2P_NFC_CMD										M2P_MESSAGE_ACCESS(0x6f)

#define M2P_NFC_TAG_TYPE								M2P_MESSAGE_ACCESS(0x70)

#define M2P_NFC_UID0									M2P_MESSAGE_ACCESS(0x71)
#define M2P_NFC_UID1									M2P_MESSAGE_ACCESS(0x72)
#define M2P_NFC_UID2									M2P_MESSAGE_ACCESS(0x73)
#define M2P_NFC_UID3									M2P_MESSAGE_ACCESS(0x74)
#define M2P_NFC_UID4									M2P_MESSAGE_ACCESS(0x75)
#define M2P_NFC_UID5									M2P_MESSAGE_ACCESS(0x76)
#define M2P_NFC_UID6									M2P_MESSAGE_ACCESS(0x77)

#define M2P_NFC_BT_MAC0									M2P_MESSAGE_ACCESS(0x78)
#define M2P_NFC_BT_MAC1									M2P_MESSAGE_ACCESS(0x79)
#define M2P_NFC_BT_MAC2									M2P_MESSAGE_ACCESS(0x7a)
#define M2P_NFC_BT_MAC3									M2P_MESSAGE_ACCESS(0x7b)
#define M2P_NFC_BT_MAC4									M2P_MESSAGE_ACCESS(0x7c)
#define M2P_NFC_BT_MAC5									M2P_MESSAGE_ACCESS(0x7d)

#define M2P_NFC_BT_NAME_BEGIN							M2P_MESSAGE_ACCESS(0x7e)//蓝牙名。占32byte
#define M2P_NFC_BT_NAME_END								M2P_MESSAGE_ACCESS(0x9d)

#define M2P_NFC_GAIN_CFG								M2P_MESSAGE_ACCESS(0x9e)
#define M2P_NFC_CUR_TRIM_CFG0							M2P_MESSAGE_ACCESS(0x9f)
#define M2P_NFC_CUR_TRIM_CFG1							M2P_MESSAGE_ACCESS(0xa0)
#define M2P_NFC_RX_IO_MODE								M2P_MESSAGE_ACCESS(0xa1)
#define M2P_NFC_OP_MODE									M2P_MESSAGE_ACCESS(0xa2)
#define M2P_NFC_BF_CFG									M2P_MESSAGE_ACCESS(0xa3)
#define M2P_NFC_PAUSE_CFG								M2P_MESSAGE_ACCESS(0xa4)
#define M2P_NFC_IDLE_TIMEOUT_TIME						M2P_MESSAGE_ACCESS(0xa5)
#define M2P_NFC_CTL_WKUP_CYC_SEL						M2P_MESSAGE_ACCESS(0xa6)
#define M2P_NFC_CTL_INVALID_CYC_SEL						M2P_MESSAGE_ACCESS(0xa7)
#define M2P_NFC_CTL_VALID_TIME							M2P_MESSAGE_ACCESS(0xa8)
#define M2P_NFC_CTL_SCAN_TIME      		                M2P_MESSAGE_ACCESS(0xa9)



#define M2P_RVD2PVDD_EN                              	M2P_MESSAGE_ACCESS(0xb0)
#define M2P_PVDD_EXTERN_DCDC                           	M2P_MESSAGE_ACCESS(0xb1)

#define M2P_USER_PEND                                   (0xb2)
#define M2P_USER_MSG_TYPE                               (0xb3)
#define M2P_USER_MSG_LEN0                               (0xb4)
#define M2P_USER_MSG_LEN1                               (0xb5)
#define M2P_USER_ADDR0                                  (0xb6)
#define M2P_USER_ADDR1                                  (0xb7)
#define M2P_USER_ADDR2                                  (0xb8)
#define M2P_USER_ADDR3                                  (0xb9)



/*
 *  Must Sync to P11 code
 */
enum {
    M2P_LP_INDEX    = 0,
    M2P_PF_INDEX,
    M2P_LLP_INDEX,
    M2P_P33_INDEX,
    M2P_SF_INDEX,
    M2P_CTMU_INDEX,
    M2P_CCMD_INDEX,       //common cmd
    M2P_VAD_INDEX,
    M2P_USER_INDEX,
    M2P_WDT_INDEX,
    M2P_NFC_INDEX,
    M2P_SYNC_INDEX,
    M2P_APP_INDEX,

};

enum {
    P2M_LP_INDEX    = 0,
    P2M_PF_INDEX,
    P2M_LLP_INDEX,
    P2M_WK_INDEX,
    P2M_WDT_INDEX,
    P2M_LP_INDEX2,
    P2M_CTMU_INDEX,
    P2M_CTMU_POWUP,
    P2M_REPLY_CCMD_INDEX,  //reply common cmd
    P2M_VAD_INDEX,
    P2M_USER_INDEX,
    P2M_BANK_INDEX,
    P2M_NFC_INDEX,
    P2M_NFC_POWUP,
    P2M_REPLY_SYNC_INDEX,
    P2M_APP_INDEX,
};

enum {
    CLOSE_P33_INTERRUPT = 1,
    OPEN_P33_INTERRUPT,
    LOWPOWER_PREPARE,
    M2P_SPIN_LOCK,
    M2P_SPIN_UNLOCK,
    P2M_SPIN_LOCK,
    P2M_SPIN_UNLOCK,

};

#include "power/lp_msg.h"

#endif
