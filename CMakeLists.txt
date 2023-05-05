# Copyright (c) 2016-2023 Makerdiary
# SPDX-License-Identifier: Apache-2.0
#
# This CMake file is picked by the Zephyr build system because it is defined
# as the module CMake entry point (see zephyr/module.yml).

# Point to NCS Playground root directory.
set(NCSP_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "NCS Playground root directory")

include(cmake/version.cmake)
