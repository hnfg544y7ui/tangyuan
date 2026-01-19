#include "nfc_reader.h"
#include "uart_comm.h"
#include "system/malloc.h"

#define NFC_FRAME_MAX_LEN   128
#define NFC_TIMEOUT_MS      5000

/**
 * @brief Calculate BCC checksum.
 * @param data Pointer to data buffer.
 * @param len Length of data.
 * @return BCC value.
 */
static u8 nfc_calc_bcc(const u8 *data, u16 len)
{
    u8 bcc = 0;
    for (u16 i = 0; i < len; i++) {
        bcc ^= data[i];
    }
    return ~bcc;
}

/**
 * @brief Send NFC command frame.
 * @param cmd Command code.
 * @param data Data buffer.
 * @param len Data length.
 * @return 0 if success, negative value on error.
 */
static int nfc_send_cmd(u8 cmd, const u8 *data, u8 len)
{
    u8 *frame = dma_malloc(NFC_FRAME_MAX_LEN);
    if (!frame) {
        return -1;
    }
    
    u8 pos = 0;
    frame[pos++] = NFC_STX;
    frame[pos++] = cmd;
    frame[pos++] = len;
    
    if (data && len > 0) {
        memcpy(&frame[pos], data, len);
        pos += len;
    }
    
    u8 bcc_data[64];
    u8 bcc_len = 0;
    bcc_data[bcc_len++] = cmd;
    bcc_data[bcc_len++] = len;
    if (data && len > 0) {
        memcpy(&bcc_data[bcc_len], data, len);
        bcc_len += len;
    }
    
    frame[pos++] = nfc_calc_bcc(bcc_data, bcc_len);
    frame[pos++] = NFC_ETX;
    
    int ret = uart_comm_send(frame, pos);
    
    dma_free(frame);
    
    return ret > 0 ? 0 : -1;
}

/**
 * @brief Receive NFC response frame.
 * @param cmd Expected command code.
 * @param data Buffer to store response data.
 * @param max_len Maximum data length.
 * @param out_len Pointer to store actual data length.
 * @return State code (0=success, other=error).
 */
static int nfc_recv_resp(u8 cmd, u8 *data, u8 max_len, u8 *out_len)
{
    u8 *frame = dma_malloc(NFC_FRAME_MAX_LEN);
    if (!frame) {
        return -1;
    }
    
    u32 wait_count = 0;
    while (uart_comm_get_recv_len() < 6 && wait_count < 100) {
        os_time_dly(5);
        wait_count++;
    }
    
    int ret = uart_comm_recv(frame, NFC_FRAME_MAX_LEN, NFC_TIMEOUT_MS);
    if (ret <= 0) {
        dma_free(frame);
        printf("NFC recv timeout (ret=%d)\n", ret);
        return -1;
    }
    
    // printf("NFC recv %d bytes: ", ret);
    // for (int i = 0; i < ret && i < 32; i++) {
    //     printf("%02X ", frame[i]);
    // }
    // printf("\n");
    
    if (frame[0] != NFC_STX) {
        dma_free(frame);
        printf("NFC invalid STX: 0x%02X\n", frame[0]);
        return -1;
    }
    
    if (frame[ret - 1] != NFC_ETX) {
        dma_free(frame);
        printf("NFC invalid ETX: 0x%02X\n", frame[ret - 1]);
        return -1;
    }
    
    u8 recv_cmd = frame[1];
    u8 state = frame[2];
    u8 len = frame[3];
    
    if (recv_cmd != cmd) {
        dma_free(frame);
        printf("NFC cmd mismatch: 0x%02X != 0x%02X\n", recv_cmd, cmd);
        return -1;
    }
    
    u8 bcc_data[64];
    u8 bcc_len = 0;
    bcc_data[bcc_len++] = recv_cmd;
    bcc_data[bcc_len++] = state;
    bcc_data[bcc_len++] = len;
    
    if (len > 0 && len <= max_len) {
        memcpy(bcc_data + bcc_len, frame + 4, len);
        bcc_len += len;
        
        if (data && out_len) {
            memcpy(data, frame + 4, len);
            *out_len = len;
        }
    }
    
    u8 recv_bcc = frame[4 + len];
    u8 calc_bcc = nfc_calc_bcc(bcc_data, bcc_len);
    
    if (recv_bcc != calc_bcc) {
        dma_free(frame);
        printf("NFC BCC error: 0x%02X != 0x%02X\n", recv_bcc, calc_bcc);
        return -1;
    }
    
    dma_free(frame);
    
    return state;
}

