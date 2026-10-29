find_package(Catch2 3 CONFIG REQUIRED)

add_executable(jarvis_tests
    core/test_command.cpp
    core/test_database.cpp
    core/test_registry.cpp
)

target_link_libraries(jarvis_tests
    PRIVATE
        jarvis::core
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(jarvis_tests)