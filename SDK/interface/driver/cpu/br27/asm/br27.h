

#ifndef __BR27__
#define __BR27__

//===============================================================================//
//
//      sfr define
//
//===============================================================================//

#define hs_base            0xfe0000
#define ls_base            0xfd0000

#define __RW               volatile       // read write
#define __RO               volatile const // only read
#define __WO               volatile       // only write

#define __u8               unsigned int   // u8  to u32 special for struct
#define __u16              unsigned int   // u16 to u32 special for struct
#define __u32              unsigned int

#define __s8(x)            char(x); char(reserved_1_##x); char(reserved_2_##x); char(reserved_3_##x)
#define __s16(x)           short(x); short(reserved_1_##x)
#define __s32(x)           int(x)

#define map_adr(grp, adr)  ((64 * grp + adr) * 4)     // grp(0x0-0xff), adr(0x0-0x3f)

//===============================================================================//
//
//      high speed sfr address define
//
//===============================================================================//

//............. 0x0000 - 0x00ff............ for hemu
typedef struct {
    __RW __u32 WREN;
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 MSG0;
    __RW __u32 MSG1;
    __RW __u32 MSG2;
    __RW __u32 MSG3;
    __RO __u32 ID;
} JL_HEMU_TypeDef;

#define JL_HEMU_BASE                 (hs_base + map_adr(0x00, 0x00))
#define JL_HEMU                      ((JL_HEMU_TypeDef    *)JL_HEMU_BASE)


//............. 0x0100 - 0x02ff............ for sfc
typedef struct {
    __RW __u32 CON;
    __RW __u32 BAUD;
    __RW __u32 REDU_BAUD;
    __RW __u32 CODE;
    __RW __u32 BASE_ADR;
    __RW __u32 QUCNT;
    __RW __u8  ENC_CON;
    __RW __u16 ENC_KEY;
    __WO __u32 UNENC_ADRH;
    __WO __u32 UNENC_ADRL;
} JL_SFC_TypeDef;

#define JL_SFC_BASE                     (hs_base + map_adr(0x02, 0x00))
#define JL_SFC                          ((JL_SFC_TypeDef    *)JL_SFC_BASE)

//............. 0x0300 - 0x03ff............ for psram
typedef struct {
    __RW __u32 PSRAM_CON0;
    __RW __u32 PSRAM_CON1;
    __RW __u32 PSRAM_CON2;
    __RW __u32 PSRAM_CFG0;
    __RW __u32 PSRAM_CFG1;
    __RW __u32 PSRAM_CMD;
    __RW __u32 PSRAM_DBG_CON;
    __RW __u32 PSRAM_DBG_START;
    __RW __u32 PSRAM_DBG_END;
    __RW __u32 PSRAM_DBG_ADR;
    __RW __u32 PSRAM_DBG_INFO;
} JL_PSRAM_TypeDef;

#define JL_PSRAM_BASE                   (hs_base + map_adr(0x03, 0x00))
#define JL_PSRAM                        ((JL_PSRAM_TypeDef  *)JL_PSRAM_BASE)

//............. 0x0500 - 0x05ff............ for audio_top
typedef struct {
    __RW __u32(DAC_CON);    /* 00 */
    __RW __u32(DAC_ADR);    /* 01 */
    __RW __u16(DAC_LEN);    /* 02 */
    __RW __u16(DAC_PNS);    /* 03 */
    __RW __u16(DAC_HRP);    /* 04 */
    __RW __u16(DAC_SWP);    /* 05 */
    __RW __u16(DAC_SWN);    /* 06 */
    __RW __u32(ADC_CON);    /* 07 */
    __RW __u32(ADC_ADR);    /* 08 */
    __RW __u16(ADC_LEN);    /* 09 */
    __RW __u16(ADC_PNS);    /* 10 */
    __RW __u16(ADC_HWP);    /* 11 */
    __RW __u16(ADC_SRP);    /* 12 */
    __RW __u16(ADC_SRN);    /* 13 */
    __RO __u32(ADC0_HWP);   /* 14 */
    __RO __u32(ADC1_HWP);   /* 15 */
    __RO __u32(ADC2_HWP);   /* 16 */
    __RO __u32(ADC3_HWP);   /* 17 */
    __RW __u32(RESERVED_AUDSYS[24 - 18]);

    __RW __u32(DAC_CON1);   /* 00 */
    __RW __u32(DAC_CON2);   /* 01 */
    __WO __u16(DAC_COP);    /* 02 */
    __RW __u32(DAC_VL0);    /* 03 */
    __RW __u32(DAC_VL1);    /* 04 */
    __RW __u32(DAC_TM0);    /* 05 */
    __RW __u32(DAC_TM1);    /* 06 */
    __RW __u32(DAC_TM2);    /* 07 */
    __RW __u32(DAC_TM3);    /* 08 */
    __RW __u32(DAC_DB0);    /* 09 */
    __RW __u32(DAC_DB1);    /* 10 */
    __RW __u32(DAC_DTV0);   /* 11 */
    __RW __u32(DAC_DTV1);   /* 12 */
    __RW __u32(DAC_DPD);    /* 13 */
    __RW __u32(DAC_DRC0);   /* 14 */
    __RW __u32(DAC_DRC1);   /* 15 */
    __WO __u32(DAC_DCW);    /* 16 */
    __RO __u32(DAC_DCR);    /* 17 */
    __RW __u16(ADC_CON1);   /* 18 */
    __RW __u16(ADC_CON2);   /* 19 */
    __WO __u32(ADC_DCW);    /* 20 */
    __RO __u32(ADC_DCR);    /* 21 */
    __RW __u32(AD2DA_CON);  /* 22 */
    __RW __u32(AD2DA_CON0); /* 23 */
    __RW __u32(AD2DA_CON1); /* 24 */
    __RW __u32(AD2DA_CON2); /* 25 */
    __RW __u32(AD2DA_CON3); /* 26 */
    __RW __u32(AD2DA_CON4); /* 27 */
    __RW __u32(AD2DA_CON5); /* 28 */
    __RW __u32(AD2DA_CON6); /* 29 */
    __RW __u32(AD2DA_CON7); /* 30 */
    __RW __u32(AD2DA_CON8); /* 31 */
    __RO __u32(AD2DA_CON9); /* 32 */
    __RO __u32(AD2DA_CON10); /* 33 */
    __RW __u32(MBG_CON);    /* 34 */
    __RW __u32(AUD_CON);    /* 35 */
    __RW __u32(PWM_CON0);   /* 36 */
    __RW __u32(PWM_CON1);   /* 37 */
    __RW __u32(RESERVED_AUDPRJ[40 - 38]);
} JL_AUDIO_TypeDef;

#define JL_AUDIO_BASE                (hs_base + map_adr(0x05, 0x00))
#define JL_AUDIO                     ((JL_AUDIO_TypeDef   *)JL_AUDIO_BASE)

//............. 0x0600 - 0x06ff............ for alnk
typedef struct {
    __RW __u16 CON0;
    __RW __u16 CON1;
    __RW __u8  CON2;
    __RW __u8  CON3;
    __WO __u32 ADR0;
    __WO __u32 ADR1;
    __WO __u32 ADR2;
    __WO __u32 ADR3;
    __WO __u16 LEN;
    __RW __u32 PNS;
    __RW __u32 HWPTR0;
    __RW __u32 HWPTR1;
    __RW __u32 HWPTR2;
    __RW __u32 HWPTR3;
    __RW __u32 SWPTR0;
    __RW __u32 SWPTR1;
    __RW __u32 SWPTR2;
    __RW __u32 SWPTR3;
    __RW __u32 SHN0;
    __RW __u32 SHN1;
    __RW __u32 SHN2;
    __RW __u32 SHN3;
} JL_ALNK_TypeDef;

#define JL_ALNK0_BASE                (hs_base + map_adr(0x06, 0x00))
#define JL_ALNK0                     ((JL_ALNK_TypeDef    *)JL_ALNK0_BASE)

//............. 0x0700 - 0x07ff............ for plnk
typedef struct {
    __RW __u16 CON;
    __RW __u8  SMR;
    __RW __u32 ADR;
    __RW __u32 LEN;
    __RW __u16 DOR;
    __RW __u32 CON1;

} JL_PLNK_TypeDef;

