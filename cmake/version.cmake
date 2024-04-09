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
math(EXPR NCSP_VERSION_NUMBER "${NCSP_VERSION_CODE}" OUTPUT_FORMAT HEXADECIMAL)

if(DEFINED BUILD_VERSION)
  set(ncsp_banner_version BUILD_VERSION)
else()
  set(ncsp_banner_version NCSP_BUILD_VERSION)
endif()

add_custom_command(
  OUTPUT ${PROJECT_BINARY_DIR}/include/generated/ncsp_version.h
  COMMAND ${CMAKE_COMMAND} -DZEPHYR_BASE=${ZEPHYR_BASE}
    -DOUT_FILE=${PROJECT_BINARY_DIR}/include/generated/ncsp_version.h
    -DVERSION_TYPE=NCSP
    # We don't want Zephyr to parse the NCSP VERSION file as it is not conforming to Zephyr VERSION
    # format. Pointing to a non-existing file allows us full control from CMake instead.
    -DVERSION_FILE=${NCSP_DIR}/VERSIONFILEDUMMY
    -DNCSP_VERSION_CODE=${NCSP_VERSION_CODE}
    -DNCSP_VERSION_NUMBER=${NCSP_VERSION_NUMBER}
    -DNCSP_VERSION_MAJOR=${NCSP_VERSION_MAJOR}
    -DNCSP_VERSION_MINOR=${NCSP_VERSION_MINOR}
    -DNCSP_PATCHLEVEL=${NCSP_VERSION_PATCH}
    -DNCSP_VERSION_STRING=${NCSP_VERSION}
    -DNCSP_VERSION_CUSTOMIZATION="\#define;BANNER_VERSION;STRINGIFY\(${ncsp_banner_version}\)"
    -P ${ZEPHYR_BASE}/cmake/gen_version_h.cmake
    DEPENDS ${NCSP_DIR}/VERSION ${git_dependency}
)
add_custom_target(ncsp_version_h DEPENDS ${PROJECT_BINARY_DIR}/include/generated/ncsp_version.h)

add_dependencies(version_h ncsp_version_h)