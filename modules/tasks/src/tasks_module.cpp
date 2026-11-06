#include "jarvis/tasks/tasks_module.hpp"

#include <charconv>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

#include "jarvis/database.hpp"
#include "migrations.hpp"
#include "task_repository.hpp"

namespace jarvis::tasks {

namespace {

// Tira espaços (e tabs/quebras de linha) das pontas
std::string trim(std::string_view text) {
    constexpr std::string_view kSpaces = " \t\r\n";
    const auto first = text.find_first_not_of(kSpaces);
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = text.find_last_not_of(kSpaces);
    return std::string(text.substr(first, last - first + 1));
}

// "3" ou "#3" -> 3. Qualquer outra coisa ("abc", "3abc", "-1", "0", "") -> nullopt.
std::optional<std::int64_t> parse_id(std::string_view text) {
    if (text.starts_with('#')) {
        text.remove_prefix(1);
    }

    std::int64_t value = 0;
    const char* begin = text.data();
    const char* end = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(begin, end, value);

    if (ec != std::errc{} || ptr != end || value <= 0) {
        return std::nullopt;
    }
    return value;
}

// "Tarefa #3 <verbo>: <título>"
std::string summary(const Task& task, const std::string& verb) {
    return "Tarefa #" + std::to_string(task.id) + " " + verb + ": " + task.title;
}

jarvis::Response invalid_id(const std::string& text) {
    return jarvis::Response::error("Id inválido: '" + text + "'. Use um número, ex.: 3 ou #3.");
}

jarvis::Response not_found(std::int64_t id) {
    return jarvis::Response::error("Tarefa #" + std::to_string(id) + " não encontrada.");
}

}  // namespace

TasksModule::TasksModule() = default;
TasksModule::~TasksModule() = default;  // aqui TaskRepository é completo: o unique_ptr sabe destruí-lo

std::string TasksModule::name() const {
    return "tarefas";
}

std::vector<jarvis::ActionInfo> TasksModule::actions() const {
    return {
        {"add", "Adiciona uma tarefa: add <título>"},
        {"listar", "Lista as tarefas pendentes; 'listar todas' inclui as concluídas"},
        {"feito", "Marca uma tarefa como concluída: feito <id>"},
        {"remover", "Apaga uma tarefa: remover <id>"},
    };
}

void TasksModule::init(jarvis::Database& db) {
    db.migrate(name(), 1, migrations::kV1);  // não faz nada se já estiver aplicada
    repo_ = std::make_unique<TaskRepository>(db);
}

jarvis::Response TasksModule::handle(const jarvis::Command& cmd) {
    if (!repo_) {
        return jarvis::Response::error(
            "O módulo de tarefas não foi inicializado (init não foi chamado).");
    }

    const std::string text = trim(cmd.args.value("texto", std::string{}));

    if (cmd.action == "add") {
        return add(text);
    }
    if (cmd.action == "listar") {
        return list(text);
    }
    if (cmd.action == "feito") {
        return done(text);
    }
    if (cmd.action == "remover") {
        return remove(text);
    }
    return jarvis::Response::error("Ação desconhecida: " + cmd.action);
}

jarvis::Response TasksModule::add(const std::string& text) {
    if (text.empty()) {
        return jarvis::Response::error("Diga o título da tarefa, ex.: add Comprar pão");
    }

    const Task task = repo_->add(text);
    return jarvis::Response::success(summary(task, "adicionada"), task);
}

jarvis::Response TasksModule::list(const std::string& text) {
    if (!text.empty() && text != "todas") {
        return jarvis::Response::error("Use: listar [todas]");
    }
    const bool include_done = (text == "todas");

    const std::vector<Task> tasks = repo_->list(include_done);
    if (tasks.empty()) {
        return jarvis::Response::success(
            include_done ? "Nenhuma tarefa." : "Nenhuma tarefa pendente.",
            nlohmann::json::array());
    }

    std::string message;
    for (const Task& task : tasks) {
        if (!message.empty()) {
            message += "\n";
        }
        message += "#" + std::to_string(task.id) + (task.done ? " [x] " : " [ ] ") + task.title;
    }
    return jarvis::Response::success(message, tasks);  // vector<Task> vira array JSON via to_json
}

jarvis::Response TasksModule::done(const std::string& text) {
    const std::optional<std::int64_t> id = parse_id(text);
    if (!id) {
        return invalid_id(text);
    }

    const std::optional<Task> task = repo_->find(*id);
    if (!task) {
        return not_found(*id);
    }
    if (task->done) {
        return jarvis::Response::error("Tarefa #" + std::to_string(*id) + " já estava concluída.");
    }

    repo_->mark_done(*id);
    return jarvis::Response::success(summary(*task, "concluída"));
}

jarvis::Response TasksModule::remove(const std::string& text) {
    const std::optional<std::int64_t> id = parse_id(text);
    if (!id) {
        return invalid_id(text);
    }

    const std::optional<Task> task = repo_->find(*id);
    if (!task) {
        return not_found(*id);
    }

    repo_->remove(*id);
    return jarvis::Response::success(summary(*task, "removida"));
}

}  // namespace jarvis::tasks
