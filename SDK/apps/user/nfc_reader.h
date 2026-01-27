#ifndef __NFC_READER_H__
#define __NFC_READER_H__

#include "system/includes.h"

#define NFC_STX     0x34
#define NFC_ETX     0x35

#define NFC_CMD_GET_UID         0x20
#define NFC_CMD_READ_BLOCK      0x21
#define NFC_CMD_WRITE_BLOCK     0x22
#define NFC_CMD_WALLET_INIT     0x28
#define NFC_CMD_WALLET_CHARGE   0x29
#define NFC_CMD_WALLET_DEDUCT   0x2A

#define NFC_STATE_SUCCESS       0x00
#define NFC_STATE_PARAM_ERROR   0x01
#define NFC_STATE_NO_CARD       0x10
#define NFC_STATE_READ_FAIL     0x11
#define NFC_STATE_WRITE_FAIL    0x12
#define NFC_STATE_AUTH_FAIL     0x13

#define NFC_CARD_TYPE_M1        0x00
#define NFC_CARD_TYPE_CPU       0x10

/**
 * @brief Aroma diffuser data structure.
 */
struct aroma_data {
    u8 type_id;              // Aroma type ID
    u8 aroma_id[6];          // Aroma 0-5 ID numbers
    u8 aroma_used_time[6];   // Aroma 0-5 used time
};

typedef void (*nfc_card_event_callback_t)(u8 present, const struct aroma_data *data);

/**
 * @brief Initialize NFC reader module.
 * @return 0 if success, negative value on error.
 */
int nfc_reader_init(void);

/**
 * @brief Set NFC card event callback.
 * @param t_callback Callback for card present/removal events.
 */
void nfc_set_card_event_callback(nfc_card_event_callback_t t_callback);

/**
 * @brief Get card UID.
 * @param uid Buffer to store UID (5 bytes).
 * @return 0 if success, negative value on error.
 */
int nfc_get_uid(u8 *uid);

/**
 * @brief Read block from card.
 * @param block Block address.
 * @param key_type Key type (0=A, 1=B).
 * @param key Key buffer (6 bytes).
 * @param data Buffer to store read data (16 bytes).
 * @return 0 if success, negative value on error.
 */
int nfc_read_block(u8 block, u8 key_type, const u8 *key, u8 *data);

/**
 * @brief Write block to card.
 * @param block Block address.
 * @param key_type Key type (0=A, 1=B).
 * @param key Key buffer (6 bytes).
 * @param data Data to write (16 bytes).
 * @return 0 if success, negative value on error.
 */
int nfc_write_block(u8 block, u8 key_type, const u8 *key, const u8 *data);

/**
 * @brief Initialize NFC card keys.
 * @param sector Sector number (0-15).
 * @return 0 if success, negative value on error.
 */
int nfc_init_keys(u8 sector);

/**
 * @brief Read aroma data from NFC card.
 * @param data Pointer to aroma_data structure.
 * @return 0 if success, negative value on error.
 */
int nfc_read_aroma_data(struct aroma_data *data);

/**
 * @brief Write aroma data to NFC card.
 * @param data Pointer to aroma_data structure.
 * @return 0 if success, negative value on error.
 */
int nfc_write_aroma_data(const struct aroma_data *data);

#endif
