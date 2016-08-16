CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
ENABLE_LANGUAGE(C CXX)
ENABLE_TESTING()

PROJECT(saxbospiral VERSION 0.8.0 LANGUAGES C)

FIND_PACKAGE(PNG REQUIRED)
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})

FILE(
    GLOB LIB_SAXBOSPIRAL_SOURCES
    "saxbospiral/*.c" "saxbospiral/render_backends/*.c"
)
ADD_LIBRARY(saxbospiral SHARED ${LIB_SAXBOSPIRAL_SOURCES})

ADD_EXECUTABLE(sxp_prepare prepare.c)
ADD_EXECUTABLE(sxp_generate generate.c)
ADD_EXECUTABLE(sxp_render render.c)
ADD_EXECUTABLE(sxp_test tests.c)

INCLUDE(GenerateExportHeader)
GENERATE_EXPORT_HEADER(
    saxbospiral
    BASE_NAME saxbospiral
    EXPORT_MACRO_NAME saxbospiral_EXPORTS
    EXPORT_FILE_NAME saxbospiral_export.h
    STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC
)

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
INSTALL(
    PROGRAMS
    sxp_prepare
    sxp_generate
    sxp_render
    DESTINATION bin
)

ADD_TEST(unit_tests sxp_test)
