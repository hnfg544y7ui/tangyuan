#ifndef __UART_COMM_H__
#define __UART_COMM_H__

#include "system/includes.h"

/**
 * @brief Initialize UART communication module.
 * @return 0 if success, negative value on error.
 */
int uart_comm_init(void);

/**
 * @brief Send data through UART.
 * @param data Pointer to data buffer.
 * @param len Length of data to send.
 * @return Number of bytes sent, negative value on error.
 */
int uart_comm_send(const u8 *data, u32 len);

/**
 * @brief Receive data from UART.
 * @param buffer Pointer to receive buffer.
 * @param len Maximum length to receive.
 * @param timeout_ms Timeout in milliseconds (0 = wait forever).
 * @return Number of bytes received, negative value on error.
 */
int uart_comm_recv(u8 *buffer, u32 len, u32 timeout_ms);

/**
 * @brief Get available receive data length.
 * @return Number of bytes available in receive buffer.
 */
int uart_comm_get_recv_len(void);

/**
 * @brief Deinitialize UART communication.
 * @return 0 if success, negative value on error.
 */
int uart_comm_deinit(void);

#endif
