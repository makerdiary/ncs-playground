# Copyright (c) 2016-2023 Makerdiary
#
# SPDX-License-Identifier: Apache-2.0
#

file(STRINGS ${CMPRJ_DIR}/VERSION CMPRJ_VERSION LIMIT_COUNT 1 LENGTH_MINIMUM 5)
string(REGEX MATCH "([^\.]*)\.([^\.]*)\.([^-]*)[-]?(.*)" OUT_VAR ${CMPRJ_VERSION})

set(CMPRJ_VERSION_MAJOR ${CMAKE_MATCH_1})
set(CMPRJ_VERSION_MINOR ${CMAKE_MATCH_2})
set(CMPRJ_VERSION_PATCH ${CMAKE_MATCH_3})
set(CMPRJ_VERSION_EXTRA ${CMAKE_MATCH_4})

math(EXPR CMPRJ_VERSION_CODE "(${CMPRJ_VERSION_MAJOR} << 16) + (${CMPRJ_VERSION_MINOR} << 8)  + (${CMPRJ_VERSION_PATCH})")

# to_hex is made available by ${ZEPHYR_BASE}/cmake/hex.cmake
to_hex(${CMPRJ_VERSION_CODE} CMPRJ_VERSION_NUMBER)

configure_file(${CMPRJ_DIR}/cmprj_version.h.in ${ZEPHYR_BINARY_DIR}/include/generated/cmprj_version.h)
