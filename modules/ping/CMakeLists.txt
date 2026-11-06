add_library(jarvis_ping
    src/ping_module.cpp
)
add_library(jarvis::ping ALIAS jarvis_ping)

target_include_directories(jarvis_ping
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jarvis_ping
    PUBLIC jarvis::core
)

jarvis_set_warnings(jarvis_ping)
