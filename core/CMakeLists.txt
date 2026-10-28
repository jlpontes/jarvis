find_package(nlohmann_json CONFIG REQUIRED)

add_library(jarvis_core INTERFACE)
add_library(jarvis::core ALIAS jarvis_core)

target_include_directories(jarvis_core
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jarvis_core
    INTERFACE nlohmann_json::nlohmann_json
)

target_compile_features(jarvis_core INTERFACE cxx_std_20)