#define JL_PLNK_BASE                 (hs_base + map_adr(0x07, 0x00))
#define JL_PLNK                      ((JL_PLNK_TypeDef     *)JL_PLNK_BASE)

//............. 0x0900 - 0x09ff............ for ass_clkgen
typedef struct {
    __RW __u32 CLK_CON;
    __RW __u32 MIC_CTL0;
    __RW __u32 MIC_CTL1;
} JL_ASS_TypeDef;

#define JL_ASS_BASE                (hs_base + map_adr(0x09, 0x00))
#define JL_ASS                     ((JL_ASS_TypeDef  *)JL_ASS_BASE)

//............. 0x0a00 - 0x0aff............ for audio_ana
typedef struct {
    __RW __u32 DAA_CON0;
    __RW __u32 DAA_CON1;
    __RW __u32 DAA_CON2;
    __RW __u32 DAA_CON3;
    __RW __u32 DAA_CON4;
    __RO __u32 RESERVED5;
    __RO __u32 RESERVED6;
    __RO __u32 DAA_CON7;
    __RW __u32 ADA_CON0;
    __RW __u32 ADA_CON1;
    __RW __u32 ADA_CON2;
    __RW __u32 ADA_CON3;
    __RW __u32 ADA_CON4;
    __RW __u32 ADA_CON5;
    __RW __u32 ADA_CON6;
    __RW __u32 ADA_CON7;
    __RW __u32 ADA_CON8;
    __RW __u32 ADA_CON9;
    __RW __u32 ADDA_CON0;
    __RW __u32 ADDA_CON1;
} JL_ADDA_TypeDef;

#define JL_ADDA_BASE               (hs_base + map_adr(0x0a, 0x00))
#define JL_ADDA                    ((JL_ADDA_TypeDef       *)JL_ADDA_BASE)

//............. 0x0b00 - 0x0bff............ for ass_mbist
typedef struct {
    __RW __u32 CON;
    __RW __u32 SEL;
    __RW __u32 BEG;
    __RW __u32 END;
    __RW __u32 DAT_VLD0;
    __RW __u32 DAT_VLD1;
    __RW __u32 DAT_VLD2;
    __RW __u32 DAT_VLD3;
} JL_ASS_MBIST_TypeDef;

#define JL_ASS_MBIST_BASE                     (hs_base + map_adr(0x0b, 0x00))
#define JL_ASS_MBIST                          ((JL_ASS_MBIST_TypeDef  *)JL_ASS_MBIST_BASE)

//............. 0x1300 - 0x13ff............ for dc
typedef struct {
    __RW __u32 CON;
    __RW __u32 ADR;
} JL_DCP_TypeDef;

#define JL_DCP_BASE                     (hs_base + map_adr(0x13, 0x00))
#define JL_DCP                          ((JL_DCP_TypeDef  *)JL_DCP_BASE)


//............. 0x1400 - 0x15ff............ for eq
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 DATAI_ADR;
    __RW __u32 DATAO_ADR;
    __RW __u32 DATA_LEN;
    __RW __u32 FLT_ADR;

} JL_EQ_TypeDef;

#define JL_EQ_BASE                      (hs_base + map_adr(0x14, 0x00))
#define JL_EQ                           ((JL_EQ_TypeDef			*)JL_EQ_BASE)

#define JL_EQ1_BASE                     (hs_base + map_adr(0x15, 0x00))
#define JL_EQ1                          ((JL_EQ_TypeDef			*)JL_EQ1_BASE)
//............. 0x1600 - 0x16ff............ for src0
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 CON4;
    __RW __u32 CON5;
    __RW __u32 CON6;
    __RW __u32 RESERVEDb;
    __RW __u32 RESERVEDc;
    __RW __u32 RESERVEDd;
    __RW __u32 IDAT_ADR;
    __RW __u32 IDAT_LEN;
    __RW __u32 ODAT_ADR;
    __RW __u32 ODAT_LEN;
    __RW __u32 ODAT_ADR_STA;
    __RW __u32 ODAT_ADR_END;
    __RW __u32 INSR;
    __RW __u32 OUTSR;
    __RW __u32 PHASE;

} JL_SRC0_TypeDef;

#define JL_SRC0_BASE                    (hs_base + map_adr(0x16, 0x00))
#define JL_SRC0                         ((JL_SRC0_TypeDef			*)JL_SRC0_BASE)

#define JL_SRC_TypeDef 					JL_SRC0_TypeDef

//............. 0x1700 - 0x17ff............ for src1
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 IDAT_ADR;
    __RW __u32 IDAT_LEN;
    __RW __u32 ODAT_ADR;
    __RW __u32 ODAT_LEN;
    __RW __u32 FLTB_ADR;
    __WO __u32 ODAT_ADR_START;
    __WO __u32 ODAT_ADR_END;
    __WO __u32 STOP_FLAG;
    __RW __u32 INSR;
    __RW __u32 OUTSR;
    __RW __u32 PHASE;
    __RW __u32 COEF;

} JL_SRC1_TypeDef;

#define JL_SRC1_BASE                    (hs_base + map_adr(0x17, 0x00))
#define JL_SRC1                         ((JL_SRC1_TypeDef			*)JL_SRC1_BASE)

//............. 0x1800 - 0x18ff............ for emi
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 ADR;
    __RW __u32 CNT;
} JL_EMI_TypeDef;

#define JL_EMI_BASE                     (hs_base + map_adr(0x18, 0x00))
#define JL_EMI                          ((JL_EMI_TypeDef      *)JL_EMI_BASE)


//............. 0x2000 - 0x20ff............

//............. 0x2100 - 0x21ff............ for wireless
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON3;
    __RW __u32 LOFC_CON;
    __RW __u32 LOFC_RES;
    __RW __u32 DBG_CON;
} JL_WL_TypeDef;

#define JL_WL_BASE                      (hs_base + map_adr(0x21, 0x00))
#define JL_WL                           ((JL_WL_TypeDef     *)JL_WL_BASE)

//............. 0x2200 - 0x22ff............
typedef struct {
    __RW __u32 CON0;
} JL_WL_AUD_TypeDef;

#define JL_WL_AUD_BASE                  (hs_base + map_adr(0x22, 0x00))
#define JL_WL_AUD                       ((JL_WL_AUD_TypeDef *)JL_WL_AUD_BASE)

//............. 0x2300 - 0x23ff............

//............. 0x2400 - 0x24ff............

//............. 0x2500 - 0x25ff............ for audio_sync
typedef struct {
    __RW __u32 CON;
    __RW __u32 DABUF;
    __RO __u32 DASPACE;
    __RO __u32 OFFEND;
    __RO __u32 OFFSUM;
    __RO __u32 PCMNUM;
    __RO __u32 BTTIME;
    __WO __u32 CON1  ;
    __RW __u32 CON2  ;
    __WO __u32 BT_OFFSET;
    __WO __u32 MIN_SLOT;
    __RW __u32 OUTSR;
    __WO __u32 INSR;
} JL_SYNC_TypeDef;

#define JL_SYNC_BASE                 (hs_base + map_adr(0x25, 0x00))
#define JL_SYNC                      ((JL_SYNC_TypeDef *)JL_SYNC_BASE)

//............. 0x2600 - 0x26ff............ for src_sync
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 IDAT_ADR;
    __RW __u32 IDAT_LEN;
    __RW __u32 ODAT_ADR;
    __RW __u32 ODAT_LEN;
    __RW __u32 FLTB_ADR;
    __WO __u32 ODAT_ADR_START;
    __WO __u32 ODAT_ADR_END;
    __RW __u32 STOP_FLAG;
    __RW __u32 INSR;
    __RW __u32 OUTSR;
    __RW __u32 PHASE;
    __RW __u32 COEF;

} JL_SRC_SYNC_TypeDef;

#define JL_SRC_SYNC_BASE                     (hs_base + map_adr(0x26, 0x00))
#define JL_SRC_SYNC                          ((JL_SRC_SYNC_TypeDef *)JL_SRC_SYNC_BASE)

//............. 0x2700 - 0x27ff............
#define JL_ALNK1_BASE                (hs_base + map_adr(0x27, 0x00))
#define JL_ALNK1                     ((JL_ALNK_TypeDef    *)JL_ALNK1_BASE)

