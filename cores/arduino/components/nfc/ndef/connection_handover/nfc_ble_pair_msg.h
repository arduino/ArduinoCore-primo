/* Copyright (c) 2010 - 2017, Nordic Semiconductor ASA All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NFC_BLE_PAIR_MSG_H__
#define NFC_BLE_PAIR_MSG_H__

/**@file
 *
 * @defgroup nfc_modules NDEF message modules
 * @ingroup nfc_api
 * @brief Implementation of NDEF messages.
 *
 * @defgroup nfc_ndef_messages Predefined NDEF messages
 * @ingroup nfc_modules
 * @brief Predefined NDEF messages for standard use.
 *
 * @defgroup nfc_ble_pair_msg BLE pairing messages
 * @{
 * @ingroup  nfc_ndef_messages
 *
 * @brief    Generation of NFC NDEF messages used for BLE pairing.
 *
 */

#include <stdint.h>
#include "ble_advdata.h"
#include "sdk_errors.h"

/**
 * @brief Types of BLE pairing message.
 *
 * Use one of these values to choose the type of NDEF BLE pairing message.
 */
typedef enum
{
    NFC_BLE_PAIR_MSG_BLUETOOTH_LE_SHORT, ///< Simplified LE OOB message.
    NFC_BLE_PAIR_MSG_BLUETOOTH_EP_SHORT, ///< Simplified EP OOB message.
    NFC_BLE_PAIR_MSG_FULL                ///< BLE Handover Select Message.
} nfc_ble_pair_type_t;

/** @brief Function for encoding simplified LE OOB messages.
 *
 * This function encodes a simplified LE OOB message into a buffer. The payload of the LE OOB record
 * inside the message can be configured via the advertising data structure.
 *
 * This function was implemented partially according to "Bluetooth Secure Simple Pairing Using NFC"
 * (denotation "NFCForum-AD-BTSSP_1_1" published on 2014-01-09) chapters 3.1, 3.2, 4.3.2,
 * and according to "Supplement to the Bluetooth Core Specification" (Version 5, adoption date:
 * Dec 02 2014).
 *
 * @note To be able to encode the message, a SoftDevice must be enabled and configured.
 *
 * @param[in]       p_le_advdata        Pointer to the BLE advertising data structure for the LE OOB record.
 * @param[out]      p_buf               Pointer to the buffer for the message.
 * @param[in,out]   p_len               Size of the available memory for the message as input.
 *                                      Size of the generated message as output.
 *
 * @retval NRF_SUCCESS                  If the function completed successfully.
 * @retval NRF_ERROR_xxx                If an error occurred.
 */
ret_code_t nfc_ble_simplified_le_oob_msg_encode( ble_advdata_t     const * const p_le_advdata,
                                                 uint8_t                 *       p_buf,
                                                 uint32_t                *       p_len);

/** @brief Function for encoding simplified EP OOB messages.
 *
 * This function encodes a simplified EP OOB message into a buffer. The payload of the EP OOB record
 * inside the message can be configured via the advertising data structure.
 *
 * This function was implemented partially according to "Bluetooth Secure Simple Pairing Using NFC"
 * (denotation "NFCForum-AD-BTSSP_1_1" published on 2014-01-09) chapters 3.1, 3.2, 4.3.1,
 * and according to "Supplement to the Bluetooth Core Specification" (Version 5, adoption date:
 * Dec 02 2014).
 *
 * @note To be able to encode the message, a SoftDevice must be enabled and configured.
 *
 * @param[in]       p_ep_advdata        Pointer to the BLE advertising data structure for the EP OOB record.
 * @param[out]      p_buf               Pointer to the buffer for the message.
 * @param[in,out]   p_len               Size of the available memory for the message as input.
 *                                      Size of the generated message as output.
 *
 * @retval NRF_SUCCESS                  If the function completed successfully.
 * @retval NRF_ERROR_xxx                If an error occurred.
 */
