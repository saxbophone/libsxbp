# This source file forms part of libsaxbospiral, a library which generates
# experimental 2D spiral-like shapes based on input binary data.
#
# This is the CMake build file for libsaxbospiral.
# It requires CMake v3.0 or greater.
#
#
#
# Copyright (C) 2016, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License (version 3),
# as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# begin basic metadata
cmake_minimum_required(VERSION 3.0)

project(libsaxbospiral VERSION 0.22.0 LANGUAGES C)

# set default C standard to use (C99) if not already set
if(NOT DEFINED SAXBOSPIRAL_C_STANDARD)
    set(SAXBOSPIRAL_C_STANDARD "99")
endif()
# if env variable LIBSAXBOSPIRAL_C_STANDARD is set and valid, override version
if(DEFINED ENV{LIBSAXBOSPIRAL_C_STANDARD})
    # not a very robust regex but ok for most purposes
    if("$ENV{LIBSAXBOSPIRAL_C_STANDARD}" MATCHES "(99|11)")
        set(SAXBOSPIRAL_C_STANDARD "$ENV{LIBSAXBOSPIRAL_C_STANDARD}")
    endif()
endif()
message(STATUS "C Standard set to C${SAXBOSPIRAL_C_STANDARD}")
set(CMAKE_C_STANDARD ${SAXBOSPIRAL_C_STANDARD})
set(CMAKE_C_STANDARD_REQUIRED ON)

set(
    SAXBOSPIRAL_VERSION_STRING
    "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
)
set(SAXBOSPIRAL_ESCAPED_VERSION_STRING "\"${SAXBOSPIRAL_VERSION_STRING}\"")
# end basic metadata

# this is a 64-bit project, so check now to make sure we're 64-bit, issue warning if not
if(CMAKE_SIZEOF_VOID_P LESS 8)
    message(
        WARNING
        "It looks like this system's architecture is not at least 64-bit.\n"
        "libsaxbospiral requires a system with an architecture of at least 64 bits!\n"
        "We'll continue trying to compile anyway, be sure to run the unit tests after."
    )
endif()

# pass in version of library as preprocessor definitions
add_definitions(-DSAXBOSPIRAL_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions(-DSAXBOSPIRAL_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_definitions(-DSAXBOSPIRAL_VERSION_PATCH=${PROJECT_VERSION_PATCH})
add_definitions(-DSAXBOSPIRAL_VERSION_STRING=${SAXBOSPIRAL_ESCAPED_VERSION_STRING})

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
    message(STATUS "Warnings Enabled")
    # enable all warnings about 'questionable constructs'
    enable_c_compiler_flag_if_supported("-Wall")
    # issue 'pedantic' warnings for strict ISO compliance
    enable_c_compiler_flag_if_supported("-pedantic")
    # enable 'extra' strict warnings
    enable_c_compiler_flag_if_supported("-Wextra")
    # convert all warnings into errors
    enable_c_compiler_flag_if_supported("-Werror")
endif()

# begin dependencies
# add custom dependencies directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
# libpng
# work out whether we have or have not requested PNG support, or don't care (default)
if(NOT DEFINED SAXBOSPIRAL_PNG_SUPPORT)
    # try and find libpng v1.x, but don't fail if we can't
    message(STATUS "PNG output support will be enabled if possible")
    find_package(PNG 1 EXACT)
    # set SAXBOSPIRAL_PNG_SUPPORT based on value of PNG_FOUND
    if(PNG_FOUND)
        set(SAXBOSPIRAL_PNG_SUPPORT ON)
    else()
        set(SAXBOSPIRAL_PNG_SUPPORT OFF)
    endif()
elseif(SAXBOSPIRAL_PNG_SUPPORT)
    # find libpng v1.x and fail the build if we can't
    message(STATUS "PNG output support explicitly enabled")
    find_package(PNG 1 EXACT REQUIRED)
else()
    # we've explicitly disabled PNG support, so don't include libpng
    # issue a message saying so
    message(STATUS "PNG output support explicitly disabled")
endif()

# include libpng directories and add feature test macro if support is enabled
if(SAXBOSPIRAL_PNG_SUPPORT)
    include_directories(${PNG_INCLUDE_DIR})
    # feature test macro
    add_definitions(-DSAXBOSPIRAL_PNG_SUPPORT)
    # issue message
    message(STATUS "PNG output support enabled")
else()
    # issue message
    message(STATUS "PNG output support disabled")
endif()
# end dependencies

# C source files
file(
    GLOB LIB_SAXBOSPIRAL_SOURCES
    "saxbospiral/*.c" "saxbospiral/render_backends/*.c"
)
# Header files
file(GLOB LIB_SAXBOSPIRAL_HEADERS "saxbospiral/*.h")
# Header files for render_backends subdirectory
file(
    GLOB LIB_SAXBOSPIRAL_RENDER_BACKENDS_HEADERS
    "saxbospiral/render_backends/*.h"
)
add_library(saxbospiral ${LIB_SAXBOSPIRAL_SOURCES})
# set up version for library objects
set_target_properties(
    saxbospiral PROPERTIES VERSION ${SAXBOSPIRAL_VERSION_STRING}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)
# link libsaxbospiral with C math library
target_link_libraries(saxbospiral m)
# Link libsaxbospiral with libpng so we get libpng symbols (if support enabled)
if(SAXBOSPIRAL_PNG_SUPPORT)
    target_link_libraries(saxbospiral ${PNG_LIBRARY})
endif()

add_executable(sxp_test tests.c)

target_link_libraries(sxp_test saxbospiral)

install(
    TARGETS saxbospiral
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Generate rough (nearest major) version-dependent header installation folder
set(
    LIBSAXBOSPIRAL_ROUGH_HEADER_DESTINATION
    "saxbospiral-${PROJECT_VERSION_MAJOR}"
)
# Generate precise (major and minor) version-dependent header installation folder
set(
    LIBSAXBOSPIRAL_PRECISE_HEADER_DESTINATION
    "saxbospiral-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

# Install main library header files, both to rough and precise install locations
install(
    FILES ${LIB_SAXBOSPIRAL_HEADERS}
    DESTINATION "include/${LIBSAXBOSPIRAL_ROUGH_HEADER_DESTINATION}"
)
# Install render_backends header files
install(
    FILES ${LIB_SAXBOSPIRAL_RENDER_BACKENDS_HEADERS}
    DESTINATION "include/${LIBSAXBOSPIRAL_ROUGH_HEADER_DESTINATION}/render_backends"
)

install(
    FILES ${LIB_SAXBOSPIRAL_HEADERS}
    DESTINATION "include/${LIBSAXBOSPIRAL_PRECISE_HEADER_DESTINATION}"
)
# Install render_backends header files
install(
    FILES ${LIB_SAXBOSPIRAL_RENDER_BACKENDS_HEADERS}
    DESTINATION "include/${LIBSAXBOSPIRAL_PRECISE_HEADER_DESTINATION}/render_backends"
)

enable_testing()
add_test(unit_tests sxp_test)
