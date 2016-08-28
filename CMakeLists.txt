# begin basic metadata
cmake_minimum_required(VERSION 3.0)
enable_language(C CXX)

project(saxbospiral VERSION 0.9.0 LANGUAGES C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
# end basic metadata

# pass in version of library as preprocessor definitions
add_definitions(-DSAXBOSPIRAL_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions(-DSAXBOSPIRAL_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_definitions(-DSAXBOSPIRAL_VERSION_PATCH=${PROJECT_VERSION_PATCH})

include(CheckCCompilerFlag)

# used for enabling additional compiler options if supported
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
if(NOT CMAKE_BUILD_TYPE EQUAL CMAKE_C_FLAGS_RELEASE)
    enable_c_compiler_flag_if_supported("-Werror")
endif()

find_package(PNG REQUIRED)
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
# Link libsaxbospiral with libpng so we get libpng symbols
target_link_libraries(saxbospiral ${PNG_LIBRARY})

add_executable(sxp_prepare prepare.c)
add_executable(sxp_generate generate.c)
add_executable(sxp_render render.c)
add_executable(sxp_test tests.c)

target_link_libraries(sxp_prepare saxbospiral ${PNG_LIBRARY})
target_link_libraries(sxp_generate saxbospiral ${PNG_LIBRARY})
target_link_libraries(sxp_render saxbospiral ${PNG_LIBRARY})
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

install(
    PROGRAMS
    sxp_prepare
    sxp_generate
    sxp_render
    DESTINATION bin
)

enable_testing()
add_test(unit_tests sxp_test)
# fetch a shell script runner
find_program(COMMAND_INTERPRETER bash)
# only run functional test if we found bash
if(COMMAND_INTERPRETER)
    add_test(
        NAME func_test COMMAND ${COMMAND_INTERPRETER}
        # each script needs to know the path to each executable it runs
        "func_test.sh" sxp_prepare sxp_generate sxp_render
    )
    add_custom_target(
        build_logo ${COMMAND_INTERPRETER}
        "build_logo.sh" sxp_prepare sxp_generate sxp_render "saxbospiral.png"
    )
else()
    # warn about skipping of functional test script
    message(WARNING "Skipping functional test script, couldn't find Bash Shell")
endif()
