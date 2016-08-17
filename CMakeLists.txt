CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
ENABLE_LANGUAGE(C CXX)

PROJECT(saxbospiral VERSION 0.8.0 LANGUAGES C)

FIND_PACKAGE(PNG REQUIRED)
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})

# C source files
FILE(
    GLOB LIB_SAXBOSPIRAL_SOURCES
    "saxbospiral/*.c" "saxbospiral/render_backends/*.c"
)
# Header files
FILE(GLOB LIB_SAXBOSPIRAL_HEADERS "saxbospiral/*.h")
# Header files for render_backends subdirectory
FILE(
    GLOB LIB_SAXBOSPIRAL_RENDER_BACKENDS_HEADERS
    "saxbospiral/render_backends/*.h"
)
ADD_LIBRARY(saxbospiral SHARED ${LIB_SAXBOSPIRAL_SOURCES})
# Link libsaxbospiral with libpng so we get libpng symbols
TARGET_LINK_LIBRARIES(saxbospiral ${PNG_LIBRARY})

ADD_EXECUTABLE(sxp_prepare prepare.c)
ADD_EXECUTABLE(sxp_generate generate.c)
ADD_EXECUTABLE(sxp_render render.c)
ADD_EXECUTABLE(sxp_test tests.c)

TARGET_LINK_LIBRARIES(sxp_prepare saxbospiral ${PNG_LIBRARY})
TARGET_LINK_LIBRARIES(sxp_generate saxbospiral ${PNG_LIBRARY})
TARGET_LINK_LIBRARIES(sxp_render saxbospiral ${PNG_LIBRARY})
TARGET_LINK_LIBRARIES(sxp_test saxbospiral ${PNG_LIBRARY})

INSTALL(
    TARGETS saxbospiral
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
# Install main library header files
INSTALL(
    FILES ${LIB_SAXBOSPIRAL_HEADERS}
    DESTINATION include/saxbospiral
)
# Install render_backends header files
INSTALL(
    FILES ${LIB_SAXBOSPIRAL_RENDER_BACKENDS_HEADERS}
    DESTINATION include/saxbospiral/render_backends
)

INSTALL(
    PROGRAMS
    sxp_prepare
    sxp_generate
    sxp_render
    DESTINATION bin
)

ENABLE_TESTING()
ADD_TEST(unit_tests sxp_test)
# fetch a shell script runner
FIND_PROGRAM(COMMAND_INTERPRETER bash)
# only run functional test if we found bash
IF(COMMAND_INTERPRETER)
    ADD_TEST(
        NAME func_test COMMAND ${COMMAND_INTERPRETER}
        # each script needs to know the path to each executable it runs
        "func_test.sh" sxp_prepare sxp_generate sxp_render
    )
    ADD_CUSTOM_TARGET(
        NAME build_logo COMMAND ${COMMAND_INTERPRETER}
        "build_logo.sh" sxp_prepare sxp_generate sxp_render "saxbospiral.png"
    )
ELSE()
    # warn about skipping of functional test script
    message(WARNING "Skipping functional test script, couldn't find Bash Shell")
ENDIF()
