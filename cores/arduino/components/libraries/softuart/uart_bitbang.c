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
#include "uart_bitbang.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "app_fifo.h"
#include "app_error.h"
#include <stdbool.h>

#define UART_BB_DEBUG_PIN_SET(a) //nrf_gpio_pin_set(a)
#define UART_BB_DEBUG_PIN_CLR(a) //nrf_gpio_pin_clear(a)
#define APP_FIFO_EMPTY(a) (a.read_pos == a.write_pos)
#define APP_FIFO_NOT_EMPTY(a) (a.read_pos != a.write_pos)

static uint16_t *m_envelope_ptr;
static uint32_t m_envelope_pos = 0, m_envelope_end;
static uint32_t m_next_env_cc;

static volatile app_fifo_t m_tx_fifo;
static volatile uint8_t  m_tx_fifo_buf[UART_BB_TX_BUF_SIZE];

// Config parameters
static bool m_uart_bb_txd_inverted;
static uint32_t m_uart_bb_pin_txd;
static uint32_t m_uart_bb_bit_reload_value;
static volatile bool m_uart_bb_operation_pending = false;

static void uart_bb_putchar(uint8_t data);
static void uart_bb_check_for_bytes_in_buffers(void);

#if(UART_BB_RX_ENABLED == 1)
static volatile uint32_t m_uart_bb_rx_cc_buf[16];
static volatile uint32_t m_uart_bb_rx_cc_buf_cnt = 0;
static uart_bb_rx_callback_t m_uart_bb_rx_callback;

static volatile app_fifo_t m_rx_fifo;
static volatile uint8_t  m_rx_fifo_buf[UART_BB_TX_BUF_SIZE];
#endif

#if(UART_BB_USE_SOFTDEVICE == 1)
#include "nrf_soc.h"
static volatile bool m_uart_bb_in_timeslot = false;
static uint32_t m_uart_bb_timeslot_end_predelay;
static nrf_radio_request_t default_radio_request = {.request_type = NRF_RADIO_REQ_TYPE_EARLIEST, 
                                                    .params.earliest.length_us = UART_BB_SD_TIMESLOT_LEN_US,
                                                    .params.earliest.priority = NRF_RADIO_PRIORITY_NORMAL,
                                                    .params.earliest.timeout_us = 1000000,
                                                    .params.earliest.hfclk = NRF_RADIO_HFCLK_CFG_NO_GUARANTEE};

nrf_radio_signal_callback_return_param_t *nrf_radio_signal_callback(uint8_t signal_type)
{
    static nrf_radio_signal_callback_return_param_t return_param;
    return_param.callback_action = NRF_RADIO_SIGNAL_CALLBACK_ACTION_NONE;
    switch(signal_type)
    {
        case NRF_RADIO_CALLBACK_SIGNAL_TYPE_START:
            m_uart_bb_in_timeslot = true;
            UART_BB_DEBUG_PIN_SET(17);
            NRF_TIMER0->CC[0] = UART_BB_SD_TIMESLOT_LEN_US - m_uart_bb_timeslot_end_predelay;
            NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Msk;
            NVIC_EnableIRQ(TIMER0_IRQn);
            uart_bb_check_for_bytes_in_buffers();
            break;
        
        case NRF_RADIO_CALLBACK_SIGNAL_TYPE_TIMER0:
            NRF_TIMER0->EVENTS_COMPARE[0] = 0;
            UART_BB_DEBUG_PIN_CLR(17);
            m_uart_bb_in_timeslot = false;
            if(APP_FIFO_EMPTY(m_tx_fifo))
            {

                return_param.callback_action = NRF_RADIO_SIGNAL_CALLBACK_ACTION_END;
            }
            else
            {
                return_param.callback_action = NRF_RADIO_SIGNAL_CALLBACK_ACTION_REQUEST_AND_END;
                return_param.params.request.p_next = &default_radio_request;
                //return_param.callback_action = NRF_RADIO_SIGNAL_CALLBACK_ACTION_EXTEND;
                //return_param.params.extend.length_us = UART_BB_SD_TIMESLOT_LEN_US;
            }
            break;
        
        case NRF_RADIO_CALLBACK_SIGNAL_TYPE_EXTEND_SUCCEEDED:
            NRF_TIMER0->CC[0] += UART_BB_SD_TIMESLOT_LEN_US;
            break;
        
        case NRF_RADIO_CALLBACK_SIGNAL_TYPE_EXTEND_FAILED:
            m_uart_bb_in_timeslot = false;
            UART_BB_DEBUG_PIN_CLR(17);
            return_param.callback_action = NRF_RADIO_SIGNAL_CALLBACK_ACTION_REQUEST_AND_END;
            return_param.params.request.p_next = &default_radio_request;
            break;
        
        default:
            break;
    }
    return &return_param;
}
#endif

