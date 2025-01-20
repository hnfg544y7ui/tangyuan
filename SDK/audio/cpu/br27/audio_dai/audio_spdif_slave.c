#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".audio_spdif_slave.data.bss")
#pragma data_seg(".audio_spdif_slave.data")
#pragma const_seg(".audio_spdif_slave.text.const")
#pragma code_seg(".audio_spdif_slave.text")
#endif
#include "includes.h"
#include "asm/includes.h"
#include "media/includes.h"
#include "system/includes.h"
#include "audio_spdif_slave.h"

#define SPDIF_DEBUG_INFO
#ifdef SPDIF_DEBUG_INFO
#define spdif_printf  r_printf
#else
#define spdif_printf(...)
#endif

#define SPDIF_SLAVE_TEST		0
static SPDIF_SLAVE_PARM *spdif_slave_parm = NULL;

static u32 PFI_SPDIF_DAT[] = {
    PFI_SPDIF_DIA,
    PFI_SPDIF_DIB,
    PFI_SPDIF_DIC,
    PFI_SPDIF_DID,
};

static u8 spdif_analog_port[] = {
    IO_PORTA_06,
    IO_PORTA_08,
    IO_PORTC_00,
    IO_PORTC_02,
};

static void spdif_slave_info_dump()
{
    spdif_printf("JL_SPDIF->SS_CON = 0x%x\n", JL_SPDIF->SS_CON);
    spdif_printf("JL_SPDIF->SS_IO_CON = 0x%x\n", JL_SPDIF->SS_IO_CON);
    spdif_printf("JL_SPDIF->SS_DMA_CON = 0x%x\n", JL_SPDIF->SS_DMA_CON);
    spdif_printf("JL_SPDIF->SS_DMA_LEN = 0x%x\n", JL_SPDIF->SS_DMA_LEN);
    spdif_printf("JL_SPDIF->SS_DAT_ADR = 0x%x\n", JL_SPDIF->SS_DAT_ADR);
    spdif_printf("JL_SPDIF->SS_INF_ADR = 0x%x\n", JL_SPDIF->SS_INF_ADR);
    spdif_printf("JL_CLOCK->CLK_CON5[10:8] = 0x%x\n", (JL_CLOCK->CLK_CON5 >> 8) & 0b111);
    spdif_printf("JL_IMAP->FI_SPDIF_DIA = 0x%x\n", JL_IMAP->FI_SPDIF_DIA);
}

sec(.volatile_ram_code)
static void *spdif_slave_data_addr()
{
    u8 buf_index = 0;
    u8 *buf_addr = NULL;

    buf_addr = (u8 *)(spdif_slave_parm->data_buf);
    buf_index = (JL_SPDIF->SS_CON & BIT(12)) ? 0 : 1;
    buf_addr = buf_addr + ((spdif_slave_parm->data_dma_len / 2) * buf_index);

    return buf_addr;
}

sec(.volatile_ram_code)
static void *spdif_slave_inf_addr()
{
    u8 buf_index = 0;
    u8 *buf_addr = NULL;

    buf_addr = (u8 *)(spdif_slave_parm->inf_buf);
    buf_index = (JL_SPDIF->SS_CON & BIT(13)) ? 0 : 1;
    buf_addr = buf_addr + ((spdif_slave_parm->inf_dma_len / 2) * buf_index);

    return buf_addr;
}

u32 spdif_slave_get_sr()
{
    u32 lsb_clk;
    lsb_clk = clk_get("lsb");

    u32 spdif_sr_cnt;
    spdif_sr_cnt = JL_SPDIF->SS_SR_CNT;

    if (!spdif_sr_cnt) {
        return 0;
    }

    u64 spdif_slave_sr;
    if (spdif_slave_parm->data_mode) {
        spdif_slave_sr = (u64)lsb_clk * (spdif_slave_parm->data_dma_len / 8) / spdif_sr_cnt;
    } else {
        spdif_slave_sr = (u64)lsb_clk * (spdif_slave_parm->data_dma_len / 16) / spdif_sr_cnt;
    }

    /* spdif_printf("spdif_slave_sr = %d\n", spdif_slave_sr); */
    return (u32)spdif_slave_sr;
}

