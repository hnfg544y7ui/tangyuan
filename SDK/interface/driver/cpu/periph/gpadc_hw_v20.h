#ifndef  __GPADC_V20_H__
#define  __GPADC_V20_H__
//适用于 br27

//ADC_CON reg
#define GPADC_CON_ADC_DEN   31
// #define GPADC_CON_RESERVED  27 //bit27~bit30
#define GPADC_CON_ADC_ISEL  26
#define GPADC_CON_ADC_TESTSEL       23
#define GPADC_CON_ADC_TESTSEL_      3
#define GPADC_CON_ADC_CHL   19
#define GPADC_CON_ADC_CHL_  4
#define GPADC_CON_WAIT_TIME     15
#define GPADC_CON_WAIT_TIME_    4
#define GPADC_CON_ADC_BAUD  8
#define GPADC_CON_ADC_BAUD_ 7
#define GPADC_CON_PND   7
#define GPADC_CON_CPND  6
// #define GPADC_CON_RESERVED  5 //bit5
#define GPADC_CON_DMA_CHL_CTL   4
#define GPADC_CON_TEST_CHL_EN   3
#define GPADC_CON_IO_CHL_EN     2
#define GPADC_CON_IO_TEST_CHL_EN_    2 //这同时控制 IO_CHL_EN 和 TEST_CHL_EN
#define GPADC_CON_IE    1
#define GPADC_CON_ADC_AEN   0


//DMA_CON0 reg
#define GPADC_DMA_CON0_DMA_LEN      16
#define GPADC_DMA_CON0_DMA_LEN_     16
#define GPADC_DMA_CON0_BUF_CLR      9
#define GPADC_DMA_CON0_BUF_FLAG     8
#define GPADC_DMA_CON0_DMA_PND      7
#define GPADC_DMA_CON0_CLR_DMA_PND  6
#define GPADC_DMA_CON0_RESERVED     1 //bit1~bit5
#define GPADC_DMA_CON0_DMA_IE       0


//DMA_CON1 reg
#define GPADC_DMA_CON1_SAMPLE_RETE      17
#define GPADC_DMA_CON1_SAMPLE_RETE_     15
#define GPADC_DMA_CON1_DMA_DAT_LOG      16
#define GPADC_DMA_CON1_DMA_CHL_EN       0
#define GPADC_DMA_CON1_DMA_CHL_EN_      16


//DMA_CON2 reg
#define GPADC_DMA_CON2_RESERVED     16 //bit16~bit31
#define GPADC_DMA_CON2_DMA_HWPTR    0
#define GPADC_DMA_CON2_DMA_HWPTR_   16


//DMA_BADR reg
#define GPADC_DMA_BADR      0
#define GPADC_DMA_BADR_     32


#endif