ret_code_t nfc_ble_simplified_ep_oob_msg_encode( ble_advdata_t     const * const p_ep_advdata,
                                                 uint8_t                 *       p_buf,
                                                 uint32_t                *       p_len);

/** @brief Function for encoding BLE Handover Select Messages.
 *
 * This function encodes a BLE Handover Select Message into a buffer. The payload of the LE OOB record
 * and the EP OOB record inside the message can be configured via the advertising data structures. 
 *
 * This function was implemented partially according to "Bluetooth Secure Simple Pairing Using NFC"
 * (denotation "NFCForum-AD-BTSSP_1_1" published on 2014-01-09) chapters 3.1, 3.2, 4.1.1
 * and 4.1.2 (combined), and according to "Supplement to the Bluetooth Core Specification" (Version 5,
 * adoption date: Dec 02 2014).
 *
 * @note To be able to encode the message, a SoftDevice must be enabled and configured.
 *
 * @param[in]       p_le_advdata        Pointer to the BLE advertising data structure for the LE OOB record.
 * @param[in]       p_ep_advdata        Pointer to the BLE advertising data structure for the EP OOB record.
 * @param[out]      p_buf               Pointer to the buffer for the message.
 * @param[in,out]   p_len               Size of the available memory for the message as input.
 *                                      Size of the generated message as output.
 *
 * @retval NRF_SUCCESS                  If the function completed successfully.
 * @retval NRF_ERROR_xxx                If an error occurred.
 */
ret_code_t nfc_ble_full_handover_select_msg_encode( ble_advdata_t    const * const p_le_advdata,
                                                    ble_advdata_t    const * const p_ep_advdata,
                                                    uint8_t                *       p_buf,
                                                    uint32_t               *       p_len);

/** @brief Function for encoding any type of BLE pairing messages with default BLE
 *         advertising data structures.
 *
 * This function encodes a BLE pairing message into a buffer. The message can be encoded as
 * one of the three message types (using @ref nfc_ble_simplified_le_oob_msg_encode,
 * @ref nfc_ble_simplified_ep_oob_msg_encode, or @ref nfc_ble_full_handover_select_msg_encode),
 * according to the @p nfc_ble_pair_type parameter. LE and EP OOB records use the default 
 * advertising data structure configuration. Only one field ('Security Manager TK') in the BLE 
 * advertising data can be configured for both records by specifying the @p p_tk_value parameter.
 *
 * For LE OOB records, the default BLE advertising data structure configuration fills the required
 * fields 'LE Bluetooth Device Address' and 'LE Role' and the optional fields 'Appearance',
 * 'Local Name', and 'Flags'.
 *
 * For EP OOB records, the default BLE advertising data structure configuration fills the required
 * field 'Security Manager Out Of Band Flags' and the optional fields 'Appearance',
 * 'Local Name', and 'Flags'.
 *
 * @note To be able to encode the message, a SoftDevice must be enabled and configured.
 *
 * @param[in]       nfc_ble_pair_type   Type of BLE pairing message.
 * @param[in]       p_tk_value          Pointer to the authentication Temporary Key (TK). If NULL,
 *                                      Just Works pairing over NFC mode is used. Otherwise, 
 *                                      Out-of-Band pairing over NFC mode is used.
 * @param[out]      p_buf               Pointer to the buffer for the message.
 * @param[in,out]   p_len               Size of the available memory for the message as input.
 *                                      Size of the generated message as output.
 *
 * @retval NRF_SUCCESS                  If the function completed successfully.
 * @retval NRF_ERROR_xxx                If an error occurred.
 */
ret_code_t nfc_ble_pair_default_msg_encode( nfc_ble_pair_type_t            nfc_ble_pair_type,
                                            ble_advdata_tk_value_t * const p_tk_value,
                                            uint8_t                *       p_buf,
                                            uint32_t               *       p_len);

/** @} */
#endif // NFC_BLE_PAIR_MSG_H__