___interrupt
static void spdif_slave_isr()
{
    if (ERROR_VALUE()) {
        u8 is_d_err = 0;
        if (F_ERR()) {
            /* putchar('F'); */
        }
        if (B_ERR()) {
            /* putchar('B'); */
        }
        if (D_ERR()) {
            is_d_err = 1;
            /* putchar('D'); */
        }
        if (RL_ERR()) {
            /* putchar('R'); */
        }

        ERR_ALL_CLR();
        if (!is_d_err) {
            /* SS_START(1); */
        }
    }

    if (INFO_PND()) {
        INFO_PND_CLR();
        /* putchar('A'); */
        s8 *inf_buf_addr = NULL;
        inf_buf_addr = spdif_slave_inf_addr();
        if (spdif_slave_parm->inf_isr_cb) {
            spdif_slave_parm->inf_isr_cb(inf_buf_addr, spdif_slave_parm->inf_dma_len / 2);
        }
    }

    if (DATA_PND()) {
        DATA_PND_CLR();
        /* putchar('a'); */
        s8 *data_buf_addr = NULL;
        data_buf_addr = spdif_slave_data_addr();
        if (spdif_slave_parm->data_isr_cb) {
            spdif_slave_parm->data_isr_cb(data_buf_addr, spdif_slave_parm->data_dma_len / 2);
        }
    }


    if (CSBR_PND()) {
        CSBR_PND_CLR();
    }

    if (IS_PND()) {
        /* putchar('i'); */
        IS_PND_CLR();
    }

    if (ET_PND()) {
        /* putchar('e'); */
        ET_PND_CLR();
    }
}

static void spdif_slave_digital_io_init(u8 gpio)
{
    gpio_set_mode(IO_PORT_SPILT(gpio), PORT_INPUT_FLOATING);
}

static void spdif_slave_analog_io_init(u8 port)
{
    gpio_set_mode(IO_PORT_SPILT(port), PORT_HIGHZ);
}

void *spdif_slave_channel_init(void *hw_spdif_slave, u8 ch_idx)
{
    if (hw_spdif_slave == NULL) {
        return NULL;
    }
    SPDIF_SLAVE_PARM *spdif_slave_hdl = (SPDIF_SLAVE_PARM *)hw_spdif_slave;
    IS_PND_CLR();
    ET_PND_CLR();
    IS_PND_IE(0);
    ET_PND_IE(0);
    IO_IN_EN(0);

    u8 port_sel = spdif_slave_hdl->ch_cfg[ch_idx].port_sel;

    if (port_sel <= SPDIF_IN_DIGITAL_PORT_D) {
        IO_IS_DEN(ch_idx, spdif_slave_hdl->ch_cfg[ch_idx].is_den);
        IO_ET_DEN(ch_idx, spdif_slave_hdl->ch_cfg[ch_idx].et_den);

        IO_IN_SEL(ch_idx);
        spdif_slave_digital_io_init(spdif_slave_hdl->ch_cfg[ch_idx].data_io);
        gpio_set_fun_input_port(spdif_slave_hdl->ch_cfg[ch_idx].data_io, PFI_SPDIF_DAT[ch_idx]);

    } else if (port_sel >= SPDIF_IN_ANALOG_PORT_A && port_sel <= SPDIF_IN_ANALOG_PORT_D) {
        port_sel = port_sel - SPDIF_IN_ANALOG_PORT_A;
        spdif_slave_analog_io_init(spdif_analog_port[port_sel]);

        IO_IS_DEN(0, spdif_slave_hdl->ch_cfg[ch_idx].is_den);
        IO_ET_DEN(0, spdif_slave_hdl->ch_cfg[ch_idx].et_den);

        IO_IN_ANA_SEL(port_sel);
        IO_IN_ANA_EN(1);
        IO_IN_ANA_MUX(1);

        IO_IN_SEL(0);
    }

    return &spdif_slave_hdl->ch_cfg[ch_idx];
}

int spdif_slave_channel_open(void *hw_spdif_slave_channel)
{
    if (hw_spdif_slave_channel == NULL) {
        return -1;
    }
    struct spdif_s_ch_cfg *spdif_slave_channel_hdl = (struct spdif_s_ch_cfg *)hw_spdif_slave_channel;

    IS_PND_CLR();
    ET_PND_CLR();
    IS_PND_IE(spdif_slave_channel_hdl->is_den);
    ET_PND_IE(spdif_slave_channel_hdl->et_den);
    IO_IN_EN(1);

    return 0;
}

int spdif_slave_channel_close(void *hw_spdif_slave_channel)
{
    if (hw_spdif_slave_channel == NULL) {
        return -1;
    }
    struct spdif_s_ch_cfg *spdif_slave_channel_hdl = (struct spdif_s_ch_cfg *)hw_spdif_slave_channel;

    IS_PND_CLR();
    ET_PND_CLR();
    IS_PND_IE(0);
    ET_PND_IE(0);
    IO_IN_EN(0);
    return 0;
}


