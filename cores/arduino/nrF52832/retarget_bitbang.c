/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "app_uart.h"
#include "nordic_common.h"
#include "nrf_error.h"
#include "uart_bitbang.h"

struct __FILE 
{
    int handle;
};

FILE __stdout;
FILE __stdin;



int fgetc(FILE * p_file)
{
    uint8_t input;

    return '0';
}


int fputc(int ch, FILE * p_file)
{
    UNUSED_PARAMETER(p_file);

    uart_bb_put((uint8_t)ch);
    return ch;
}

