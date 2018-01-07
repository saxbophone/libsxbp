# This source file forms part of sxbp, a library which generates experimental
# 2D spiral-like shapes based on input binary data.
#
# This is the CMake build file for sxbp.
# It requires CMake v3.0 or greater.
#
#
#
# Copyright (C) 2016, 2018 Joshua Saxby <joshua.a.saxby@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# begin basic metadata
cmake_minimum_required(VERSION 3.0)

project(sxbp VERSION 0.54.0 LANGUAGES C)

# set default C standard to use (C99) if not already set
if(NOT DEFINED SXBP_C_STANDARD)
    set(SXBP_C_STANDARD "99")
endif()
# if env variable SXBP_C_STANDARD is set and valid, override version
if(DEFINED ENV{SXBP_C_STANDARD})
    # not a very robust regex but ok for most purposes
    if("$ENV{SXBP_C_STANDARD}" MATCHES "(99|11)")
        set(SXBP_C_STANDARD "$ENV{SXBP_C_STANDARD}")
    endif()
endif()
message(STATUS "[sxbp] C Standard set to C${SXBP_C_STANDARD}")
set(CMAKE_C_STANDARD ${SXBP_C_STANDARD})
set(CMAKE_C_STANDARD_REQUIRED ON)

set(
    SXBP_VERSION_STRING
    "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
)
set(SXBP_ESCAPED_VERSION_STRING "\"${SXBP_VERSION_STRING}\"")
# end basic metadata

# must be running on an architecture of at least 32 bits
if(CMAKE_SIZEOF_VOID_P LESS 4)
    message(
        WARNING
        "[sxbp] It looks like this system's architecture is not at least 32-bit.\n"
        "       SXBP requires a system with an architecture of at least 32 bits!\n"
        "       We'll continue trying to compile anyway, but may fail.\n"
        "       Be sure to run the unit tests after!"
    )
endif()

# pass in version of library as preprocessor definitions
add_definitions(-DSXBP_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions(-DSXBP_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_definitions(-DSXBP_VERSION_PATCH=${PROJECT_VERSION_PATCH})
add_definitions(-DSXBP_VERSION_STRING=${SXBP_ESCAPED_VERSION_STRING})

# used for enabling additional compiler options if supported
include(CheckCCompilerFlag)

function(enable_c_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_C_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_c_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
    endif()
endfunction()

# enable extra flags (warnings) if we're not in release mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "")
    message(STATUS "[sxbp] Warnings Enabled")
    # enable all warnings about 'questionable constructs'
    enable_c_compiler_flag_if_supported("-Wall")
    # issue 'pedantic' warnings for strict ISO compliance
    enable_c_compiler_flag_if_supported("-pedantic")
    # enable 'extra' strict warnings
    enable_c_compiler_flag_if_supported("-Wextra")
    # enable warnings on missing prototypes of global functions
    enable_c_compiler_flag_if_supported("-Wmissing-prototypes")
    # convert all warnings into errors
    enable_c_compiler_flag_if_supported("-Werror")
endif()

# begin dependencies
# add custom dependencies directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
# libpng
# work out whether we have or have not requested PNG support, or don't care (default)
if(NOT DEFINED SXBP_PNG_SUPPORT)
    # try and find libpng v1.x, but don't fail if we can't
    message(STATUS "[sxbp] PNG output support will be enabled if possible")
    find_package(PNG 1 EXACT)
    # set SXBP_PNG_SUPPORT based on value of PNG_FOUND
    if(PNG_FOUND)
        set(SXBP_PNG_SUPPORT ON)
    else()
        set(SXBP_PNG_SUPPORT OFF)
    endif()
elseif(SXBP_PNG_SUPPORT)
    # find libpng v1.x and fail the build if we can't
    message(STATUS "[sxbp] PNG output support explicitly enabled")
    find_package(PNG 1 EXACT REQUIRED)
else()
    # we've explicitly disabled PNG support, so don't include libpng
    # issue a message saying so
    message(STATUS "[sxbp] PNG output support explicitly disabled")
endif()

# include libpng directories and add feature test macro if support is enabled
if(SXBP_PNG_SUPPORT)
    include_directories(${PNG_INCLUDE_DIR})
    # feature test macro
    add_definitions(-DSXBP_PNG_SUPPORT)
    # issue message
    message(STATUS "[sxbp] PNG output support enabled")
else()
    # issue message
    message(STATUS "[sxbp] PNG output support disabled")
endif()
# end dependencies

# C source files
file(GLOB SXBP_SOURCES "sxbp/*.c")
# Header files
file(GLOB SXBP_HEADERS "sxbp/*.h")

add_library(sxbp ${SXBP_SOURCES})
# set up version for library objects
set_target_properties(
    sxbp PROPERTIES VERSION ${SXBP_VERSION_STRING}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)
# link SXBP with C math library
target_link_libraries(sxbp m)
# Link SXBP with libpng so we get libpng symbols (if support enabled)
if(SXBP_PNG_SUPPORT)
    target_link_libraries(sxbp ${PNG_LIBRARY})
endif()

add_executable(sxp_test tests.c)

target_link_libraries(sxp_test sxbp)

install(
    TARGETS sxbp
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Generate rough (nearest major) version-dependent header installation folder
set(
    SXBP_ROUGH_HEADER_DESTINATION
    "sxbp-${PROJECT_VERSION_MAJOR}"
)
# Generate precise (major and minor) version-dependent header installation folder
set(
    SXBP_PRECISE_HEADER_DESTINATION
    "sxbp-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

# Install main library header files, both to rough and precise install locations
install(
    FILES ${SXBP_HEADERS}
    DESTINATION "include/${SXBP_ROUGH_HEADER_DESTINATION}"
)
# Install render_backends header files
install(
    FILES ${SXBP_RENDER_BACKENDS_HEADERS}
    DESTINATION "include/${SXBP_ROUGH_HEADER_DESTINATION}/render_backends"
)

install(
    FILES ${SXBP_HEADERS}
    DESTINATION "include/${SXBP_PRECISE_HEADER_DESTINATION}"
)
# Install render_backends header files
install(
    FILES ${SXBP_RENDER_BACKENDS_HEADERS}
    DESTINATION "include/${SXBP_PRECISE_HEADER_DESTINATION}/render_backends"
)

enable_testing()
add_test(unit_tests sxp_test)
