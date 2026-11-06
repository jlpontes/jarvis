find_package(Catch2 3 CONFIG REQUIRED)

add_executable(jarvis_tests
    core/test_command.cpp
    core/test_database.cpp
    core/test_registry.cpp
    modules/test_clock.cpp
    modules/test_ping.cpp
    modules/test_tasks.cpp
)

target_link_libraries(jarvis_tests
    PRIVATE
        jarvis::core
        jarvis::clock
        jarvis::ping
        jarvis::tasks
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(jarvis_tests)

jarvis_set_warnings(jarvis_tests)
