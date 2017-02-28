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
 
#ifndef NFC_FIXES_H__
#define NFC_FIXES_H__

#include <stdint.h>

/** @file
 * @defgroup nfc_fixes NFC fixes
 * @{
 * @ingroup nfc_library
 * @brief @tagAPI52 Fixes for HW anomaly.
 *
 * If you are using PCA10036 (part of nRF52 Preview Development Kit), 
 * you must define the macro HAL_NFC_ENGINEERING_A_FTPAN_WORKAROUND to apply 
 * workarounds for the following anomalies:
 * - 17. NFCT: The EVENTS_FIELDLOST is not generated
 * - 24. NFCT: The FIELDPRESENT register read is not reliable
 * - 27. NFCT: Triggering NFCT ACTIVATE task also activates the Rx easyDMA
 * - 45. NFCT: EasyDMA does not function properly while System ON CPU IDLE state
 * - 57. NFCT: NFC Modulation amplitude
 *
 * If you are using PCA10040 (part of nRF52 Development Kit), 
 * you must define the macro HAL_NFC_ENGINEERING_BC_FTPAN_WORKAROUND to apply 
 * workarounds for the following anomalies:
 * - 79. NFCT:  A false EVENTS_FIELDDETECTED event occurs after the field is lost
 * - NFCT does not release HFCLK when switching from ACTVATED to SENSE mode.
 *
 * The current code contains a patch for anomaly 25 (NFCT: Reset value of 
 * SENSRES register is incorrect), so that it now works on Windows Phone.
 */
 
#ifdef BOARD_PCA10036 // assume nRF52832 chip in IC rev. Engineering A
    #define HAL_NFC_ENGINEERING_A_FTPAN_WORKAROUND
#elif defined(BOARD_PCA10040) // assume nRF52832 chip in IC rev. Engineering B or Engineering C
    #define HAL_NFC_ENGINEERING_BC_FTPAN_WORKAROUND
#endif

/* Begin: Bugfix for FTPAN-45 */
#ifdef HAL_NFC_ENGINEERING_A_FTPAN_WORKAROUND
    extern volatile uint8_t m_nfc_active;
    #define NFC_NEED_MCU_RUN_STATE() (m_nfc_active != 0)
#else
    #define NFC_NEED_MCU_RUN_STATE() (0)
#endif
/* End: Bugfix for FTPAN-45 */

/** @} */
#endif /* NFC_FIXES_H__ */