//............. 0x2800 - 0x28ff............

//............. 0x2900 - 0x29ff............
typedef struct {
    __RW __u32 CON;
    __RW __u32 SEL;
    __RW __u32 BEG;
    __RW __u32 END;
    __RW __u32 DAT_VLD0;
    __RW __u32 DAT_VLD1;
    __RW __u32 DAT_VLD2;
    __RW __u32 DAT_VLD3;
} JL_HMBIST_TypeDef;

#define JL_HMBIST_BASE               (hs_base + map_adr(0x29, 0x00))
#define JL_HMBIST                    ((JL_HMBIST_TypeDef *)JL_HMBIST_BASE)

//............. 0x2a00 - 0x2aff............
typedef struct {
    __RW __u32 CON;
    __RW __u32 SEL;
    __RW __u32 BEG;
    __RW __u32 END;
    __RW __u32 DAT_VLD0;
    __RW __u32 DAT_VLD1;
    __RW __u32 DAT_VLD2;
    __RW __u32 DAT_VLD3;
} JL_WL_MBIST_TypeDef;

#define JL_WL_MBIST_BASE             (hs_base + map_adr(0x2a, 0x00))
#define JL_WL_MBIST                  ((JL_WL_MBIST_TypeDef *)JL_WL_MBIST_BASE)

//............. 0x2b00 - 0x2bff............

//............. 0x2c00 - 0x2cff............

//............. 0x2d00 - 0x2dff............
typedef struct {
    __RW __u32 IMD_CON0;
    __RW __u32 IMD_CON1;
    __RW __u32 IMD_CON2;
    __RW __u32 IMD_CON3;
    __RW __u32 IMD_LD_START_ADR;
    __RW __u32 IMD_DEBUG_PIXEL;
    __RW __u32 IMD_IO_CFG;
    __RW __u16 IMD_BAUD;
    __RW __u32 IMDSPI_CON0;
    __RW __u32 IMDSPI_BUF;
    __RW __u32 IMDPAP_CON0;
    __RW __u32 IMDPAP_PRD;
    __RW __u32 IMDPAP_BUF;
    __RW __u32 IMDRGB_CON0;
    __RW __u32 IMDRGB_HPRD;
    __RW __u32 IMDRGB_VPRD;
} JL_IMD_TypeDef;

#define JL_IMD_BASE                  (hs_base + map_adr(0x2d, 0x00))
#define JL_IMD                       ((JL_IMD_TypeDef *)JL_IMD_BASE)


//===============================================================================//
//
//      low speed sfr address define
//
//===============================================================================//

//............. 0x0000 - 0x00ff............ for syscfg
typedef struct {
    __RW __u32 RST_SRC;
} JL_RST_TypeDef;

#define JL_RST_BASE             (ls_base + map_adr(0x00, 0x00))
#define JL_RST                  ((JL_RST_TypeDef        *)JL_RST_BASE)

typedef struct {
    __RW __u32 PWR_CON;
    __RW __u32 SYS_SEL;
    __RW __u32 SYS_DIV;
    __RW __u32 DCCK_CON0;
    __RW __u32 CLK_CON0;
    __RW __u32 CLK_CON1;
    __RW __u32 CLK_CON2;
    __RW __u32 CLK_CON3;
    __RW __u32 CLK_CON4;
    __RW __u32 CLK_CON5;
} JL_CLOCK_TypeDef;

#define JL_CLOCK_BASE           (ls_base + map_adr(0x00, 0x01))
#define JL_CLOCK                ((JL_CLOCK_TypeDef      *)JL_CLOCK_BASE)

//............. 0x0100 - 0x01ff............
typedef struct {
    __RW __u32 MODE_CON;
} JL_MODE_TypeDef;

#define JL_MODE_BASE                  (ls_base + map_adr(0x01, 0x00))
#define JL_MODE                       ((JL_MODE_TypeDef     *)JL_MODE_BASE)

//............. 0x0200 - 0x02ff............
typedef struct {
    __WO __u32 CHIP_ID;
} JL_SYSTEM_TypeDef;

#define JL_SYSTEM_BASE                  (ls_base + map_adr(0x02, 0x00))
#define JL_SYSTEM                       ((JL_SYSTEM_TypeDef     *)JL_SYSTEM_BASE)

//............. 0x0300 - 0x03ff............
typedef struct {
    __RW __u32 WREN;
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 MSG0;
    __RW __u32 MSG1;
    __RW __u32 MSG2;
    __RW __u32 MSG3;
    __RO __u32 ID;
} JL_LEMU_TypeDef;

#define JL_LEMU_BASE                  (ls_base + map_adr(0x03, 0x00))
#define JL_LEMU                       ((JL_LEMU_TypeDef    *)JL_LEMU_BASE)

//............. 0x0400 - 0x09ff............
typedef struct {
    __RW __u32 CON;
    __RW __u32 CNT;
    __RW __u32 PRD;
    __RW __u32 PWM;
} JL_TIMER_TypeDef;

#define JL_TIMER0_BASE                  (ls_base + map_adr(0x04, 0x00))
#define JL_TIMER0                       ((JL_TIMER_TypeDef     *)JL_TIMER0_BASE)

#define JL_TIMER1_BASE                  (ls_base + map_adr(0x05, 0x00))
#define JL_TIMER1                       ((JL_TIMER_TypeDef     *)JL_TIMER1_BASE)

#define JL_TIMER2_BASE                  (ls_base + map_adr(0x06, 0x00))
#define JL_TIMER2                       ((JL_TIMER_TypeDef     *)JL_TIMER2_BASE)

#define JL_TIMER3_BASE                  (ls_base + map_adr(0x07, 0x00))
#define JL_TIMER3                       ((JL_TIMER_TypeDef     *)JL_TIMER3_BASE)

#define JL_TIMER4_BASE                  (ls_base + map_adr(0x08, 0x00))
#define JL_TIMER4                       ((JL_TIMER_TypeDef     *)JL_TIMER4_BASE)

#define JL_TIMER5_BASE                  (ls_base + map_adr(0x09, 0x00))
#define JL_TIMER5                       ((JL_TIMER_TypeDef     *)JL_TIMER5_BASE)


//............. 0x1000 - 0x10ff............
typedef struct {
    __RW __u32 CON;
    __RW __u32 VAL;
} JL_PCNT_TypeDef;

#define JL_PCNT_BASE                    (ls_base + map_adr(0x10, 0x00))
#define JL_PCNT                         ((JL_PCNT_TypeDef       *)JL_PCNT_BASE)

//............. 0x1100 - 0x11ff............
typedef struct {
    __RW __u32 CON;
    __RO __u32 NUM;
} JL_GPCNT_TypeDef;

#define JL_GPCNT_BASE                   (ls_base + map_adr(0x11, 0x00))
#define JL_GPCNT                        ((JL_GPCNT_TypeDef     *)JL_GPCNT_BASE)


//............. 0x1400 - 0x17ff............
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __WO __u32 CPTR;
    __WO __u32 DPTR;
    __RW __u32 CTU_CON;
    __WO __u32 CTU_CNT;
} JL_SD_TypeDef;

#define JL_SD0_BASE                     (ls_base + map_adr(0x14, 0x00))
#define JL_SD0                          ((JL_SD_TypeDef        *)JL_SD0_BASE)

#define JL_SD1_BASE                     (ls_base + map_adr(0x15, 0x00))
#define JL_SD1                          ((JL_SD_TypeDef        *)JL_SD1_BASE)

//............. 0x1800 - 0x18ff............
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __WO __u32 EP0_CNT;
    __WO __u32 EP1_CNT;
    __WO __u32 EP2_CNT;
    __WO __u32 EP3_CNT;
    __WO __u32 EP4_CNT;
    __WO __u32 EP0_ADR;
    __WO __u32 EP1_TADR;
    __WO __u32 EP1_RADR;
    __WO __u32 EP2_TADR;
    __WO __u32 EP2_RADR;
    __WO __u32 EP3_TADR;
    __WO __u32 EP3_RADR;
    __WO __u32 EP4_TADR;
    __WO __u32 EP4_RADR;
    __RW __u32 TXDLY_CON;
    __RW __u32 EP1_RLEN;
    __RW __u32 EP2_RLEN;
    __RW __u32 EP3_RLEN;
    __RW __u32 EP4_RLEN;
    __RW __u32 EP1_MTX_PRD;
    __RW __u32 EP1_MRX_PRD;
    __RO __u32 EP1_MTX_NUM;
    __RO __u32 EP1_MRX_NUM;
} JL_USB_TypeDef;

