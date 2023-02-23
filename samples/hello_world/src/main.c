/*
 * Copyright (c) 2016-2023 Makerdiary
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

void main(void)
{
    while (1) {
        printk("Hello World! %s\n", CONFIG_BOARD);
        k_sleep(K_SECONDS(1));
    }
}