/**
 * @brief NFC card reading task.
 * @param p Task parameter (unused).
 */
static void nfc_card_read_task(void *p)
{
    u8 uid[5];
    int ret;
    
    printf("NFC card read task started\n");
    
    os_time_dly(20);
    
    while (1) {
        ret = nfc_get_uid(uid);
        if (ret == 0) {
            printf("Card UID: ");
            for (int i = 0; i < 4; i++) {
                printf("%02X ", uid[i]);
            }
            printf("(BCC: %02X)\n", uid[4]);
        }
        
        os_time_dly(200);
    }
}

/**
 * @brief Initialize NFC reader module.
 * @return 0 if success, negative value on error.
 */
int nfc_reader_init(void)
{
    printf("NFC reader initialized\n");
    
    int ret = 0;
    
    // ret = os_task_create(nfc_card_read_task,
    //                          NULL,
    //                          4,
    //                          512,
    //                          0,
    //                          "nfc_read");
    
    return ret;
}

/**
 * @brief Get card UID.
 * @param uid Buffer to store UID (5 bytes).
 * @return 0 if success, negative value on error.
 */
int nfc_get_uid(u8 *uid)
{
    if (!uid) {
        return -1;
    }
    
    int ret = nfc_send_cmd(NFC_CMD_GET_UID, NULL, 0);
    if (ret < 0) {
        printf("NFC send get_uid cmd failed\n");
        return ret;
    }
    
    u8 data[16];
    u8 data_len = 0;
    int state = nfc_recv_resp(NFC_CMD_GET_UID, data, sizeof(data), &data_len);
    
    if (state != NFC_STATE_SUCCESS) {
        printf("NFC get_uid failed: state=0x%02X\n", state);
        return -1;
    }
    
    if (data_len != 5) {
        printf("NFC UID len error: %d\n", data_len);
        return -1;
    }
    
    memcpy(uid, data, 5);
    
    printf("NFC UID: ");
    for (int i = 0; i < 5; i++) {
        printf("%02X ", uid[i]);
    }
    printf("\n");
    
    return 0;
}

/**
 * @brief Read block from card.
 * @param block Block address.
 * @param key_type Key type (0=A, 1=B).
 * @param key Key buffer (6 bytes).
 * @param data Buffer to store read data (16 bytes).
 * @return 0 if success, negative value on error.
 */
int nfc_read_block(u8 block, u8 key_type, const u8 *key, u8 *data)
{
    if (!key || !data) {
        return -1;
    }
    
    u8 cmd_data[8];
    cmd_data[0] = block;
    cmd_data[1] = key_type;
    memcpy(&cmd_data[2], key, 6);
    
    int ret = nfc_send_cmd(NFC_CMD_READ_BLOCK, cmd_data, 8);
    if (ret < 0) {
        return ret;
    }
    
    os_time_dly(10);
    
    u8 resp_data[16];
    u8 resp_len = 0;
    int state = nfc_recv_resp(NFC_CMD_READ_BLOCK, resp_data, sizeof(resp_data), &resp_len);
    
    if (state != NFC_STATE_SUCCESS) {
        printf("NFC read block failed: state=0x%02X\n", state);
        return -1;
    }
    
    if (resp_len != 16) {
        printf("NFC read block len error: %d\n", resp_len);
        return -1;
    }
    
    memcpy(data, resp_data, 16);
    
    return 0;
}

/**
 * @brief Write block to card.
 * @param block Block address.
 * @param key_type Key type (0=A, 1=B).
 * @param key Key buffer (6 bytes).
 * @param data Data to write (16 bytes).
 * @return 0 if success, negative value on error.
 */
int nfc_write_block(u8 block, u8 key_type, const u8 *key, const u8 *data)
{
    if (!key || !data) {
        return -1;
    }
    
    u8 cmd_data[24];
    cmd_data[0] = block;
    cmd_data[1] = key_type;
    memcpy(&cmd_data[2], key, 6);
    memcpy(&cmd_data[8], data, 16);
    
    int ret = nfc_send_cmd(NFC_CMD_WRITE_BLOCK, cmd_data, 24);
    if (ret < 0) {
        return ret;
    }
    
    os_time_dly(50);
    
    int state = nfc_recv_resp(NFC_CMD_WRITE_BLOCK, NULL, 0, NULL);
    
    if (state < 0) {
        printf("NFC write block failed: state=0x%08X\n", (u32)state);
        return -1;
    }
    
    if (state != NFC_STATE_SUCCESS) {
        printf("NFC write block failed: state=0x%02X\n", state);
        return -1;
    }
    
    return 0;
}
