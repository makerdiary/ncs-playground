/*
 * Copyright (c) 2016-2023 Makerdiary
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/init.h>
#include <zephyr/kernel.h>

#if defined(CONFIG_BOARD_DONGLE_NRF52840) && !defined(CONFIG_GPIO_AS_PINRESET)

static int pselreset_erase(void)
{
    /* if PSELRESET[0,1] in UICR is connected, nRESET function is exposed on Button 0.
     * To use GPIO function on Button 0, PSELRESET[0,1] in UICR should be erased.
     * This function will erase PSELRESET[0,1] and keep other registers in UICR unchanged.
     */

    NRF_UICR_Type nrf_uicr;
    volatile uint32_t *addr = NULL;

    if (((NRF_UICR->PSELRESET[0] & UICR_PSELRESET_CONNECT_Msk) == (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos)) || 
        ((NRF_UICR->PSELRESET[1] & UICR_PSELRESET_CONNECT_Msk) == (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos))) {

        addr = NRF_UICR->NRFFW;
        for (int i = 0; i < ARRAY_SIZE(nrf_uicr.NRFFW); i++) {
            nrf_uicr.NRFFW[i] = *addr;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                ;
            }
            addr++;
        }

        addr = NRF_UICR->NRFHW;
        for (int i = 0; i < ARRAY_SIZE(nrf_uicr.NRFHW); i++) {
            nrf_uicr.NRFHW[i] = *addr;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                ;
            }
            addr++;
        }

        addr = NRF_UICR->CUSTOMER;
        for (int i = 0; i < ARRAY_SIZE(nrf_uicr.CUSTOMER); i++) {
            nrf_uicr.CUSTOMER[i] = *addr;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                ;
            }
            addr++;
        }

        nrf_uicr.PSELRESET[0] = NRF_UICR->PSELRESET[0];
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            ;
        }

        nrf_uicr.PSELRESET[1] = NRF_UICR->PSELRESET[1];
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            ;
        }

        nrf_uicr.APPROTECT = NRF_UICR->APPROTECT;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            ;
        }

        nrf_uicr.NFCPINS = NRF_UICR->NFCPINS;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            ;
        }

        nrf_uicr.DEBUGCTRL = NRF_UICR->DEBUGCTRL;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            ;
        }

        nrf_uicr.REGOUT0 = NRF_UICR->REGOUT0;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            ;
        }

        // Enable Erase mode
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos; // 0x02; 
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            ;
        }

        // Erase the UICR registers
        NRF_NVMC->ERASEUICR = NVMC_ERASEUICR_ERASEUICR_Erase << NVMC_ERASEUICR_ERASEUICR_Pos; //0x00000001;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            ;
        }

        // Enable WRITE mode
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
            ;
        }

        addr = NRF_UICR->NRFFW;
        for (int i = 0; i < ARRAY_SIZE(nrf_uicr.NRFFW); i++) {
            if (nrf_uicr.NRFFW[i] != 0xFFFFFFFF) {
                *addr = nrf_uicr.NRFFW[i];
                while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                    ;
                }
            }
            addr++;
        }

        addr = NRF_UICR->NRFHW;
        for (int i = 0; i < ARRAY_SIZE(nrf_uicr.NRFHW); i++) {
            if (nrf_uicr.NRFHW[i] != 0xFFFFFFFF) {
                *addr = nrf_uicr.NRFHW[i];
                while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                    ;
                }
            }
            addr++;
        }

        addr = NRF_UICR->CUSTOMER;
        for (int i = 0; i < ARRAY_SIZE(nrf_uicr.CUSTOMER); i++) {
            if (nrf_uicr.CUSTOMER[i] != 0xFFFFFFFF) {
                *addr = nrf_uicr.CUSTOMER[i];
                while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                    ;
                }
            }
            addr++;
        }

        if (nrf_uicr.APPROTECT != 0xFFFFFFFF) {
            NRF_UICR->APPROTECT = nrf_uicr.APPROTECT;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                ;
            }
        }

        if (nrf_uicr.NFCPINS != 0xFFFFFFFF) {
            NRF_UICR->NFCPINS = nrf_uicr.NFCPINS;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                ;
            }
        }

        if (nrf_uicr.DEBUGCTRL != 0xFFFFFFFF) {
            NRF_UICR->DEBUGCTRL = nrf_uicr.DEBUGCTRL;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                ;
            }
        }

        if (nrf_uicr.REGOUT0 != 0xFFFFFFFF) {
            NRF_UICR->REGOUT0 = nrf_uicr.REGOUT0;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
                ;
            }
        }
        
        /* a reset is required for changes to take effect */
        NVIC_SystemReset();
    }

    return 0;
}

SYS_INIT(pselreset_erase, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

#endif /* CONFIG_BOARD_DONGLE_NRF52840 && !CONFIG_GPIO_AS_PINRESET */