#define JL_USB_BASE                     (ls_base + map_adr(0x18, 0x00))
#define JL_USB                          ((JL_USB_TypeDef       *)JL_USB_BASE)

//............. 0x1900 - 0x19ff............
typedef struct {
    __RW __u32 WLA_CON0 ;
    __RW __u32 WLA_CON1 ;
    __RW __u32 WLA_CON2 ;
    __RW __u32 WLA_CON3 ;
    __RW __u32 WLA_CON4 ;
    __RW __u32 WLA_CON5 ;
    __RW __u32 WLA_CON6 ;
    __RW __u32 WLA_CON7 ;
    __RW __u32 WLA_CON8 ;
    __RW __u32 WLA_CON9 ;
    __RW __u32 WLA_CON10;
    __RW __u32 WLA_CON11;
    __RW __u32 WLA_CON12;
    __RW __u32 WLA_CON13;
    __RW __u32 WLA_CON14;
    __RW __u32 WLA_CON15;
    __RW __u32 WLA_CON16;
    __RW __u32 WLA_CON17;
    __RW __u32 WLA_CON18;
    __RW __u32 WLA_CON19;
    __RW __u32 WLA_CON20;
    __RW __u32 WLA_CON21;
    __RW __u32 WLA_CON22;
    __RW __u32 WLA_CON23;
    __RW __u32 WLA_CON24;
    __RW __u32 WLA_CON25;
    __RW __u32 WLA_CON26;
    __RW __u32 WLA_CON27;
    __RW __u32 WLA_CON28;
    __RW __u32 WLA_CON29;
    __RO __u32 WLA_CON30;
    __RW __u32 WLA_CON31;
    __RO __u32 WLA_CON32;
    __RO __u32 WLA_CON33;
    __RO __u32 WLA_CON34;
    __RO __u32 WLA_CON35;
    __RO __u32 WLA_CON36;
    __RO __u32 WLA_CON37;
    __RO __u32 WLA_CON38;
    __RO __u32 WLA_CON39;
} JL_WLA_TypeDef;

#define JL_WLA_BASE                     (ls_base + map_adr(0x19, 0x00))
#define JL_WLA                          ((JL_WLA_TypeDef       *)JL_WLA_BASE)

//............. 0x1c00 - 0x1eff............
typedef struct {
    __RW __u32 CON;
    __RW __u32 BAUD;
    __RW __u32 BUF;
    __WO __u32 ADR;
    __RW __u32 CNT;
    __RW __u32 CON1;
} JL_SPI_TypeDef;

#define JL_SPI0_BASE                    (ls_base + map_adr(0x1c, 0x00))
#define JL_SPI0                         ((JL_SPI_TypeDef      *)JL_SPI0_BASE)

#define JL_SPI1_BASE                    (ls_base + map_adr(0x1d, 0x00))
#define JL_SPI1                         ((JL_SPI_TypeDef      *)JL_SPI1_BASE)

#define JL_SPI2_BASE                    (ls_base + map_adr(0x1e, 0x00))
#define JL_SPI2                         ((JL_SPI_TypeDef      *)JL_SPI2_BASE)

//............. 0x2000 - 0x22ff............
typedef struct {
    __RW __u16 CON0;
    __RW __u16 CON1;
    __RW __u16 CON2;
    __RW __u16 BAUD;
    __RW __u8  BUF;
    __RW __u32 OTCNT;
    __RW __u32 TXADR;
    __WO __u16 TXCNT;
    __RW __u32 RXSADR;
    __RW __u32 RXEADR;
    __RW __u32 RXCNT;
    __RO __u16 HRXCNT;
    __RO __u16 RX_ERR_CNT;
} JL_UART_TypeDef;

#define JL_UART0_BASE                   (ls_base + map_adr(0x20, 0x00))
#define JL_UART0                        ((JL_UART_TypeDef       *)JL_UART0_BASE)

#define JL_UART1_BASE                   (ls_base + map_adr(0x21, 0x00))
#define JL_UART1                        ((JL_UART_TypeDef       *)JL_UART1_BASE)

#define JL_UART2_BASE                   (ls_base + map_adr(0x22, 0x00))
#define JL_UART2                        ((JL_UART_TypeDef       *)JL_UART2_BASE)

//............. 0x2400 - 0x24ff............
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u16 BAUD;
    __RW __u8  BUF;
} JL_IIC_TypeDef;

#define JL_IIC_BASE                     (ls_base + map_adr(0x24, 0x00))
#define JL_IIC                          ((JL_IIC_TypeDef       *)JL_IIC_BASE)

//............. 0x2800 - 0x28ff............
typedef struct {
    __RW __u32 CON0;
    __RW __u32 WCLK_DUTY;
    __RW __u32 WCLK_PRD;
    __WO __u32 DMA_BASE_ADR;
    __WO __u32 DMA_BUF_LEN;
    __RW __u32 SLOT_EN;

} JL_TDM_TypeDef;

#define JL_TDM_BASE                   (ls_base + map_adr(0x28, 0x00))
#define JL_TDM                        ((JL_TDM_TypeDef *)JL_TDM_BASE)

//............. 0x2900 - 0x29ff............
typedef struct {
    __RW __u32 TMR0_CON;
    __RW __u32 TMR0_CNT;
    __RW __u32 TMR0_PR;
    __RW __u32 TMR1_CON;
    __RW __u32 TMR1_CNT;
    __RW __u32 TMR1_PR;
    __RW __u32 TMR2_CON;
    __RW __u32 TMR2_CNT;
    __RW __u32 TMR2_PR;
    __RW __u32 TMR3_CON;
    __RW __u32 TMR3_CNT;
    __RW __u32 TMR3_PR;
    __RW __u32 TMR4_CON;
    __RW __u32 TMR4_CNT;
    __RW __u32 TMR4_PR;
    __RW __u32 TMR5_CON;
    __RW __u32 TMR5_CNT;
    __RW __u32 TMR5_PR;
    __RW __u32 TMR6_CON;
    __RW __u32 TMR6_CNT;
    __RW __u32 TMR6_PR;
    __RW __u32 TMR7_CON;
    __RW __u32 TMR7_CNT;
    __RW __u32 TMR7_PR;
    __RW __u32 FPIN_CON;
    __RW __u32 CH0_CON0;
    __RW __u32 CH0_CON1;
    __RW __u32 CH0_CMPH;
    __RW __u32 CH0_CMPL;
    __RW __u32 CH1_CON0;
    __RW __u32 CH1_CON1;
    __RW __u32 CH1_CMPH;
    __RW __u32 CH1_CMPL;
    __RW __u32 CH2_CON0;
    __RW __u32 CH2_CON1;
    __RW __u32 CH2_CMPH;
    __RW __u32 CH2_CMPL;
    __RW __u32 CH3_CON0;
    __RW __u32 CH3_CON1;
    __RW __u32 CH3_CMPH;
    __RW __u32 CH3_CMPL;
    __RW __u32 CH4_CON0;
    __RW __u32 CH4_CON1;
    __RW __u32 CH4_CMPH;
    __RW __u32 CH4_CMPL;
    __RW __u32 CH5_CON0;
    __RW __u32 CH5_CON1;
    __RW __u32 CH5_CMPH;
    __RW __u32 CH5_CMPL;
    __RW __u32 CH6_CON0;
    __RW __u32 CH6_CON1;
    __RW __u32 CH6_CMPH;
    __RW __u32 CH6_CMPL;
    __RW __u32 CH7_CON0;
    __RW __u32 CH7_CON1;
    __RW __u32 CH7_CMPH;
    __RW __u32 CH7_CMPL;
    __RW __u32 MCPWM_CON0;
} JL_MCPWM_TypeDef;

#define JL_MCPWM_BASE                   (ls_base + map_adr(0x29, 0x00))
#define JL_MCPWM                        ((JL_MCPWM_TypeDef     *)JL_MCPWM_BASE)

