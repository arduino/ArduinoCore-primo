/*
  Copyright (c) 2016 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
*/

#ifndef _DFU_SERVICE_H
#define _DFU_SERVICE_H


/*
 * \brief Choose if remove or not the DFU service from the sketches (enabled by default).
 *
 * \param remove
 */
extern void removeDfuService(bool remove);


/*
 * \brief Indicate whether DFU service is enabled or not.
 *
 */
extern bool dfuIsEnabled();


/*
 * \brief Add DFU service to the sketch.
 *
 */
extern void add_dfu_service();

/*
 * \brief Set parameters used in bonding procedure from external libraries.
 *
 * \param Bond, Mimt, Lest, IO_caps
 */ 
extern void setSecParams(uint8_t Bond, uint8_t Mitm, uint8_t Lesc, uint8_t IO_caps);

extern void eraseBond();
extern void initDM();
/*
 * \brief Receive and forward all BLE events.
 *
 * \param p_ble_evt
 */
extern void ble_evt_dispatch(ble_evt_t * p_ble_evt);

#ifdef __cplusplus
extern "C"{
#endif
extern void forwardEvent(uint32_t type, uint32_t code);
#ifdef __cplusplus
}
#endif

#endif /*_DFU_SERVICE_H*/