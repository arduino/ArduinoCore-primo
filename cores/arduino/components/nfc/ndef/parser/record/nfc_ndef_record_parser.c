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

#include <stdint.h>
#include <stdbool.h>
#include "nfc_ndef_record_parser.h"
#include "nfc_ndef_parser_logger.h"
#include "app_util.h"
#include "nordic_common.h"
#include "nrf_delay.h"


/* Sum of sizes of fields: TNF-flags, Type Length, Payload Length in short NDEF record. */
#define NDEF_RECORD_BASE_LONG_SHORT 2 + NDEF_RECORD_PAYLOAD_LEN_SHORT_SIZE


ret_code_t ndef_record_parser(nfc_ndef_bin_payload_desc_t * p_bin_pay_desc,
                              nfc_ndef_record_desc_t      * p_rec_desc,
                              nfc_ndef_record_location_t  * p_record_location,
                              uint8_t const               * p_nfc_data,
                              uint32_t                    * p_nfc_data_len)
{
    uint32_t expected_rec_size = NDEF_RECORD_BASE_LONG_SHORT;

    if (expected_rec_size > *p_nfc_data_len)
    {
        return NRF_ERROR_INVALID_LENGTH;
    }

    p_rec_desc->tnf = (nfc_ndef_record_tnf_t) ((*p_nfc_data) & NDEF_RECORD_TNF_MASK);

    /* An NDEF parser that receives an NDEF record with an unknown or unsupported TNF field value
       SHOULD treat it as Unknown. See NFCForum-TS-NDEF_1.0 */
    if (p_rec_desc->tnf == TNF_RESERVED)
    {
        p_rec_desc->tnf = TNF_UNKNOWN_TYPE;
    }

    *p_record_location = (nfc_ndef_record_location_t) ((*p_nfc_data) & NDEF_RECORD_LOCATION_MASK);

    uint8_t flags = *(p_nfc_data++);

    p_rec_desc->type_length = *(p_nfc_data++);

    uint32_t payload_lenght;

    if (flags & NDEF_RECORD_SR_MASK)
    {
        payload_lenght = *(p_nfc_data++);
    }
    else
    {
        expected_rec_size +=
            NDEF_RECORD_PAYLOAD_LEN_LONG_SIZE - NDEF_RECORD_PAYLOAD_LEN_SHORT_SIZE;

        if (expected_rec_size > *p_nfc_data_len)
        {
            return NRF_ERROR_INVALID_LENGTH;
        }

        payload_lenght = uint32_big_decode(p_nfc_data);
        p_nfc_data    += NDEF_RECORD_PAYLOAD_LEN_LONG_SIZE;
    }

    if (flags & NDEF_RECORD_IL_MASK)
    {
        expected_rec_size += NDEF_RECORD_ID_LEN_SIZE;

        if (expected_rec_size > *p_nfc_data_len)
        {
            return NRF_ERROR_INVALID_LENGTH;
        }

        p_rec_desc->id_length = *(p_nfc_data++);
    }
    else
    {
        p_rec_desc->id_length = 0;
        p_rec_desc->p_id      = NULL;
    }

    expected_rec_size += p_rec_desc->type_length + p_rec_desc->id_length + payload_lenght;

    if (expected_rec_size > *p_nfc_data_len)
    {
        return NRF_ERROR_INVALID_LENGTH;
    }

    if (p_rec_desc->type_length > 0)
    {
        p_rec_desc->p_type = p_nfc_data;

        p_nfc_data += p_rec_desc->type_length;
    }
    else
    {
        p_rec_desc->p_type = NULL;
    }

    if (p_rec_desc->id_length > 0)
    {
        p_rec_desc->p_id = p_nfc_data;

        p_nfc_data += p_rec_desc->id_length;
    }

    if (payload_lenght == 0)
    {
        p_bin_pay_desc->p_payload = NULL;
    }
    else
    {
        p_bin_pay_desc->p_payload = p_nfc_data;
    }

    p_bin_pay_desc->payload_length = payload_lenght;

    p_rec_desc->p_payload_descriptor = p_bin_pay_desc;
    p_rec_desc->payload_constructor  = (p_payload_constructor_t) nfc_ndef_bin_payload_memcopy;

    *p_nfc_data_len = expected_rec_size;

    return NRF_SUCCESS;
}


