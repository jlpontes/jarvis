add_library(jarvis_clock
    src/clock_module.cpp
)
add_library(jarvis::clock ALIAS jarvis_clock)

target_include_directories(jarvis_clock
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jarvis_clock
    PUBLIC jarvis::core
)

jarvis_set_warnings(jarvis_clock)
