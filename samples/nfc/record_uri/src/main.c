/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * Copyright (c) 2016-2023 Makerdiary
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/drivers/gpio.h>

#include <nfc_t2t_lib.h>
#include <nfc/ndef/uri_msg.h>

#define NDEF_MSG_BUF_SIZE    256

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* URL makerdiary.com */
static const uint8_t m_url[] =
    {'m', 'a', 'k', 'e', 'r', 'd', 'i', 'a', 'r', 'y', '.', 'c', 'o', 'm'}; //URL "makerdiary.com"

/* Buffer used to hold an NFC NDEF message. */
static uint8_t ndef_msg_buf[NDEF_MSG_BUF_SIZE];


static void nfc_callback(void *context,
             nfc_t2t_event_t event,
             const uint8_t *data,
             size_t data_length)
{
    ARG_UNUSED(context);
    ARG_UNUSED(data);
    ARG_UNUSED(data_length);

    switch (event) {
    case NFC_T2T_EVENT_FIELD_ON:
        gpio_pin_set_dt(&led, 1);
        break;
    case NFC_T2T_EVENT_FIELD_OFF:
        gpio_pin_set_dt(&led, 0);
        break;
    default:
        break;
    }
}

int main(void)
{
    int err;

    printk("Starting NFC URI record sample\n");

    if (!device_is_ready(led.port)) {
        return -ENOTSUP;
    }

    err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (err < 0) {
        return err;
    }

    /* Set up NFC */
    err = nfc_t2t_setup(nfc_callback, NULL);
    if (err) {
        printk("Cannot setup NFC T2T library!\n");
        goto fail;
    }

    /* Provide information about available buffer size to encoding function */
    size_t len = sizeof(ndef_msg_buf);

    /* Encode URI message into buffer */
    err = nfc_ndef_uri_msg_encode( NFC_URI_HTTPS,
                                   m_url,
                                   sizeof(m_url),
                                   ndef_msg_buf,
                                   &len);

    if (err) {
        printk("Cannot encode message!\n");
        goto fail;
    }

    /* Set created message as the NFC payload */
    err = nfc_t2t_payload_set(ndef_msg_buf, len);
    if (err) {
        printk("Cannot set payload!\n");
        goto fail;
    }

    /* Start sensing NFC field */
    err = nfc_t2t_emulation_start();
    if (err) {
        printk("Cannot start emulation!\n");
        goto fail;
    }

    printk("NFC configuration done\n");
    return 0;

fail:
#if CONFIG_REBOOT
    sys_reboot(SYS_REBOOT_COLD);
#endif /* CONFIG_REBOOT */

    return err;
}