void uart_bb_on_sys_evt(uint32_t sys_evt_id)
{
#if(UART_BB_USE_SOFTDEVICE == 1)
    switch(sys_evt_id)
    {
        case NRF_EVT_RADIO_BLOCKED:
            break;
        case NRF_EVT_RADIO_CANCELED:
            break;
        case NRF_EVT_RADIO_SIGNAL_CALLBACK_INVALID_RETURN:
            break;
        case NRF_EVT_RADIO_SESSION_IDLE:
            break;
        case NRF_EVT_RADIO_SESSION_CLOSED:
            break;            
    }
#endif
}

void uart_bb_init(uart_bb_config_t *config)
{
    app_fifo_init((app_fifo_t*)&m_tx_fifo, (uint8_t*)m_tx_fifo_buf, UART_BB_TX_BUF_SIZE);

    m_uart_bb_pin_txd = config->pin_txd;

    NRF_GPIO->DIRSET = 1 << config->pin_txd; 

    m_uart_bb_txd_inverted = config->txd_inverted;  
    nrf_gpio_pin_write(m_uart_bb_pin_txd, m_uart_bb_txd_inverted ? 0 : 1);  
    
    m_uart_bb_bit_reload_value = (16000000 + config->baudrate / 2) / config->baudrate; 
    
    UART_BB_TX_TIMER->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
    // Configure the timer with a base frequency of 500 kHz
    UART_BB_TX_TIMER->PRESCALER = 0;
        
    for(int i = 0; i < UART_BB_TX_TIMER_CC_NUM; i++)
    {
        NRF_PPI->CH[UART_BB_PPI_START_CHANNEL + i].EEP = (uint32_t)&UART_BB_TX_TIMER->EVENTS_COMPARE[i];
        NRF_PPI->CH[UART_BB_PPI_START_CHANNEL + i].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[UART_BB_TX_GPIOTE_CH];
        NRF_PPI->CHENSET = 1 << (UART_BB_PPI_START_CHANNEL + i);
        NRF_PPI->CHG[0] |= 1 << (UART_BB_PPI_START_CHANNEL + i);
        UART_BB_TX_TIMER->INTENSET = TIMER_INTENSET_COMPARE0_Msk << i;
    }
    NVIC_SetPriority(UART_BB_TX_TIMER_IRQn, 1);
    NVIC_EnableIRQ(UART_BB_TX_TIMER_IRQn);
    
    NVIC_SetPriority(UART_BB_SW_IRQn, 7);
    NVIC_EnableIRQ(UART_BB_SW_IRQn);
    
#if(UART_BB_USE_SOFTDEVICE == 1)
    m_uart_bb_timeslot_end_predelay = (10 - UART_BB_TX_TIMER_CC_NUM) * 1000000 / config->baudrate + 100;
    sd_radio_session_open(nrf_radio_signal_callback);
#endif
    
#if(UART_BB_RX_ENABLED == 1)
    app_fifo_init((app_fifo_t*)&m_rx_fifo, (uint8_t*)m_rx_fifo_buf, UART_BB_RX_BUF_SIZE);
    
    nrf_gpio_cfg_input(config->pin_rxd, NRF_GPIO_PIN_NOPULL);
   
    NRF_GPIOTE->CONFIG[UART_BB_RX_GPIOTE_CH] = GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos | GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos | 
                            config->pin_rxd << GPIOTE_CONFIG_PSEL_Pos;
      
    UART_BB_RX_TIMER->PRESCALER = 0;
    UART_BB_RX_TIMER->CC[0] = 1;
    UART_BB_RX_TIMER->CC[2] = 9 * (16000000 + config->baudrate / 2) / config->baudrate;
    UART_BB_RX_TIMER->SHORTS = TIMER_SHORTS_COMPARE2_STOP_Msk | TIMER_SHORTS_COMPARE2_CLEAR_Msk;
    UART_BB_RX_TIMER->INTENSET = TIMER_INTENSET_COMPARE0_Msk;
    NVIC_SetPriority(UART_BB_RX_TIMER_IRQn, 3);
    NVIC_EnableIRQ(UART_BB_RX_TIMER_IRQn);
    
    NRF_PPI->CH[UART_BB_TX_TIMER_CC_NUM + 0].EEP = (uint32_t)&NRF_GPIOTE->EVENTS_IN[UART_BB_RX_GPIOTE_CH];
    NRF_PPI->CH[UART_BB_TX_TIMER_CC_NUM + 0].TEP = (uint32_t)&UART_BB_RX_TIMER->TASKS_CAPTURE[1];
    NRF_PPI->FORK[UART_BB_TX_TIMER_CC_NUM + 0].TEP = (uint32_t)&UART_BB_RX_TIMER->TASKS_START;

    NRF_PPI->CHENSET = (1 << (UART_BB_TX_TIMER_CC_NUM + 0));
    
    m_uart_bb_rx_callback = config->rx_callback;
    
    NVIC_SetPriority(UART_BB_RX_TIMER_IRQn, 2);
    NVIC_EnableIRQ(UART_BB_RX_TIMER_IRQn);
#endif
}

