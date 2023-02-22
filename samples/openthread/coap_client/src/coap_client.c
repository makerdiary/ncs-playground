/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 * Copyright (c) 2016-2023 Makerdiary
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/kernel.h>
#include <dk_buttons_and_leds.h>
#include <zephyr/logging/log.h>
#include <ram_pwrdn.h>
#include <zephyr/device.h>
#include <zephyr/pm/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>

#include "coap_client_utils.h"

#if CONFIG_BT_NUS
#include "ble_utils.h"
#endif

LOG_MODULE_REGISTER(coap_client, CONFIG_COAP_CLIENT_LOG_LEVEL);

#define OT_CONNECTION_LED    2   /* RGB LED - Green */
#define BLE_CONNECTION_LED   3   /* RGB LED - Blue */
#define MTD_SED_LED          0   /* Green LED */

#if CONFIG_BT_NUS

#define COMMAND_REQUEST_UNICAST      'u'
#define COMMAND_REQUEST_MULTICAST    'm'
#define COMMAND_REQUEST_PROVISIONING 'p'

static void on_nus_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
    LOG_INF("Received data: %c", data[0]);

    switch (*data) {
    case COMMAND_REQUEST_UNICAST:
        coap_client_toggle_one_light();
        break;

    case COMMAND_REQUEST_MULTICAST:
        coap_client_toggle_mesh_lights();
        break;

    case COMMAND_REQUEST_PROVISIONING:
        coap_client_send_provisioning_request();
        break;

    default:
        LOG_WRN("Received invalid data from NUS");
    }
}

static void on_ble_connect(struct k_work *item)
{
    ARG_UNUSED(item);

    dk_set_led_on(BLE_CONNECTION_LED);
}

static void on_ble_disconnect(struct k_work *item)
{
    ARG_UNUSED(item);

    dk_set_led_off(BLE_CONNECTION_LED);
}

#endif /* CONFIG_BT_NUS */

static void on_ot_connect(struct k_work *item)
{
    ARG_UNUSED(item);

    dk_set_led_on(OT_CONNECTION_LED);
}

static void on_ot_disconnect(struct k_work *item)
{
    ARG_UNUSED(item);

    dk_set_led_off(OT_CONNECTION_LED);
}

static void on_mtd_mode_toggle(uint32_t med)
{
#if IS_ENABLED(CONFIG_PM_DEVICE)
    const struct device *cons = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

    if (!device_is_ready(cons)) {
        return;
    }

    if (med) {
        pm_device_action_run(cons, PM_DEVICE_ACTION_RESUME);
    } else {
        pm_device_action_run(cons, PM_DEVICE_ACTION_SUSPEND);
    }
#endif
    dk_set_led(MTD_SED_LED, med);
}

static void on_button_changed(uint32_t button_state, uint32_t has_changed)
{
    uint32_t buttons = button_state & has_changed;

    if (buttons & DK_BTN1_MSK) {
        coap_client_toggle_minimal_sleepy_end_device();
    }

}

void main(void)
{
    int ret;

#if DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_shell_uart), zephyr_cdc_acm_uart)
    const struct device *dev;
    uint32_t dtr = 0U;

    ret = usb_enable(NULL);
    if (ret != 0) {
        LOG_ERR("Failed to enable USB");
        return;
    }

    dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));
    if (dev == NULL) {
        LOG_ERR("Failed to find specific UART device");
        return;
    }

    LOG_INF("Waiting for host to be ready to communicate");

    /* Data Terminal Ready - check if host is ready to communicate */
    while (!dtr) {
        ret = uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
        if (ret) {
            LOG_ERR("Failed to get Data Terminal Ready line state: %d",
                ret);
            continue;
        }
        k_msleep(100);
    }

    /* Data Carrier Detect Modem - mark connection as established */
    (void)uart_line_ctrl_set(dev, UART_LINE_CTRL_DCD, 1);
    /* Data Set Ready - the NCP SoC is ready to communicate */
    (void)uart_line_ctrl_set(dev, UART_LINE_CTRL_DSR, 1);
#endif

    LOG_INF("Start CoAP-client sample");

    if (IS_ENABLED(CONFIG_RAM_POWER_DOWN_LIBRARY)) {
        power_down_unused_ram();
    }

    ret = dk_buttons_init(on_button_changed);
    if (ret) {
        LOG_ERR("Cannot init buttons (error: %d)", ret);
        return;
    }

    ret = dk_leds_init();
    if (ret) {
        LOG_ERR("Cannot init leds, (error: %d)", ret);
        return;
    }

#if CONFIG_BT_NUS
    struct bt_nus_cb nus_clbs = {
        .received = on_nus_received,
        .sent = NULL,
    };

    ret = ble_utils_init(&nus_clbs, on_ble_connect, on_ble_disconnect);
    if (ret) {
        LOG_ERR("Cannot init BLE utilities");
        return;
    }

#endif /* CONFIG_BT_NUS */

    coap_client_utils_init(on_ot_connect, on_ot_disconnect, on_mtd_mode_toggle);
}
