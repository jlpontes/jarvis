#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "jarvis/command.hpp"
#include "jarvis/database.hpp"
#include "jarvis/module.hpp"
#include "jarvis/registry.hpp"

#include "jarvis/clock/clock_module.hpp"
#include "jarvis/ping/ping_module.hpp"
#include "jarvis/tasks/tasks_module.hpp"

// ---------- Help gerado a partir do registry ----------
void print_help(const jarvis::ModuleRegistry& registry) {
    std::cout << "Uso: jarvis <modulo> <acao> [texto...]\n";
    std::cout << "\nMódulos disponíveis:\n";

    for (const jarvis::IModule* module : registry.list()) {
        std::cout << "\n  " << module->name() << "\n";

        for (const jarvis::ActionInfo& action : module->actions()) {
            std::cout << "    " << action.name << " - " << action.description << "\n";
        }
    }
}

// ---------- Onde o Jarvis guarda seus dados: ~/.jarvis ----------
std::filesystem::path data_dir() {
    const char* home = std::getenv("HOME");

    std::filesystem::path base =
        (home != nullptr) ? std::filesystem::path(home) : std::filesystem::current_path();

    std::filesystem::path dir = base / ".jarvis";
    std::filesystem::create_directories(dir);
    return dir;
}

// ---------- O programa de verdade ----------
int run(int argc, char* argv[]) {
    // 1. Abrir o banco ANTES do registry (a ordem importa!)
    std::filesystem::path db_path = data_dir() / "jarvis.db";
    jarvis::Database db(db_path.string());

    // 2. Registrar e inicializar os módulos
    jarvis::ModuleRegistry registry;
    registry.add(std::make_unique<jarvis::ping::PingModule>());
    registry.add(std::make_unique<jarvis::clock::ClockModule>());
    registry.add(std::make_unique<jarvis::tasks::TasksModule>());
    registry.init_all(db);

    // 3. "jarvis help" ou comando incompleto → mostrar ajuda
    if (argc == 2 && std::string(argv[1]) == "help") {
        print_help(registry);
        return 0;
    }
    if (argc < 3) {
        print_help(registry);
        return 1;
    }

    // 4. Transformar o que foi digitado em Command
    jarvis::Command cmd;
    cmd.module = argv[1];
    cmd.action = argv[2];
    if (argc >= 4) {
        // Junta todas as palavras depois da ação: "add Comprar pão" -> "Comprar pão"
        std::string texto = argv[3];
        for (int i = 4; i < argc; ++i) {
            texto += " ";
            texto += argv[i];
        }
        cmd.args["texto"] = texto;
    }

    // 5. O registry decide quem atende
    jarvis::Response resp = registry.dispatch(cmd);

    if (resp.ok) {
        std::cout << resp.message << "\n";
        return 0;
    }

    std::cerr << "Erro: " << resp.message << "\n";
    return 1;
}

int main(int argc, char* argv[]) {
    try {
        return run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Erro fatal: " << e.what() << "\n";
        return 1;
    }
}
