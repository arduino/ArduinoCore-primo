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
#include "nfc_ndef_msg_parser.h"
#include "nfc_ndef_parser_logger.h"
#include "nrf_delay.h"

ret_code_t ndef_msg_parser(uint8_t  * const p_result_buf,
                           uint32_t * const p_result_buf_len,
                           uint8_t  * const p_nfc_data,
                           uint32_t * const p_nfc_data_len)
{
    ret_code_t                  ret_code;
    nfc_ndef_parser_memo_desc_t parser_memory_helper;

    ret_code = ndef_parser_memo_resolve(p_result_buf,
                                        p_result_buf_len,
                                        &parser_memory_helper);

    if (ret_code != NRF_SUCCESS)
    {
        return ret_code;
    }

    ret_code = internal_ndef_msg_parser(&parser_memory_helper,
                                        p_nfc_data,
                                        p_nfc_data_len);

    return ret_code;
}


void ndef_msg_printout(nfc_ndef_msg_desc_t * const p_msg_desc)
{
    uint32_t i;

    nrf_delay_ms(100);
    NDEF_PARSER_TRACE("NDEF message contains %d record(s)", p_msg_desc->record_count);
    if (p_msg_desc->record_count > 1)
    {
        NDEF_PARSER_TRACE("s");
    }
    NDEF_PARSER_TRACE("\r\n\r\n");

    for (i = 0; i < p_msg_desc->record_count; i++)
    {
        ndef_record_printout(i, p_msg_desc->pp_record[i]);
    }
}