void *spdif_slave_init(void *hw_spdif_slave)
{
    if (hw_spdif_slave == NULL) {
        return NULL;
    }
    spdif_slave_parm = hw_spdif_slave;

    SS_RESET();													//reset all con

    /* DATA_DMA_LEN(spdif_slave_parm->data_dma_len / 8); */
    INFO_DMA_LEN(spdif_slave_parm->inf_dma_len / 8);

    if (spdif_slave_parm->data_mode == SPDIF_S_DAT_24BIT) {
        DATA_DMA_LEN(spdif_slave_parm->data_dma_len / 16);
        spdif_slave_parm->data_buf = dma_malloc(spdif_slave_parm->data_dma_len);
    } else if (spdif_slave_parm->data_mode == SPDIF_S_DAT_16BIT) {
        DATA_DMA_LEN(spdif_slave_parm->data_dma_len / 8);
        spdif_slave_parm->data_buf = dma_malloc(spdif_slave_parm->data_dma_len);
    }
    memset(spdif_slave_parm->data_buf, 0, spdif_slave_parm->data_dma_len);
    DATA_DMA_MD(spdif_slave_parm->data_mode);

    spdif_slave_parm->inf_buf = dma_malloc(spdif_slave_parm->inf_dma_len);
    memset(spdif_slave_parm->inf_buf, 0, spdif_slave_parm->inf_dma_len);

    spdif_printf("data_buf = 0x%x, inf_buf = 0x%x\n", spdif_slave_parm->data_buf, spdif_slave_parm->inf_buf);

    if (spdif_slave_parm->data_buf) {
        JL_SPDIF->SS_DAT_ADR = (volatile unsigned int)spdif_slave_parm->data_buf;
    }

    if (spdif_slave_parm->inf_buf) {
        JL_SPDIF->SS_INF_ADR = (volatile unsigned int)spdif_slave_parm->inf_buf;
    }

    clk_set_api("spdif_slave", spdif_slave_parm->rx_clk);

    INFO_PND_IE(1);
    DATA_PND_IE(1);
    CSBR_PND_IE(1);

    F_ERR_IE(1);
    B_ERR_IE(1);
    D_ERR_IE(0);
    SE_ERR_IE(1);
    RLE_DECT_EN(1);

    DATA_PND_CLR();
    INFO_PND_CLR();
    ERR_ALL_CLR();
    IS_PND_CLR();
    ET_PND_CLR();
    CSBR_PND_CLR();

    RLE_CNT_SET(0xffff);
    SS_DET_MODE(1);
    FTL_TRACE(3);
    FTL_SEL(1);

    request_irq(IRQ_SPDIF_S_IDX, 3, spdif_slave_isr, 0);

    return spdif_slave_parm;
}

int spdif_slave_start(void *hw_spdif_slave)
{
    if (hw_spdif_slave == NULL) {
        return 0;
    }
    SPDIF_SLAVE_PARM *spdif_slave_hdl = (SPDIF_SLAVE_PARM *)hw_spdif_slave;
    if (spdif_slave_hdl->data_buf) {
        DATA_DMA_EN(1);
    }
    if (spdif_slave_hdl->inf_buf) {
        INFO_DMA_EN(1);
    }
    SS_EN(1);
    SS_START(1);
    spdif_printf("spdif_start\n");
    spdif_slave_info_dump();
    return 1;
}

int spdif_slave_stop(void *hw_spdif_slave)
{
    if (hw_spdif_slave == NULL) {
        return -1;
    }
    SPDIF_SLAVE_PARM *spdif_slave_hdl = (SPDIF_SLAVE_PARM *)hw_spdif_slave;
    DATA_PND_CLR();
    INFO_PND_CLR();
    ERR_ALL_CLR();
    IS_PND_CLR();
    ET_PND_CLR();
    CSBR_PND_CLR();
    SS_EN(0);
    return 0;
}


int spdif_slave_uninit(void *hw_spdif_slave)
{
    if (hw_spdif_slave == NULL) {
        return -1;
    }
    SPDIF_SLAVE_PARM *spdif_slave_hdl = (SPDIF_SLAVE_PARM *)hw_spdif_slave;
    spdif_slave_stop(spdif_slave_hdl);

    INFO_PND_IE(0);
    DATA_PND_IE(0);
    CSBR_PND_IE(0);
    F_ERR_IE(0);
    B_ERR_IE(0);
    D_ERR_IE(0);
    SE_ERR_IE(0);
    RLE_DECT_EN(0);

    DATA_PND_CLR();
    INFO_PND_CLR();
    ERR_ALL_CLR();
    IS_PND_CLR();
    ET_PND_CLR();
    CSBR_PND_CLR();

    if (spdif_slave_hdl->data_buf) {
        dma_free(spdif_slave_hdl->data_buf);
        spdif_slave_hdl->data_buf = NULL;
    }

    if (spdif_slave_hdl->inf_buf) {
        dma_free(spdif_slave_hdl->inf_buf);
        spdif_slave_hdl->inf_buf = NULL;
    }
    spdif_slave_hdl = NULL;
    spdif_printf("spdif_slave_exit OK\n");
    return 0;
}

SPDIF_DAT_FORMAT spdif_slave_get_data_format()
{
    u8 ss_csb0 = (JL_SPDIF->SS_CSB0 & 0x3f);
    if (ss_csb0 & 0x2) {
        return SPDIF_S_DAT_NOT_PCM;
    } else {
        return SPDIF_S_DAT_PCM;
    }
    return SPDIF_S_DAT_UNKNOWN;
}


