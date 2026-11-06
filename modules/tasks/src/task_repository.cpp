#include "task_repository.hpp"

#include <stdexcept>

#include <SQLiteCpp/Statement.h>

#include "jarvis/database.hpp"

namespace jarvis::tasks {

namespace {

// Colunas na ordem que from_row espera
constexpr const char* kSelectColumns = "SELECT id, title, done, created_at FROM tasks";

}  // namespace

TaskRepository::TaskRepository(jarvis::Database& db) : db_(db) {}

Task TaskRepository::from_row(SQLite::Statement& row) {
    Task task;
    task.id = row.getColumn(0).getInt64();
    task.title = row.getColumn(1).getString();
    task.done = row.getColumn(2).getInt() != 0;
    task.created_at = row.getColumn(3).getString();
    return task;
}

Task TaskRepository::add(const std::string& title) {
    SQLite::Statement insert(db_.raw(), "INSERT INTO tasks (title) VALUES (?)");
    insert.bind(1, title);  // o valor vai separado do SQL: nada de injeção
    insert.exec();

    std::optional<Task> created = find(db_.raw().getLastInsertRowid());
    if (!created) {
        throw std::runtime_error("tarefa inserida não foi encontrada");
    }
    return *created;
}

std::vector<Task> TaskRepository::list(bool include_done) const {
    const std::string sql = std::string(kSelectColumns) + (include_done ? "" : " WHERE done = 0") +
                            " ORDER BY done, id";

    SQLite::Statement query(db_.raw(), sql);

    std::vector<Task> tasks;
    while (query.executeStep()) {
        tasks.push_back(from_row(query));
    }
    return tasks;
}

std::optional<Task> TaskRepository::find(std::int64_t id) const {
    SQLite::Statement query(db_.raw(), std::string(kSelectColumns) + " WHERE id = ?");
    query.bind(1, id);

    if (!query.executeStep()) {
        return std::nullopt;  // nenhuma linha: a tarefa não existe
    }
    return from_row(query);
}

bool TaskRepository::mark_done(std::int64_t id) {
    SQLite::Statement update(db_.raw(),
                             "UPDATE tasks SET done = 1, done_at = datetime('now', 'localtime') "
                             "WHERE id = ? AND done = 0");
    update.bind(1, id);
    return update.exec() > 0;  // exec() devolve quantas linhas mudaram
}

bool TaskRepository::remove(std::int64_t id) {
    SQLite::Statement del(db_.raw(), "DELETE FROM tasks WHERE id = ?");
    del.bind(1, id);
    return del.exec() > 0;
}

}  // namespace jarvis::tasks