void uart_bb_put(uint8_t data)
{
    uint32_t err_code;
    do
    {
        //NVIC_DisableIRQ(UART_BB_SW_IRQ);
        err_code = app_fifo_put((app_fifo_t*)&m_tx_fifo, data);
        //NVIC_EnableIRQ(UART_BB_SW_IRQ);
        uart_bb_check_for_bytes_in_buffers();
    }while(err_code != NRF_SUCCESS);
}

static void uart_bb_check_for_bytes_in_buffers(void)
{
    NVIC_SetPendingIRQ(UART_BB_SW_IRQn);
}

#ifdef __cplusplus
extern "C" {
#endif

void UART_BB_SW_IRQHandler(void) 
{
    static uint8_t byte_from_buf;
    if(!m_uart_bb_operation_pending)
    {
        if(APP_FIFO_NOT_EMPTY(m_tx_fifo))
        {
#if(UART_BB_USE_SOFTDEVICE == 1)
            if(m_uart_bb_in_timeslot)
            {
                if(app_fifo_get((app_fifo_t*)&m_tx_fifo, &byte_from_buf) == NRF_SUCCESS)
                {
                    uart_bb_putchar(byte_from_buf);
                }
            }
            else
            {
                sd_radio_request(&default_radio_request);	
            }
#else       
            if(app_fifo_get((app_fifo_t*)&m_tx_fifo, &byte_from_buf) == NRF_SUCCESS)
            {
                uart_bb_putchar(byte_from_buf);
            }
#endif            
        }
    }
    if(m_uart_bb_rx_callback)
    {
        while(APP_FIFO_NOT_EMPTY(m_rx_fifo))
        {
            app_fifo_get((app_fifo_t*)&m_rx_fifo, &byte_from_buf);
            m_uart_bb_rx_callback(byte_from_buf);
        }
    }
}

#ifdef __cplusplus
}
#endif

static void uart_bb_putchar(uint8_t data)
{
    static uint16_t env_buf[12];

    m_uart_bb_operation_pending = true;
    
    data = ((data & 0xAA) >> 1) | ((data & 0x55) << 1);
    data = ((data & 0xCC) >> 2) | ((data & 0x33) << 2);
    data = ((data & 0xF0) >> 4) | ((data & 0x0F) << 4);
    
    uint32_t tmp_data = ((uint32_t)data | 0x100) << 1;
    uint32_t env_buf_pos = 0;
    uint32_t last_bit_value = 1, current_bit_value = 1;
    uint32_t current_envelope = 0;
    if(!m_uart_bb_txd_inverted) tmp_data ^= (0x1FE);
    
    for(int i = 0; i < 10; i++)
    {
        current_bit_value = (tmp_data & 0x100) ? 1 : 0;
        tmp_data <<= 1;
        if(current_bit_value == last_bit_value)
        {
            current_envelope += m_uart_bb_bit_reload_value;
        }
        else
        {
            env_buf[env_buf_pos++] = current_envelope + m_uart_bb_bit_reload_value;
            current_envelope = 0;
            last_bit_value = current_bit_value;
        }
    }
    env_buf[env_buf_pos++] = current_envelope + m_uart_bb_bit_reload_value;
    uart_bb_run(env_buf, env_buf_pos);
}