//............. 0x2b00 - 0x2dff............
typedef struct {
    __RW __u8  CON;
    __RW __u8  DAT;
    __RW __u8  SMP;
    __RW __u8  DBE;
} JL_QDEC_TypeDef;

#define JL_QDEC0_BASE                   (ls_base + map_adr(0x2b, 0x00))
#define JL_QDEC0                        ((JL_QDEC_TypeDef       *)JL_QDEC0_BASE)

#define JL_QDEC1_BASE                   (ls_base + map_adr(0x2c, 0x00))
#define JL_QDEC1                        ((JL_QDEC_TypeDef       *)JL_QDEC1_BASE)

#define JL_QDEC2_BASE                   (ls_base + map_adr(0x2d, 0x00))
#define JL_QDEC2                        ((JL_QDEC_TypeDef       *)JL_QDEC2_BASE)

//............. 0x3100 - 0x31ff............
typedef struct {
    __RW __u32 CON;
    __RO __u32 RES;
    __RW __u32 DMA_CON0;
    __RW __u32 DMA_CON1;
    __RW __u32 DMA_CON2;
    __RW __u32 DMA_BADR;
} JL_ADC_TypeDef;

#define JL_ADC_BASE                     (ls_base + map_adr(0x31, 0x00))
#define JL_ADC                          ((JL_ADC_TypeDef       *)JL_ADC_BASE)

//............. 0x3200 - 0x32ff............
typedef struct {
    __RW __u32 RFLT_CON;
} JL_IR_TypeDef;

#define JL_IR_BASE                      (ls_base + map_adr(0x32, 0x00))
#define JL_IR                           ((JL_IR_TypeDef         *)JL_IR_BASE)

//............. 0x3400 - 0x34ff............
typedef struct {
    __RW __u32 CON;
} JL_OSA_TypeDef;

#define JL_OSA_BASE                     (ls_base + map_adr(0x34, 0x00))
#define JL_OSA                          ((JL_OSA_TypeDef          *)JL_OSA_BASE)

//............. 0x3500 - 0x35ff............
typedef struct {
    __WO __u32 FIFO;
    __RW __u32 REG;
} JL_CRC_TypeDef;

#define JL_CRC_BASE                     (ls_base + map_adr(0x35, 0x00))
#define JL_CRC                          ((JL_CRC_TypeDef       *)JL_CRC_BASE)


//............. 0x3600 - 0x36ff............
typedef struct {
    __WO __u32 CON;
    __RW __u32 NUM;
} JL_LRCT_TypeDef;

#define JL_LRCT_BASE                    (ls_base + map_adr(0x36, 0x00))
#define JL_LRCT                         ((JL_LRCT_TypeDef     *)JL_LRCT_BASE)

//............. 0x3b00 - 0x3bff............
typedef struct {
    __RO __u32 R64L;
    __RO __u32 R64H;
} JL_RAND_TypeDef;

#define JL_RAND_BASE                    (ls_base + map_adr(0x3b, 0x00))
#define JL_RAND                         ((JL_RAND_TypeDef   *)JL_RAND_BASE)

//............. 0x3c00 - 0x3cff............
typedef struct {
    __RW __u32 CLK;
} JL_LEDCK_TypeDef;
#define JL_LEDCK_BASE                (ls_base + map_adr(0x3c,0x00))
#define JL_LEDCK                     ((JL_LEDCK_TypeDef     *)JL_LEDCK_BASE)

typedef struct {
    __RW __u32 CON;
    __RW __u32 FD;
    __RW __u32 LP;
    __RW __u32 TIX;
    __RW __u32 RSTX;
    __WO __u32 ADR;
} JL_LED_TypeDef;
#define JL_LED0_BASE                 (ls_base + map_adr(0x3c, 0x01))
#define JL_LED0                      ((JL_LED_TypeDef      *)JL_LED0_BASE)

#define JL_LED1_BASE                 (ls_base + map_adr(0x3c, 0x09))
#define JL_LED1                      ((JL_LED_TypeDef      *)JL_LED1_BASE)

#define JL_LED2_BASE                 (ls_base + map_adr(0x3c, 0x11))
#define JL_LED2                      ((JL_LED_TypeDef      *)JL_LED2_BASE)

//............. 0x3e00 - 0x3eff............ for p33
typedef struct {
    __RW __u32 PMU_CON;
    __RW __u32 PMU_STA;
} JL_PMU_TypeDef;

#define JL_PMU_BASE                 (ls_base + map_adr(0x3e, 0x00))
#define JL_PMU                      ((JL_PMU_TypeDef        *)JL_PMU_BASE)

//............. 0x4000 - 0x40ff............
typedef struct {
    /* 1 */ __RW __u32 TX_CON0;
    /* 2 */ __RW __u32 TX_CON1;
    /* 3 */ __RW __u32 TX_PILOT;
    /* 4 */ __RW __u32 TX_SYN_GAIN;
    /* 5 */ __RW __u32 TX_MUL;
    /* 6 */ __RW __u32 TX_ADR;
    /* 7 */ __RW __u32 TX_LEN;
    /* 8 */ __RW __u32 TX_FREQ;
    /* 9 */ __RW __u32 TX_BASE_ADR;
    /* 10*/ __RW __u32 TX_RDS_ADR;
    /* 11*/ __RW __u32 TX_RDS_CON0;
    /* 12*/ __RW __u32 TX_RDS_CON1;
    /* 13*/ __RW __u32 TX_RDS_MUL;
    /*~24*/ __RO __u32 TX_RESERVE[24 - 13];

    /* 1 */ __RW __u32 RXSYS_CON0;
    /* 2 */ __RW __u32 RXSYS_CON1;
    /* 3 */ __RW __u32 RX_AUD_BASE;
    /* 4 */ __RW __u32 RX_RDS_BASE;
    /*~8*/  __RO __u32 RXSYS_RESERVE[8 - 4];

    /* 1 */ __RW __u32 RX_CON0;
    /* 2 */ __RW __u32 RX_CON1;
    /* 3 */ __RW __u32 RX_CON2;
    /* 4 */ __RW __u32 RX_CON3;
    /* 5 */ __RW __u32 RX_CON4;
    /* 6 */ __RW __u32 RX_CON5;
    /* 7 */ __RW __u32 RX_CON6;
    /* 8 */ __RW __u32 RX_CON7;
    /* 9 */ __RW __u32 RX_CON8;
    /* 10*/ __RW __u32 RX_CON9;
    /* 11*/ __RW __u32 RX_CON10;
    /* 12*/ __RW __u32 RX_CON11;
    /* 13*/ __RW __u32 RX_CON12;
    /* 14*/ __RW __u32 RX_CON13;
    /* 15*/ __RW __u32 RX_CON14;
    /* 16*/ __RW __u32 RX_CON15;
    /* 17*/ __RW __u32 RX_CON16;
    /* 18*/ __RW __u32 RX_CON17;
    /* 19*/ __RW __u32 RX_CON18;
    /* 20*/ __RW __u32 RX_CON19;
    /* 21*/ __RW __u32 RX_CON20;
    /* 22*/ __RW __u32 RX_CON21;
    /* 23*/ __RW __u32 RX_CON22;
    /* 24*/ __RW __u32 RX_CON23;
    /* 25*/ __RW __u32 RX_CON24;
    /* 26*/ __RW __u32 RX_CON25;
    /* 27*/ __RW __u32 RX_CON26;
    /* 28*/ __RW __u32 RX_CON27;
    /* 29*/ __RW __u32 RX_CON28;
    /* 30*/ __RW __u32 RX_CON29;
    /*~32*/ __RO __u32 RX_RESERVE[32 - 30];
} JL_FM_TypeDef;

#define JL_FM_BASE                      (ls_base + map_adr(0x40, 0x00))
#define JL_FM                           ((JL_FM_TypeDef			*)JL_FM_BASE)


//............. 0x4100 - 0x42ff............ for lsb peri(spi0/sd0)
typedef struct {
    __RW __u8  ENCCON ;
    __WO __u16 ENCKEY ;
    __WO __u16 ENCADR ;
} JL_PERIENC_TypeDef;

#define JL_PERIENC0_BASE             (ls_base + map_adr(0x41, 0x00))
#define JL_PERIENC0                 ((JL_PERIENC_TypeDef *)JL_PERIENC0_BASE)

