find_package(nlohmann_json CONFIG REQUIRED)

add_library(jarvis_core
    src/registry.cpp
)
add_library(jarvis::core ALIAS jarvis_core)

target_include_directories(jarvis_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jarvis_core
    PUBLIC nlohmann_json::nlohmann_json
)

target_compile_features(jarvis_core PUBLIC cxx_std_20)