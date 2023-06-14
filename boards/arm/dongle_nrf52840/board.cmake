# Copyright (c) 2016-2023 Makerdiary
# SPDX-License-Identifier: Apache-2.0

board_runner_args(pyocd "--target=nrf52840")
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
include(${ZEPHYR_BASE}/boards/common/openocd-nrf5.board.cmake)