#define JL_PERIENC1_BASE             (ls_base + map_adr(0x42, 0x00))
#define JL_PERIENC1                 ((JL_PERIENC_TypeDef *)JL_PERIENC1_BASE)

#define JL_PERIENC_BASE              (ls_base + map_adr(0x41, 0x00))
#define JL_PERIENC                   ((JL_PERIENC_TypeDef *)JL_PERIENC_BASE)

//............. 0x4300 - 0x43ff............ for sbc
typedef struct {
    __RW __u32 CON0;
    __WO __u32 DEC_SRC_ADR;
    __WO __u32 DEC_DST_ADR;
    __WO __u32 DEC_PCM_WCNT;
    __WO __u32 DEC_INBUF_LEN;
    __WO __u32 ENC_SRC_ADR;
    __WO __u32 ENC_DST_ADR;
    __RO __u32 DEC_DST_BASE;
    __WO __u32 DEC_WEIGHT_BASE;
    __WO __u32 DEC_WEIGHT_ADD;

} JL_SBC_TypeDef;

#define JL_SBC_BASE                   (ls_base + map_adr(0x43, 0x00))
#define JL_SBC                        ((JL_SBC_TypeDef *)JL_SBC_BASE)

//............. 0x4300 - 0x44ff............ for aes
typedef struct {
    __RW __u32 CON;
    __RW __u32 DATIN;
    __WO __u32 KEY;
    __RW __u32 ENCRES0;
    __RW __u32 ENCRES1;
    __RW __u32 ENCRES2;
    __RW __u32 ENCRES3;
    __WO __u32 NONCE;
    __WO __u8  HEADER;
    __WO __u32 SRCADR;
    __WO __u32 DSTADR;
    __WO __u32 CTCNT;
    __WO __u32 TAGLEN;
    __RO __u32 TAGRES0;
    __RO __u32 TAGRES1;
    __RO __u32 TAGRES2;
    __RO __u32 TAGRES3;
} JL_AES_TypeDef;

#define JL_AES_BASE               (ls_base + map_adr(0x44, 0x00))
#define JL_AES                    ((JL_AES_TypeDef *)JL_AES_BASE)

//............. 0x4500 - 0x45ff............ for spdif
typedef struct {
    __RW __u32(SM_CLK);
    __RW __u32(SM_CON);
    __RW __u32(SM_ABADR);
    __RW __u32(SM_ABUFLEN);
    __RW __u32(SM_AHPTR);
    __RW __u32(SM_ASPTR);
    __RW __u32(SM_ASHN);
    __RW __u32(SM_APNS);
    __RW __u32(SM_IBADR);
    __RW __u32(SM_IBUFLEN);
    __RW __u32(SM_IHPTR);
    __RW __u32(SS_CON);
    __RO __u32(SS_SR_CNT);
    __RW __u32(SS_IO_CON);
    __RW __u32(SS_DMA_CON);
    __RW __u32(SS_DMA_LEN);
    __WO __u32(SS_DAT_ADR);
    __WO __u32(SS_INF_ADR);
    __RO __u32(SS_CSB0);
    __RO __u32(SS_CSB1);
    __RO __u32(SS_CSB2);
    __RO __u32(SS_CSB3);
    __RO __u32(SS_CSB4);
    __RO __u32(SS_CSB5);
} JL_SPDIF_TypeDef;

#define JL_SPDIF_BASE                  (ls_base + map_adr(0x45, 0x00))
#define JL_SPDIF                       ((JL_SPDIF_TypeDef    *) JL_SPDIF_BASE)

//............. 0x4600 - 0x46ff............ for pll0_ctl
//typedef struct {
//  __RW __u32 CON0;
//  __RW __u32 CON1;
//  __RW __u32 CON2;
//  __RW __u32 CON3;
//  __RW __u32 CON4;
//  __RW __u32 INTF0;
//  __RW __u32 INTF1;
//} JL_USBPLL_TypeDef;
//
//#define JL_USBPLL_BASE                   (ls_base + map_adr(0x46, 0x00))
//#define JL_USBPLL                        ((JL_USBPLL_TypeDef      *)JL_USBPLL_BASE)

//............. 0x4700 - 0x47ff............ for pll1_ctl
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 CON4;
    __RW __u32 CON8;
    __RW __u32 CON9;
    __RW __u32 CON10;
    __RW __u32 INTF0;
    __RW __u32 INTF1;
} JL_SYSPLL_TypeDef;

#define JL_SYSPLL_BASE                   (ls_base + map_adr(0x47, 0x00))
#define JL_SYSPLL                        ((JL_SYSPLL_TypeDef      *)JL_SYSPLL_BASE)


//............. 0x4a00 - 0x4fff............ for sie
typedef struct {
    __RW __u32 FADDR;
    __RW __u32 POWER;
    __RO __u32 INTRTX1;
    __RO __u32 INTRTX2;
    __RO __u32 INTRRX1;
    __RO __u32 INTRRX2;
    __RO __u32 INTRUSB;
    __RW __u32 INTRTX1E;
    __RW __u32 INTRTX2E;
    __RW __u32 INTRRX1E;
    __RW __u32 INTRRX2E;
    __RW __u32 INTRUSBE;
    __RO __u32 FRAME1;
    __RO __u32 FRAME2;
    __RO __u32 RESERVED14;
    __RW __u32 DEVCTL;
    __RO __u32 RESERVED10_0x16[0x16 - 0x10 + 1];

} JL_USB_SIE_TypeDef;

#define JL_USB_SIE_BASE                   (ls_base + map_adr(0x4a, 0x00))
#define JL_USB_SIE                        ((JL_USB_SIE_TypeDef *)JL_USB_SIE_BASE)

typedef struct {
    __RO __u32 RESERVED0;
    __RW __u32 CSR0;
    __RO __u32 RESERVED2_5[5 - 1];
    __RO __u32 COUNT0;

} JL_USB_EP0_TypeDef;

#define JL_USB_EP0_BASE                   (ls_base + map_adr(0x4a, 0x10))
#define JL_USB_EP0                        ((JL_USB_EP0_TypeDef *)JL_USB_EP0_BASE)


typedef struct {
    __RW __u32 TXMAXP;
    __RW __u32 TXCSR1;
    __RW __u32 TXCSR2;
    __RW __u32 RXMAXP;
    __RW __u32 RXCSR1;
    __RW __u32 RXCSR2;
    __RO __u32 RXCOUNT1;
    __RO __u32 RXCOUNT2;
    __RW __u32 TXTYPE;
    __RO __u32 TXINTERVAL;
    __RW __u32 RXTYPE;
    __RO __u32 RXINTERVAL;

} JL_USB_EP_TypeDef;

#define JL_USB_EP1_BASE                   (ls_base + map_adr(0x4b, 0x10))
#define JL_USB_EP1                        ((JL_USB_EP_TypeDef *)JL_USB_EP1_BASE)

#define JL_USB_EP2_BASE                   (ls_base + map_adr(0x4c, 0x10))
#define JL_USB_EP2                        ((JL_USB_EP_TypeDef *)JL_USB_EP2_BASE)

#define JL_USB_EP3_BASE                   (ls_base + map_adr(0x4d, 0x10))
#define JL_USB_EP3                        ((JL_USB_EP_TypeDef *)JL_USB_EP3_BASE)

#define JL_USB_EP4_BASE                   (ls_base + map_adr(0x4e, 0x10))
#define JL_USB_EP4                        ((JL_USB_EP_TypeDef *)JL_USB_EP4_BASE)


//............. 0x5000 - 0x57ff............ for port
typedef struct {
    __RO __u32 IN  ;
    __RW __u32 OUT ;
    __RW __u32 DIR ;
    __RW __u32 DIE ;
    __RW __u32 DIEH;
    __RW __u32 PU0 ;
    __RW __u32 PU1 ;
    __RW __u32 PD0 ;
    __RW __u32 PD1 ;
    __RW __u32 HD0 ;
    __RW __u32 HD1 ;
} JL_PORT_FLASH_TypeDef;

#define JL_PORTA_BASE                   (ls_base + map_adr(0x50, 0x00))
#define JL_PORTA                        ((JL_PORT_FLASH_TypeDef *)JL_PORTA_BASE)

