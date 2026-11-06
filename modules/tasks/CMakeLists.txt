add_library(jarvis_tasks
    src/task_repository.cpp
    src/tasks_module.cpp
)
add_library(jarvis::tasks ALIAS jarvis_tasks)

target_include_directories(jarvis_tasks
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(jarvis_tasks
    PUBLIC jarvis::core
)

jarvis_set_warnings(jarvis_tasks)
