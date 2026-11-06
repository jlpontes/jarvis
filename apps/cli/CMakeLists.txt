add_executable(jarvis main.cpp)

target_link_libraries(jarvis
    PRIVATE
        jarvis::core
        jarvis::clock
        jarvis::ping
        jarvis::tasks
)

jarvis_set_warnings(jarvis)
