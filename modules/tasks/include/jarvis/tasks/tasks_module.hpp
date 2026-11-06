#pragma once

#include <memory>
#include <string>
#include <vector>

#include "jarvis/command.hpp"
#include "jarvis/module.hpp"

namespace jarvis::tasks {

class TaskRepository;  // declaração antecipada: o header público não precisa saber o que tem dentro

// Módulo "tarefas": lista de afazeres guardada no SQLite
class TasksModule final : public jarvis::IModule {
public:
    // Construtor e destrutor DEFINIDOS no .cpp: lá TaskRepository já é um tipo completo,
    // e o unique_ptr precisa disso para saber como destruí-lo.
    TasksModule();
    ~TasksModule() override;

    std::string name() const override;
    std::vector<jarvis::ActionInfo> actions() const override;
    void init(jarvis::Database& db) override;
    jarvis::Response handle(const jarvis::Command& cmd) override;

private:
    jarvis::Response add(const std::string& text);
    jarvis::Response list(const std::string& text);
    jarvis::Response done(const std::string& text);
    jarvis::Response remove(const std::string& text);

    std::unique_ptr<TaskRepository> repo_;  // vazio até init() ser chamado
};

}  // namespace jarvis::tasks