#define JL_PORTB_BASE                   (ls_base + map_adr(0x51, 0x00))
#define JL_PORTB                        ((JL_PORT_FLASH_TypeDef *)JL_PORTB_BASE)

#define JL_PORTC_BASE                   (ls_base + map_adr(0x52, 0x00))
#define JL_PORTC                        ((JL_PORT_FLASH_TypeDef *)JL_PORTC_BASE)

#define JL_PORTD_BASE                   (ls_base + map_adr(0x53, 0x00))
#define JL_PORTD                        ((JL_PORT_FLASH_TypeDef *)JL_PORTD_BASE)

#define JL_PORTP_BASE                   (ls_base + map_adr(0x57, 0x00))
#define JL_PORTP                        ((JL_PORT_FLASH_TypeDef *)JL_PORTP_BASE)

//............. 0x6000 - 0x60ff............ for port others
typedef struct {
    __RW __u32 CON0;
} JL_HUSB_IO_TypeDef;

#define JL_HUSB_IO_BASE                  (ls_base + map_adr(0x60, 0x00))
#define JL_HUSB_IO                       ((JL_HUSB_IO_TypeDef    *)JL_HUSB_IO_BASE)
#define JL_USB_IO                       ((JL_HUSB_IO_TypeDef    *)JL_HUSB_IO_BASE)

typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
} JL_WAKEUP_TypeDef;

#define JL_WAKEUP_BASE               (ls_base + map_adr(0x60, 0x01))
#define JL_WAKEUP                    ((JL_WAKEUP_TypeDef			*)JL_WAKEUP_BASE)

typedef struct {
    //__RW __u32 IOMC0;
    __RW __u32 CON0;
    __RW __u32 OCH_CON0;
    __RW __u32 OCH_CON1;
    __RW __u32 ICH_CON0;
    __RW __u32 ICH_CON1;
    __RW __u32 ICH_CON2;
} JL_IOMC_TypeDef;

#define JL_IOMC_BASE                   (ls_base + map_adr(0x60, 0x05))
#define JL_IOMC                        ((JL_IOMC_TypeDef      *)JL_IOMC_BASE)
#define JL_IOMAP                        ((JL_IOMC_TypeDef      *)JL_IOMC_BASE)

//............. 0x6100 - 0x61ff............ for led
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 CON2;
    __RW __u32 CON3;
    __RW __u32 BRI_PRDL;
    __RW __u32 BRI_PRDH;
    __RW __u32 BRI_DUTY0L;
    __RW __u32 BRI_DUTY0H;
    __RW __u32 BRI_DUTY1L;
    __RW __u32 BRI_DUTY1H;
    __RW __u32 PRD_DIVL;
    __RW __u32 DUTY0;
    __RW __u32 DUTY1;
    __RW __u32 DUTY2;
    __RW __u32 DUTY3;
    __RO __u32 CNT_RD;
    __RW __u32 CON4;
    __WO __u32 CNT_SYNC;
    __RW __u32 CNT_DEC;
} JL_PLED_TypeDef;

#define JL_PLED_BASE                   (ls_base + map_adr(0x61, 0x00))
#define JL_PLED                        ((JL_PLED_TypeDef      *)JL_PLED_BASE)

//............. 0x6200 - 0x62ff............ for lcd
typedef struct {
    __RW __u32 CON0;
    __RW __u32 CON1;
    __RW __u32 SEG_IO_EN0;
    __RW __u32 SEG_IO_EN1;
    __RW __u32 SEG0_DAT;
    __RW __u32 SEG1_DAT;
    __RW __u32 SEG2_DAT;
    __RW __u32 SEG3_DAT;
    __RW __u32 SEG4_DAT;
    __RW __u32 SEG5_DAT;
} JL_LCDC_TypeDef;

#define JL_LCDC_BASE                   (ls_base + map_adr(0x62, 0x00))
#define JL_LCDC                        ((JL_LCDC_TypeDef      *)JL_LCDC_BASE)

#include "io_omap.h"
#include "io_imap.h"

//===============================================================================//
//
//      husb sfr address define
//
//===============================================================================//

#define husb_base(i)    (0xfc0000 + 0x1000*i)

#define husb_phy_com_sfr_ptr(num)   (*(volatile u32 *)(husb_base(0) + num*4))

#define HUSB_PLL_CON0                   husb_phy_com_sfr_ptr(0)
#define HUSB_PLL_CON1                   husb_phy_com_sfr_ptr(1)
#define HUSB_PLL_CON2                   husb_phy_com_sfr_ptr(2)
#define HUSB_PLL_CON3                   husb_phy_com_sfr_ptr(3)
#define HUSB_PLL_CON4                   husb_phy_com_sfr_ptr(4)
#define HUSB_PLL_CON5                   husb_phy_com_sfr_ptr(5)
#define HUSB_PLL_CON6                   husb_phy_com_sfr_ptr(6)
#define HUSB_PLL_NR                     husb_phy_com_sfr_ptr(7)
#define HUSB_PLL_INTF0                  husb_phy_com_sfr_ptr(8)
#define HUSB_PLL_INTF1                  husb_phy_com_sfr_ptr(9)
#define HUSB_PHY0_CON0                  husb_phy_com_sfr_ptr(10)
#define HUSB_PHY0_CON1                  husb_phy_com_sfr_ptr(11)
#define HUSB_PHY0_CON2                  husb_phy_com_sfr_ptr(12)
#define HUSB_PHY0_CON3                  husb_phy_com_sfr_ptr(13)
#define HUSB_PHY0_READ                  husb_phy_com_sfr_ptr(14)
#define HUSB_PHY0_TSCON                 husb_phy_com_sfr_ptr(15)
#define HUSB_PHY0_TSCRC                 husb_phy_com_sfr_ptr(16)
#define HUSB_TRIM_CON0                  husb_phy_com_sfr_ptr(17)
#define HUSB_TRIM_CON1                  husb_phy_com_sfr_ptr(18)
#define HUSB_TRIM_PND                   husb_phy_com_sfr_ptr(19)
#define HUSB_FRQ_CNT                    husb_phy_com_sfr_ptr(20)
#define HUSB_FRQ_SCA                    husb_phy_com_sfr_ptr(21)

#define husb_sie_sfr_ptr(num)   (husb_base(1) + num)

#define H0_FADDR                        (*(volatile u8  *)husb_sie_sfr_ptr(0x000))
#define H0_POWER                        (*(volatile u8  *)husb_sie_sfr_ptr(0x001))
#define H0_INTRTX                       (*(volatile u16 *)husb_sie_sfr_ptr(0x002))
#define H0_INTRRX                       (*(volatile u16 *)husb_sie_sfr_ptr(0x004))
#define H0_INTRTXE                      (*(volatile u16 *)husb_sie_sfr_ptr(0x006))
#define H0_INTRRXE                      (*(volatile u16 *)husb_sie_sfr_ptr(0x008))
#define H0_INTRUSB                      (*(volatile u8  *)husb_sie_sfr_ptr(0x00a))
#define H0_INTRUSBE                     (*(volatile u8  *)husb_sie_sfr_ptr(0x00b))
#define H0_FRAME                        (*(volatile u16 *)husb_sie_sfr_ptr(0x00c))
#define H0_INDEX                        (*(volatile u8  *)husb_sie_sfr_ptr(0x00e))
#define H0_TESTMODE                     (*(volatile u8  *)husb_sie_sfr_ptr(0x00f))
#define H0_FIFO0                        (*(volatile u8  *)husb_sie_sfr_ptr(0x020))
#define H0_FIFO1                        (*(volatile u8  *)husb_sie_sfr_ptr(0x024))
#define H0_FIFO2                        (*(volatile u8  *)husb_sie_sfr_ptr(0x028))
#define H0_FIFO3                        (*(volatile u8  *)husb_sie_sfr_ptr(0x02c))
#define H0_FIFO4                        (*(volatile u8  *)husb_sie_sfr_ptr(0x030))
#define H0_DEVCTL                       (*(volatile u8  *)husb_sie_sfr_ptr(0x060))

#define H0_EP0_TXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x080)))
#define H0_EP0_TXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x082)))
#define H0_EP0_TXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x083)))
#define H0_EP0_RXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x084)))
#define H0_EP0_RXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x086)))
#define H0_EP0_RXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x087)))

