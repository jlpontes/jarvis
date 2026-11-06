#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "jarvis/tasks/task.hpp"

namespace SQLite {
class Statement;
}

namespace jarvis {
class Database;
}

namespace jarvis::tasks {

// Todo o SQL do módulo de tarefas mora aqui (padrão Repository)
class TaskRepository {
public:
    // Guarda só uma referência: quem cria o Database é dono dele e precisa mantê-lo vivo
    explicit TaskRepository(jarvis::Database& db);

    // Insere e devolve a tarefa já com id e created_at preenchidos pelo banco
    Task add(const std::string& title);

    // Pendentes (ou todas, se include_done), pendentes primeiro, em ordem de id
    std::vector<Task> list(bool include_done) const;

    // Busca por id. std::nullopt = não existe.
    std::optional<Task> find(std::int64_t id) const;

    // Marca como concluída. false se não existe ou já estava concluída.
    bool mark_done(std::int64_t id);

    // Apaga. false se não existia.
    bool remove(std::int64_t id);

private:
    static Task from_row(SQLite::Statement& row);

    jarvis::Database& db_;
};

}  // namespace jarvis::tasks
