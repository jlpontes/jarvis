cmake_minimum_required(VERSION 3.25)

project(Jarvis
    VERSION 0.1.0
    DESCRIPTION "Assistente pessoal modular"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)

add_subdirectory(core)
add_subdirectory(modules)
add_subdirectory(apps/cli)

option(JARVIS_BUILD_TESTS "Compilar os testes automatizados" ON)

if(JARVIS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