#define H0_EP1_TXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x088)))
#define H0_EP1_TXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x08a)))
#define H0_EP1_TXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x08b)))
#define H0_EP1_RXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x08c)))
#define H0_EP1_RXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x08e)))
#define H0_EP1_RXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x08f)))

#define H0_EP2_TXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x090)))
#define H0_EP2_TXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x092)))
#define H0_EP2_TXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x093)))
#define H0_EP2_RXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x094)))
#define H0_EP2_RXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x096)))
#define H0_EP2_RXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x097)))

#define H0_EP3_TXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x098)))
#define H0_EP3_TXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x09a)))
#define H0_EP3_TXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x09b)))
#define H0_EP3_RXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x09c)))
#define H0_EP3_RXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x09e)))
#define H0_EP3_RXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x09f)))

#define H0_EP4_TXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x0a0)))
#define H0_EP4_TXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0a2)))
#define H0_EP4_TXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0a3)))
#define H0_EP4_RXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x0a4)))
#define H0_EP4_RXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0a6)))
#define H0_EP4_RXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0a7)))

#define H0_EP5_TXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x0a8)))
#define H0_EP5_TXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0aa)))
#define H0_EP5_TXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0ab)))
#define H0_EP5_RXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x0ac)))
#define H0_EP5_RXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0ae)))
#define H0_EP5_RXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0af)))

#define H0_EP6_TXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x0b0)))
#define H0_EP6_TXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0b2)))
#define H0_EP6_TXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0b3)))
#define H0_EP6_RXFUNCADDR               (*(volatile u8  *)(husb_sie_sfr_ptr(0x0b4)))
#define H0_EP6_RXHUBADDR                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0b6)))
#define H0_EP6_RXHUBPORT                (*(volatile u8  *)(husb_sie_sfr_ptr(0x0b7)))

#define H0_EP0TXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x100))
#define H0_CSR0                         (*(volatile u16 *)husb_sie_sfr_ptr(0x102))
#define H0_COUNT0                       (*(volatile u16 *)husb_sie_sfr_ptr(0x108))
#define H0_NAKLIMIT0                    (*(volatile u8  *)husb_sie_sfr_ptr(0x10b))
#define H0_CFGDATA                      (*(volatile u8  *)husb_sie_sfr_ptr(0x10f))
#define H0_EP1TXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x110))
#define H0_EP1TXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x112))
#define H0_EP1RXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x114))
#define H0_EP1RXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x116))
#define H0_EP1RXCOUNT                   (*(volatile u16 *)husb_sie_sfr_ptr(0x118))
#define H0_EP1TXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x11a))
#define H0_EP1TXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x11b))
#define H0_EP1RXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x11c))
#define H0_EP1RXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x11d))
#define H0_EP1FIFOSIZE                  (*(volatile u8  *)husb_sie_sfr_ptr(0x11f))
#define H0_EP2TXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x120))
#define H0_EP2TXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x122))
#define H0_EP2RXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x124))
#define H0_EP2RXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x126))
#define H0_EP2RXCOUNT                   (*(volatile u16 *)husb_sie_sfr_ptr(0x128))
#define H0_EP2TXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x12a))
#define H0_EP2TXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x12b))
#define H0_EP2RXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x12c))
#define H0_EP2RXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x12d))
#define H0_EP2FIFOSIZE                  (*(volatile u8  *)husb_sie_sfr_ptr(0x12f))
#define H0_EP3TXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x130))
#define H0_EP3TXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x132))
#define H0_EP3RXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x134))
#define H0_EP3RXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x136))
#define H0_EP3RXCOUNT                   (*(volatile u16 *)husb_sie_sfr_ptr(0x138))
#define H0_EP3TXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x13a))
#define H0_EP3TXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x13b))
#define H0_EP3RXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x13c))
#define H0_EP3RXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x13d))
#define H0_EP3FIFOSIZE                  (*(volatile u8  *)husb_sie_sfr_ptr(0x13f))
#define H0_EP4TXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x140))
#define H0_EP4TXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x142))
#define H0_EP4RXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x144))
#define H0_EP4RXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x146))
#define H0_EP4RXCOUNT                   (*(volatile u16 *)husb_sie_sfr_ptr(0x148))
#define H0_EP4TXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x14a))
#define H0_EP4TXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x14b))
#define H0_EP4RXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x14c))
#define H0_EP4RXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x14d))
#define H0_EP4FIFOSIZE                  (*(volatile u8  *)husb_sie_sfr_ptr(0x14f))
#define H0_EP5TXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x150))
#define H0_EP5TXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x152))
#define H0_EP5RXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x154))
#define H0_EP5RXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x156))
#define H0_EP5RXCOUNT                   (*(volatile u16 *)husb_sie_sfr_ptr(0x158))
#define H0_EP5TXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x15a))
#define H0_EP5TXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x15b))
#define H0_EP5RXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x15c))
#define H0_EP5RXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x15d))
#define H0_EP5FIFOSIZE                  (*(volatile u8  *)husb_sie_sfr_ptr(0x15f))
#define H0_EP6TXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x160))
#define H0_EP6TXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x162))
#define H0_EP6RXMAXP                    (*(volatile u16 *)husb_sie_sfr_ptr(0x164))
#define H0_EP6RXCSR                     (*(volatile u16 *)husb_sie_sfr_ptr(0x166))
#define H0_EP6RXCOUNT                   (*(volatile u16 *)husb_sie_sfr_ptr(0x168))
#define H0_EP6TXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x16a))
#define H0_EP6TXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x16b))
#define H0_EP6RXTYPE                    (*(volatile u8  *)husb_sie_sfr_ptr(0x16c))
#define H0_EP6RXINTERVAL                (*(volatile u8  *)husb_sie_sfr_ptr(0x16d))
#define H0_EP6FIFOSIZE                  (*(volatile u8  *)husb_sie_sfr_ptr(0x16f))
#define H0_RX_DPKTDIS                   (*(volatile u16 *)husb_sie_sfr_ptr(0x340))
#define H0_TX_DPKTDIS                   (*(volatile u16 *)husb_sie_sfr_ptr(0x342))
#define H0_C_T_UCH                      (*(volatile u16 *)husb_sie_sfr_ptr(0x344))



#define husb_ctl_sfr_ptr(num)   (*(volatile u32 *)(husb_base(2) + num*4))

#define H0_SIE_CON                      husb_ctl_sfr_ptr(0x00)
#define H0_EP0_CNT                      husb_ctl_sfr_ptr(0x01)
#define H0_EP1_CNT                      husb_ctl_sfr_ptr(0x02)
#define H0_EP2_CNT                      husb_ctl_sfr_ptr(0x03)
#define H0_EP3_CNT                      husb_ctl_sfr_ptr(0x04)
#define H0_EP4_CNT                      husb_ctl_sfr_ptr(0x05)
#define H0_EP5_CNT                      husb_ctl_sfr_ptr(0x06)
#define H0_EP6_CNT                      husb_ctl_sfr_ptr(0x07)
#define H0_EP1_TADR                     husb_ctl_sfr_ptr(0x08)
#define H0_EP1_RADR                     husb_ctl_sfr_ptr(0x09)
#define H0_EP2_TADR                     husb_ctl_sfr_ptr(0x0a)
#define H0_EP2_RADR                     husb_ctl_sfr_ptr(0x0b)
#define H0_EP3_TADR                     husb_ctl_sfr_ptr(0x0c)
#define H0_EP3_RADR                     husb_ctl_sfr_ptr(0x0d)
#define H0_EP4_TADR                     husb_ctl_sfr_ptr(0x0e)
#define H0_EP4_RADR                     husb_ctl_sfr_ptr(0x0f)
#define H0_EP5_TADR                     husb_ctl_sfr_ptr(0x10)
#define H0_EP5_RADR                     husb_ctl_sfr_ptr(0x11)
#define H0_EP6_TADR                     husb_ctl_sfr_ptr(0x12)
#define H0_EP6_RADR                     husb_ctl_sfr_ptr(0x13)
#define H0_EP3_ISO_CON                  husb_ctl_sfr_ptr(0x14)
#define H0_EP4_ISO_CON                  husb_ctl_sfr_ptr(0x15)


#endif
