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

# set default C standard to use (C99)
set(SAXBOSPIRAL_C_STANDARD "99")
# if env variable LIBSAXBOSPIRAL_C_STANDARD is set and valid, override version
if(DEFINED ENV{LIBSAXBOSPIRAL_C_STANDARD})
    # not a very robust regex but ok for most purposes
    if("$ENV{LIBSAXBOSPIRAL_C_STANDARD}" MATCHES "(99|11)")
        set(SAXBOSPIRAL_C_STANDARD "$ENV{LIBSAXBOSPIRAL_C_STANDARD}")
    endif()
endif()
message(STATUS "C Standard set to C${SAXBOSPIRAL_C_STANDARD}")

project(libsaxbospiral VERSION 0.19.2 LANGUAGES C)
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
        "libsaxbospiral requires a system with an architecture of at least 64 bits!")
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

# dependencies
# add custom dependencies directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
# libpng
find_package(PNG 1 EXACT REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

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
# Link libsaxbospiral with libpng so we get libpng symbols
target_link_libraries(saxbospiral ${PNG_LIBRARY})

add_executable(sxp_test tests.c)

target_link_libraries(sxp_test saxbospiral ${PNG_LIBRARY})

install(
    TARGETS saxbospiral
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
# Install main library header files
install(
    FILES ${LIB_SAXBOSPIRAL_HEADERS}
    DESTINATION include/saxbospiral
)
# Install render_backends header files
install(
    FILES ${LIB_SAXBOSPIRAL_RENDER_BACKENDS_HEADERS}
    DESTINATION include/saxbospiral/render_backends
)

enable_testing()
add_test(unit_tests sxp_test)
