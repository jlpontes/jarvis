find_package(nlohmann_json CONFIG REQUIRED)
find_package(SQLiteCpp CONFIG REQUIRED)

add_library(jarvis_core
    src/database.cpp
    src/registry.cpp
)
add_library(jarvis::core ALIAS jarvis_core)

target_include_directories(jarvis_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jarvis_core
    PUBLIC
        nlohmann_json::nlohmann_json
        SQLiteCpp
)

target_compile_features(jarvis_core PUBLIC cxx_std_20)

jarvis_set_warnings(jarvis_core)
