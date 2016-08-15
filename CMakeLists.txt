cmake_minimum_required(VERSION 3.0)
enable_language(C CXX)

project(saxbospiral VERSION 0.8.0 LANGUAGES C)

file(GLOB LIB_SAXBOSPIRAL_SOURCES "saxbospiral/*.c" "saxbospiral/render_backends/*.c")
add_library(saxbospiral SHARED ${LIB_SAXBOSPIRAL_SOURCES})

add_executable(sxp_prepare prepare.c)
add_executable(sxp_generate generate.c)
add_executable(sxp_render render.c)
add_executable(sxp_test tests.c)

include (GenerateExportHeader)
generate_export_header(
    saxbospiral
    BASE_NAME saxbospiral
    EXPORT_MACRO_NAME saxbospiral_EXPORTS
    EXPORT_FILE_NAME saxbospiral_export.h
    STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC
)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

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
install(
    PROGRAMS sxp_prepare sxp_generate sxp_render sxp_test DESTINATION bin
)