char const * const tnf_strings[] =
{
    "Empty\r\n",
    "NFC Forum well-known type\r\n",
    "Media-type (RFC 2046)\r\n",
    "Absolute URI (RFC 3986)\r\n",
    "NFC Forum external type (NFC RTD)\r\n",
    "Unknown\r\n",
    "Unchanged\r\n",
    "Reserved\r\n"
};


#define MAX_NDEF_PASER_HEX_STRING_LEN 8

static void ndef_hexdata_printout(uint32_t num, uint8_t const * p_data, uint16_t indent)
{
    uint32_t i;
    uint32_t j;
    uint8_t  tempbuf[MAX_NDEF_PASER_HEX_STRING_LEN + 1] = {[MAX_NDEF_PASER_HEX_STRING_LEN] = 0};

    if (num > 0)
    {
        for (j = 0; j < indent; j++)
        {
            NDEF_PARSER_TRACE(" ");
        }

        for (i = 0; i < num; i++)
        {
            if (i && ((i % MAX_NDEF_PASER_HEX_STRING_LEN) == 0))
            {
                NDEF_PARSER_TRACE("    %s\r\n", tempbuf);

                for (j = 0; j < indent; j++)
                {
                    NDEF_PARSER_TRACE(" ");
                }

                nrf_delay_ms(10);
            }
            NDEF_PARSER_TRACE("%02X ", *p_data);


            if (*p_data > ' ' && *p_data <= '~')
            {
                tempbuf[i % MAX_NDEF_PASER_HEX_STRING_LEN] = *p_data;
            }
            else
            {
                tempbuf[i % MAX_NDEF_PASER_HEX_STRING_LEN] = '.';
            }

            p_data++;
        }

        i = i % MAX_NDEF_PASER_HEX_STRING_LEN;
        if (i == 0)
            i = MAX_NDEF_PASER_HEX_STRING_LEN;

        if (i > 0)
        {
            for (j = i; j < MAX_NDEF_PASER_HEX_STRING_LEN; j++)
            {
                NDEF_PARSER_TRACE("   ");
            }

            tempbuf[i] = 0;
            NDEF_PARSER_TRACE("    %s", tempbuf);
        }
    }

    NDEF_PARSER_TRACE("\r\n");

    UNUSED_VARIABLE(tempbuf);
}


void ndef_record_printout(uint32_t num, nfc_ndef_record_desc_t * const p_rec_desc)
{
    NDEF_PARSER_TRACE("NDEF record %d content:\r\n", num);
    NDEF_PARSER_TRACE("TNF: ");
    NDEF_PARSER_TRACE((char *)tnf_strings[(uint32_t) p_rec_desc->tnf]);

    if (p_rec_desc->p_id != NULL)
    {
        NDEF_PARSER_TRACE("ID:\r\n");
        ndef_hexdata_printout(p_rec_desc->id_length, p_rec_desc->p_id, 2);
    }

    if (p_rec_desc->p_type != NULL)
    {
        NDEF_PARSER_TRACE("type:\r\n");
        ndef_hexdata_printout(p_rec_desc->type_length, p_rec_desc->p_type, 2);
    }

    if (p_rec_desc->payload_constructor == (p_payload_constructor_t) nfc_ndef_bin_payload_memcopy)
    {
        nfc_ndef_bin_payload_desc_t * p_bin_pay_desc = p_rec_desc->p_payload_descriptor;

        if (p_bin_pay_desc->p_payload != NULL)
        {
            NDEF_PARSER_TRACE("Payload data (%d bytes):\r\n", p_bin_pay_desc->payload_length);
            ndef_hexdata_printout(p_bin_pay_desc->payload_length, p_bin_pay_desc->p_payload, 2);
        }
        else
        {
            NDEF_PARSER_TRACE("No payload\r\n");
        }
    }
    NDEF_PARSER_TRACE("\r\n");
}


