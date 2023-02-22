/*
 * Copyright (c) 2016 Intel Corporation.
 * Copyright (c) 2016-2023 Makerdiary
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>

BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
         "Console device is not ACM CDC UART device");

#define QSPI_FLASH_TEST_REGION_OFFSET 0xff000
#define QSPI_FLASH_SECTOR_SIZE        4096

void main(void)
{
    const uint8_t expected[] = { 0x55, 0xaa, 0x66, 0x99 };
    const size_t len = sizeof(expected);
    uint8_t buf[sizeof(expected)];
    const struct device *flash_dev;
    int rc;

    flash_dev = DEVICE_DT_GET(DT_ALIAS(spi_flash0));

    const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
    uint32_t dtr = 0;

    if (usb_enable(NULL)) {
        return;
    }

    /* Poll if the DTR flag was set */
    while (!dtr) {
        uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
        /* Give CPU resources to low priority threads. */
        k_sleep(K_MSEC(100));
    }

    if (!device_is_ready(flash_dev)) {
        printf("%s: device not ready.\n", flash_dev->name);
        return;
    }

    printf("\n%s QSPI flash testing\n", flash_dev->name);
    printf("==========================\n");


    /* Write protection needs to be disabled before each write or
     * erase, since the flash component turns on write protection
     * automatically after completion of write and erase
     * operations.
     */
    printf("\nTest 1: Flash erase\n");

    /* Full flash erase if QSPI_FLASH_TEST_REGION_OFFSET = 0 and
     * QSPI_FLASH_SECTOR_SIZE = flash size
     */
    rc = flash_erase(flash_dev, QSPI_FLASH_TEST_REGION_OFFSET,
             QSPI_FLASH_SECTOR_SIZE);
    if (rc != 0) {
        printf("Flash erase failed! %d\n", rc);
    } else {
        printf("Flash erase succeeded!\n");
    }

    printf("\nTest 2: Flash write\n");

    printf("Attempting to write %zu bytes\n", len);
    rc = flash_write(flash_dev, QSPI_FLASH_TEST_REGION_OFFSET, expected, len);
    if (rc != 0) {
        printf("Flash write failed! %d\n", rc);
        return;
    }

    memset(buf, 0, len);
    rc = flash_read(flash_dev, QSPI_FLASH_TEST_REGION_OFFSET, buf, len);
    if (rc != 0) {
        printf("Flash read failed! %d\n", rc);
        return;
    }

    if (memcmp(expected, buf, len) == 0) {
        printf("Data read matches data written. Good!!\n");
    } else {
        const uint8_t *wp = expected;
        const uint8_t *rp = buf;
        const uint8_t *rpe = rp + len;

        printf("Data read does not match data written!!\n");
        while (rp < rpe) {
            printf("%08x wrote %02x read %02x %s\n",
                   (uint32_t)(QSPI_FLASH_TEST_REGION_OFFSET + (rp - buf)),
                   *wp, *rp, (*rp == *wp) ? "match" : "MISMATCH");
            ++rp;
            ++wp;
        }
    }
}
