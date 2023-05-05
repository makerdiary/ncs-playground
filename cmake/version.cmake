# Copyright (c) 2016-2023 Makerdiary
#
# SPDX-License-Identifier: Apache-2.0
#

file(STRINGS ${NCSP_DIR}/VERSION NCSP_VERSION LIMIT_COUNT 1 LENGTH_MINIMUM 5)
string(REGEX MATCH "([^\.]*)\.([^\.]*)\.([^-]*)[-]?(.*)" OUT_VAR ${NCSP_VERSION})

set(NCSP_VERSION_MAJOR ${CMAKE_MATCH_1})
set(NCSP_VERSION_MINOR ${CMAKE_MATCH_2})
set(NCSP_VERSION_PATCH ${CMAKE_MATCH_3})
set(NCSP_VERSION_EXTRA ${CMAKE_MATCH_4})

math(EXPR NCSP_VERSION_CODE "(${NCSP_VERSION_MAJOR} << 16) + (${NCSP_VERSION_MINOR} << 8)  + (${NCSP_VERSION_PATCH})")

# to_hex is made available by ${ZEPHYR_BASE}/cmake/hex.cmake
to_hex(${NCSP_VERSION_CODE} NCSP_VERSION_NUMBER)

configure_file(${NCSP_DIR}/ncsp_version.h.in ${ZEPHYR_BINARY_DIR}/include/generated/ncsp_version.h)
