#ifndef __UART_BITBANG_H__
#define __UART_BITBANG_H__

#define UART_BB_TX_TIMER                NRF_TIMER4
#define UART_BB_TX_TIMER_IRQn           TIMER4_IRQn
#define UART_BB_TX_TIMER_IRQHandler     TIMER4_IRQHandler
#define UART_BB_TX_TIMER_IRQPriority    0
#define UART_BB_TX_GPIOTE_CH            0
#define UART_BB_TX_TIMER_CC_NUM         6
#define UART_BB_TX_BUF_SIZE             64  
#define UART_BB_USE_SOFTDEVICE          0
#define UART_BB_SD_TIMESLOT_LEN_US      2000

#define UART_BB_PPI_START_CHANNEL       0
#define UART_BB_PPI_CHANNEL_NUM         (UART_BB_TX_TIMER_CC_NUM)

#define UART_BB_RX_ENABLED              1
#define UART_BB_RX_TIMER                NRF_TIMER3
#define UART_BB_RX_TIMER_IRQn           TIMER3_IRQn
#define UART_BB_RX_TIMER_IRQHandler     TIMER3_IRQHandler
#define UART_BB_RX_TIMER_IRQPriority    1
#define UART_BB_RX_BUF_SIZE             16

#define UART_BB_RX_GPIOTE_CH            1

#define UART_BB_SW_IRQn                 QDEC_IRQn
#define UART_BB_SW_IRQHandler           QDEC_IRQHandler
#define UART_BB_SW_IRQPriority          7

#if(UART_BB_USE_SOFTDEVICE == 1 && (UART_BB_TX_TIMER_IRQPriority == 0 || UART_BB_TX_TIMER_IRQPriority == 1 || UART_BB_TX_TIMER_IRQPriority == 4 || UART_BB_TX_TIMER_IRQPriority == 5))
#error Invalid IRQ Priority for TX TIMER. Legal values when using softdevice are 2, 3, 6, 7
#endif

#if(UART_BB_USE_SOFTDEVICE == 1 && (UART_BB_RX_TIMER_IRQPriority == 0 || UART_BB_RX_TIMER_IRQPriority == 1 || UART_BB_RX_TIMER_IRQPriority == 4 || UART_BB_RX_TIMER_IRQPriority == 5))
#error Invalid IRQ Priority for RX TIMER. Legal values when using softdevice are 2, 3, 6, 7
#endif

#if(UART_BB_USE_SOFTDEVICE == 1 && (UART_BB_SW_IRQPriority == 0 || UART_BB_SW_IRQPriority == 1 || UART_BB_SW_IRQPriority == 4 || UART_BB_SW_IRQPriority == 5))
#error Invalid IRQ Priority for the Software interrupt. Legal values when using softdevice are 2, 3, 6, 7
#endif

#include "nrf.h"

typedef void (*uart_bb_rx_callback_t)(uint8_t byte);

typedef struct
{
    uint32_t pin_txd;
    uint32_t pin_rxd;
    uint32_t txd_inverted : 1;
    uint32_t baudrate;
    uart_bb_rx_callback_t rx_callback;
}uart_bb_config_t;

void uart_bb_init(uart_bb_config_t *config);

void uart_bb_put(uint8_t data);

void uart_bb_run(uint16_t *envelope_list, uint32_t envelope_num);

void uart_bb_on_sys_evt(uint32_t sys_evt);

#endif
