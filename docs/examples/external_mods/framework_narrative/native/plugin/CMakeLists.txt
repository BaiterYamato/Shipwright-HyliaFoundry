cmake_minimum_required(VERSION 3.20)

project(com.sylian.framework.narrative_plugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(plugin SHARED src/plugin.cpp)
target_include_directories(plugin PRIVATE ../sdk)

if(MSVC)
    target_compile_definitions(plugin PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
    target_compile_options(plugin PRIVATE /Zi)
    target_link_options(plugin PRIVATE /DEBUG)
endif()

set_target_properties(plugin PROPERTIES
    PREFIX ""
    OUTPUT_NAME "plugin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../win64"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../win64"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../win64"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../win64"
    PDB_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../win64"
)