void uart_bb_run(uint16_t *envelope_list, uint32_t envelope_num)
{
    m_envelope_ptr = envelope_list;
    m_envelope_pos = 0;
    m_envelope_end = envelope_num;
    m_next_env_cc = 0;
 
    for(int i = 0; i < UART_BB_TX_TIMER_CC_NUM; i++)
    {
        UART_BB_TX_TIMER->EVENTS_COMPARE[i] = 0;
        UART_BB_TX_TIMER->CC[i] = 0xFFFF;
    }
    
    UART_BB_TX_TIMER->TASKS_CLEAR = 1;
    NRF_PPI->TASKS_CHG[0].EN = 1;

    nrf_gpiote_task_configure(UART_BB_TX_GPIOTE_CH, m_uart_bb_pin_txd, NRF_GPIOTE_POLARITY_TOGGLE, m_uart_bb_txd_inverted ? NRF_GPIOTE_INITIAL_VALUE_LOW : NRF_GPIOTE_INITIAL_VALUE_HIGH);
    nrf_gpiote_task_enable(UART_BB_TX_GPIOTE_CH); 
    
    // Fill the first CC_NUM envelopes into the CC registers
    UART_BB_TX_TIMER->CC[0] = 20; 
    for(int i = 1; (i < UART_BB_TX_TIMER_CC_NUM) && (i <= envelope_num); i++)
    {
        UART_BB_TX_TIMER->CC[i] = UART_BB_TX_TIMER->CC[i-1] + *m_envelope_ptr++; 
    }   
    UART_BB_TX_TIMER->TASKS_START = 1;
}

#ifdef __cplusplus
extern "C" {
#endif

void UART_BB_TX_TIMER_IRQHandler(void)
{
    while(UART_BB_TX_TIMER->EVENTS_COMPARE[m_next_env_cc] != 0)
    {
        UART_BB_TX_TIMER->EVENTS_COMPARE[m_next_env_cc] = 0;
        UART_BB_DEBUG_PIN_SET(15);
        if((m_envelope_end >= UART_BB_TX_TIMER_CC_NUM) && m_envelope_pos < (m_envelope_end -(UART_BB_TX_TIMER_CC_NUM-1)))
        {
            UART_BB_TX_TIMER->CC[m_next_env_cc] = UART_BB_TX_TIMER->CC[(m_next_env_cc + UART_BB_TX_TIMER_CC_NUM - 1) % UART_BB_TX_TIMER_CC_NUM] + *m_envelope_ptr++;
            m_envelope_pos++;
            m_next_env_cc = (m_next_env_cc + 1) % UART_BB_TX_TIMER_CC_NUM;
        }
        else if(m_envelope_pos < m_envelope_end)
        {
            if(m_envelope_pos == (m_envelope_end-1))  NRF_PPI->TASKS_CHG[0].DIS = 1;
            m_envelope_pos++;
            m_next_env_cc = (m_next_env_cc + 1) % UART_BB_TX_TIMER_CC_NUM;
        }
        else
        {
            UART_BB_TX_TIMER->TASKS_STOP = 1;

            nrf_gpiote_task_disable(UART_BB_TX_GPIOTE_CH);
            m_uart_bb_operation_pending = false;
            
            uart_bb_check_for_bytes_in_buffers();
        }
        UART_BB_DEBUG_PIN_CLR(15);
    }
}

void UART_BB_RX_TIMER_IRQHandler(void)
{
    static uint32_t cc_buf[16];
    uint32_t cc_buf_cnt = 0;
    uint32_t new_value = 0;
    if(UART_BB_RX_TIMER->EVENTS_COMPARE[0]) 
    {
        UART_BB_RX_TIMER->EVENTS_COMPARE[0] = 0;
        
        UART_BB_DEBUG_PIN_SET(18);
        UART_BB_RX_TIMER->EVENTS_COMPARE[2] = 0;
        cc_buf[0] = 0;
        do
        {
            if(UART_BB_RX_TIMER->CC[1] != cc_buf[cc_buf_cnt])
            {
                cc_buf[++cc_buf_cnt] = UART_BB_RX_TIMER->CC[1];
            }
        }while(UART_BB_RX_TIMER->EVENTS_COMPARE[2] == 0);
        
        uint32_t current_bit_state = 0;
        for(int cc_buf_index = 1; cc_buf_index <= cc_buf_cnt; cc_buf_index++)
        {
            int bits_in_a_row = ((cc_buf[cc_buf_index] - cc_buf[cc_buf_index - 1]) + m_uart_bb_bit_reload_value / 2) / m_uart_bb_bit_reload_value;
            for(int i = 0; i < bits_in_a_row; i++) new_value = new_value >> 1 | current_bit_state;
            current_bit_state ^= (1 << 7);
        }    
        app_fifo_put((app_fifo_t*)&m_rx_fifo, (uint8_t)new_value); 
        uart_bb_check_for_bytes_in_buffers();
        UART_BB_DEBUG_PIN_CLR(18);
    }
}

#ifdef __cplusplus
}
#endif
