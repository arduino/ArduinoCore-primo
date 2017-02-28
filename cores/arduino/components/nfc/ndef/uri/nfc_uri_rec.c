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

#include <string.h>
#include "nfc_uri_rec.h"
#include "nrf_error.h"

/**
 * @brief Type of description of the payload of a URI record.
 */
typedef struct
{
    nfc_uri_id_t    uri_id_code;  ///< URI identifier code.
    uint8_t const * p_uri_data;   ///< Pointer to a URI string.
    uint8_t         uri_data_len; ///< Length of the URI string.
} uri_payload_desc_t;

/**
 * @brief Function for constructing the payload for a URI record.
 *
 * This function encodes the payload according to the URI record definition. It implements an API
 * compatible with @ref p_payload_constructor_t.
 *
 * @param[in] p_input           Pointer to the description of the payload.
 * @param[out] p_buff           Pointer to payload destination.
 *
 * @param[in,out] p_len         Size of available memory to write as input. Size of generated
 *                              payload as output.
 *
 * @retval NRF_SUCCESS          If the payload was encoded successfully.
 * @retval NRF_ERROR_NO_MEM     If the predicted payload size is bigger than the provided buffer space.
 */
static ret_code_t nfc_uri_payload_constructor( uri_payload_desc_t * p_input,
                                               uint8_t * p_buff,
                                               uint32_t * p_len)
{
    /* Verify if there is enough available memory */
    if(p_input->uri_data_len >= *p_len)
    {
        return NRF_ERROR_NO_MEM;
    }

    /* Copy descriptor content into the buffer */
    *p_len      = p_input->uri_data_len + 1;
    *(p_buff++) = p_input->uri_id_code;
    memcpy(p_buff, p_input->p_uri_data, p_input->uri_data_len );
    
    return NRF_SUCCESS;
}

nfc_ndef_record_desc_t * nfc_uri_rec_declare( nfc_uri_id_t           uri_id_code,
                                              uint8_t const *  const p_uri_data,
                                              uint8_t                uri_data_len)
{
    static uri_payload_desc_t uri_payload_desc;
    static const uint8_t static_uri_type = 'U';
    
    NFC_NDEF_GENERIC_RECORD_DESC_DEF( uri_rec,
                                      TNF_WELL_KNOWN, // tnf <- well-known
                                      NULL,
                                      0,    // no id
                                      &static_uri_type,
                                      1, // type size 1B
                                      nfc_uri_payload_constructor,
                                      &uri_payload_desc);
   
   uri_payload_desc.uri_id_code  = uri_id_code;
   uri_payload_desc.p_uri_data   = p_uri_data;
   uri_payload_desc.uri_data_len = uri_data_len;
   
   return &NFC_NDEF_GENERIC_RECORD_DESC( uri_rec);
}

