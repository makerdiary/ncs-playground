/*
 * Copyright (c) 2016-2023 Makerdiary
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#define VSYS_NODE   DT_PATH(power_vsys_in)

#if !DT_NODE_EXISTS(VSYS_NODE) || \
    !DT_NODE_HAS_PROP(VSYS_NODE, io_channels) || \
    !DT_NODE_HAS_PROP(VSYS_NODE, output_ohms) || \
    !DT_NODE_HAS_PROP(VSYS_NODE, full_ohms) || \
    !DT_NODE_HAS_PROP(VSYS_NODE, power_gpios)
#error "No suitable devicetree specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
    ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

#define VOLTAGE_DIVIDER_OUTPUT_OHM    DT_PROP(VSYS_NODE, output_ohms)
#define VOLTAGE_DIVIDER_FULL_OHM      DT_PROP(VSYS_NODE, full_ohms)

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
    DT_FOREACH_PROP_ELEM(VSYS_NODE, io_channels,
                 DT_SPEC_AND_COMMA)
};

static const struct gpio_dt_spec power_gpio = GPIO_DT_SPEC_GET(VSYS_NODE, power_gpios);

int main(void)
{
    int err;
    int16_t buf;
    struct adc_sequence sequence = {
        .buffer = &buf,
        /* buffer size in bytes, not number of samples */
        .buffer_size = sizeof(buf),
    };

    err = gpio_pin_configure_dt(&power_gpio, GPIO_OUTPUT_ACTIVE);
    if (err < 0) {
        return 0;
    }

    err = gpio_pin_set_dt(&power_gpio, 1);
    if (err < 0) {
        return 0;
    }    


    /* Configure channels individually prior to sampling. */
    for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
        if (!device_is_ready(adc_channels[i].dev)) {
            printk("ADC controller device not ready\n");
            return 0;
        }

        err = adc_channel_setup_dt(&adc_channels[i]);
        if (err < 0) {
            printk("Could not setup channel #%d (%d)\n", i, err);
            return 0;
        }
    }

    while (1) {
        printk("ADC reading:\n");
        for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
            int32_t val_mv;

            printk("- %s, channel %d: ",
                   adc_channels[i].dev->name,
                   adc_channels[i].channel_id);

            (void)adc_sequence_init_dt(&adc_channels[i], &sequence);

            err = adc_read(adc_channels[i].dev, &sequence);
            if (err < 0) {
                printk("Could not read (%d)\n", err);
                continue;
            } else {
                printk("%"PRId16, buf);
            }

            /* conversion to mV may not be supported, skip if not */
            val_mv = buf;
            err = adc_raw_to_millivolts_dt(&adc_channels[i],
                               &val_mv);
            if (err < 0) {
                printk(" (value in mV not available)\n");
            } else {
                int32_t result = (int32_t)((int64_t)(val_mv) * VOLTAGE_DIVIDER_FULL_OHM / VOLTAGE_DIVIDER_OUTPUT_OHM);
                printk(" = %"PRId32" mV, VSYS = %"PRId32" mV\n", val_mv, result);
            }
        }

        k_sleep(K_MSEC(1000));
    }
    return 0;
}
