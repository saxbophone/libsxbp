cmake_minimum_required(VERSION 3.0)
enable_language(C CXX)

project(saxbospiral VERSION 0.8.0 LANGUAGES C)

file(GLOB LIB_SAXBOSPIRAL_SOURCES "saxbospiral/*.c" "saxbospiral/render_backends/*.c")
add_library(saxbospiral SHARED ${LIB_SAXBOSPIRAL_SOURCES})

add_executable(prepare prepare.c)
add_executable(generate generate.c)
add_executable(renderer renderer.c)
add_executable(tests tests.c)

include (GenerateExportHeader)
GENERATE_EXPORT_HEADER(
    saxbospiral
    BASE_NAME saxbospiral
    EXPORT_MACRO_NAME saxbospiral_EXPORTS
    EXPORT_FILE_NAME saxbospiral_export.h
    STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC
)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

target_link_libraries(prepare saxbospiral ${PNG_LIBRARY})
target_link_libraries(generate saxbospiral ${PNG_LIBRARY})
target_link_libraries(renderer saxbospiral ${PNG_LIBRARY})
target_link_libraries(tests saxbospiral ${PNG_LIBRARY})
