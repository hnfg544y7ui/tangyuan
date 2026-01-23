#include "uart_comm.h"
#include "uart.h"
#include "gpio.h"
#include "system/malloc.h"

#define UART_COMM_DEBUG_ENABLE  0
#if UART_COMM_DEBUG_ENABLE
#define uart_comm_debug(fmt, ...) printf("[UART_COMM] "fmt, ##__VA_ARGS__)
#else
#define uart_comm_debug(...)
#endif

#define UART_RX_BUF_SIZE    1024

static s32 g_uart_dev = -1;
static void *g_uart_rx_buf = NULL;

/**
 * @brief UART event callback handler.
 * @param uart_num UART device number.
 * @param event UART event type.
 */
static void uart_comm_event_callback(uart_dev uart_num, enum uart_event event)
{
    if (event & UART_EVENT_RX_DATA) {
        int len = uart_get_recv_len(uart_num);
        uart_comm_debug("UART RX: %d bytes\n", len);
    }
    
    if (event & UART_EVENT_RX_TIMEOUT) {
        uart_comm_debug("UART RX timeout\n");
    }
    
    if (event & UART_EVENT_RX_FIFO_OVF) {
        uart_comm_debug("UART RX overflow\n");
        uart_dma_rx_reset(uart_num);
    }
    
    if (event & UART_EVENT_TX_DONE) {
        uart_comm_debug("UART TX done\n");
    }
}

/**
 * @brief UART receive task.
 * @param p Task parameter (unused).
 */
static void uart_comm_recv_task(void *p)
{
    u8 *buf = dma_malloc(256);
    int len;
    
    if (!buf) {
        uart_comm_debug("UART recv task buffer malloc failed\n");
    }
    
    while (1) {
        len = uart_get_recv_len(g_uart_dev);
        if (len > 0) {
            if (len > 256) {
                len = 256;
            }
            
            len = uart_recv_bytes(g_uart_dev, buf, len);
            if (len > 0) {
                // Data handled by NFC module
            }
        }
        
        os_time_dly(5);
    }
}

/**
 * @brief Initialize UART communication module.
 * @return 0 if success, negative value on error.
 */
int uart_comm_init(void)
{
    struct uart_config uart_cfg;
    struct uart_dma_config dma_cfg;
    int ret;
    
    s32 uart_num = 1;
    u32 baud_rate = 9600;
    u16 tx_pin = IO_PORTC_09;
    u16 rx_pin = IO_PORTC_10;
    
    g_uart_rx_buf = dma_malloc(UART_RX_BUF_SIZE);
    if (!g_uart_rx_buf) {
        uart_comm_debug("UART rx buffer malloc failed\n");
        return -1;
    }
    
    uart_cfg.baud_rate = baud_rate;
    uart_cfg.tx_pin = tx_pin;
    uart_cfg.rx_pin = rx_pin;
    uart_cfg.parity = UART_PARITY_DISABLE;
    uart_cfg.tx_wait_mutex = 0;
    
    g_uart_dev = uart_init(uart_num, &uart_cfg);
    if (g_uart_dev < 0) {
        uart_comm_debug("UART init failed: %d\n", g_uart_dev);
        dma_free(g_uart_rx_buf);
        g_uart_rx_buf = NULL;
        return g_uart_dev;
    }
    
    memset(&dma_cfg, 0, sizeof(dma_cfg));
    dma_cfg.rx_timeout_thresh = 3 * 10000000 / baud_rate;
    dma_cfg.frame_size = UART_RX_BUF_SIZE;
    dma_cfg.rx_cbuffer_size = UART_RX_BUF_SIZE;
    dma_cfg.rx_cbuffer = g_uart_rx_buf;
    dma_cfg.irq_callback = uart_comm_event_callback;
    dma_cfg.irq_priority = 3;
    dma_cfg.event_mask = UART_EVENT_RX_DATA | UART_EVENT_RX_TIMEOUT | 
                         UART_EVENT_RX_FIFO_OVF | UART_EVENT_TX_DONE;
    
    ret = uart_dma_init(g_uart_dev, &dma_cfg);
    if (ret != UART_OK) {
        uart_comm_debug("UART DMA init failed: %d\n", ret);
        uart_deinit(g_uart_dev);
        dma_free(g_uart_rx_buf);
        g_uart_rx_buf = NULL;
        g_uart_dev = -1;
        return ret;
    }
    
    uart_comm_debug("UART%d initialized: %d baud, TX=0x%04X, RX=0x%04X\n", 
           g_uart_dev, baud_rate, tx_pin, rx_pin);
    
    uart_dump();
    
    // ret = os_task_create(uart_comm_recv_task,
    //                      NULL,
    //                      3,
    //                      512,
    //                      0,
    //                      "uart_recv");
    
    return 0;
}

/**
 * @brief Send data through UART.
 * @param data Pointer to data buffer.
 * @param len Length of data to send.
 * @return Number of bytes sent, negative value on error.
 */
int uart_comm_send(const u8 *data, u32 len)
{
    if (g_uart_dev < 0) {
        return -1;
    }
    
    return uart_send_blocking(g_uart_dev, data, len, 1000);
}

/**
 * @brief Receive data from UART.
 * @param buffer Pointer to receive buffer.
 * @param len Maximum length to receive.
 * @param timeout_ms Timeout in milliseconds (0 = wait forever).
 * @return Number of bytes received, negative value on error.
 */
int uart_comm_recv(u8 *buffer, u32 len, u32 timeout_ms)
{
    if (g_uart_dev < 0) {
        return -1;
    }
    
    if (timeout_ms == 0) {
        return uart_recv_bytes(g_uart_dev, buffer, len);
    } else {
        return uart_recv_blocking(g_uart_dev, buffer, len, timeout_ms);
    }
}

/**
 * @brief Get available receive data length.
 * @return Number of bytes available in receive buffer.
 */
int uart_comm_get_recv_len(void)
{
    if (g_uart_dev < 0) {
        return -1;
    }
    
    return uart_get_recv_len(g_uart_dev);
}

/**
 * @brief Deinitialize UART communication.
 * @return 0 if success, negative value on error.
 */
int uart_comm_deinit(void)
{
    if (g_uart_dev < 0) {
        return -1;
    }
    
    int ret = uart_deinit(g_uart_dev);
    g_uart_dev = -1;
    
    return ret;
}
