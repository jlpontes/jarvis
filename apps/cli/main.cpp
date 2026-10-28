#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "jarvis/command.hpp"
#include "jarvis/module.hpp"
#include "jarvis/registry.hpp"

// ---------- Módulo 1: ping (teste) ----------
class PingModule : public jarvis::IModule {
public:
    std::string name() const override {
        return "ping";
    }

    std::vector<jarvis::ActionInfo> actions() const override {
        return {
            {"ping", "Responde pong"},
            {"echo", "Repete o texto recebido"},
        };
    }

    jarvis::Response handle(const jarvis::Command& cmd) override {
        if (cmd.action == "ping") {
            return jarvis::Response::success("pong");
        }
        if (cmd.action == "echo") {
            std::string texto = cmd.args.value("texto", std::string{});
            return jarvis::Response::success(texto);
        }
        return jarvis::Response::error("Ação desconhecida: " + cmd.action);
    }
};

// ---------- Módulo 2: relógio ----------
class ClockModule : public jarvis::IModule {
public:
    std::string name() const override {
        return "relogio";
    }

    std::vector<jarvis::ActionInfo> actions() const override {
        return {
            {"agora", "Mostra a data e a hora atuais"},
        };
    }

    jarvis::Response handle(const jarvis::Command& cmd) override {
        if (cmd.action == "agora") {
            std::time_t agora = std::time(nullptr);
            std::tm local = *std::localtime(&agora);

            std::ostringstream texto;
            texto << "Agora são " << std::put_time(&local, "%H:%M de %d/%m/%Y");

            return jarvis::Response::success(texto.str());
        }
        return jarvis::Response::error("Ação desconhecida: " + cmd.action);
    }
};

// ---------- Help gerado a partir do registry ----------
void print_help(const jarvis::ModuleRegistry& registry) {
    std::cout << "Uso: jarvis <modulo> <acao> [texto]\n";
    std::cout << "\nMódulos disponíveis:\n";

    for (const jarvis::IModule* module : registry.list()) {
        std::cout << "\n  " << module->name() << "\n";

        for (const jarvis::ActionInfo& action : module->actions()) {
            std::cout << "    " << action.name << " - " << action.description << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    // 1. Registrar os módulos (uma vez só)
    jarvis::ModuleRegistry registry;
    registry.add(std::make_unique<PingModule>());
    registry.add(std::make_unique<ClockModule>());

    // 2. "jarvis help" ou comando incompleto → mostrar ajuda
    if (argc == 2 && std::string(argv[1]) == "help") {
        print_help(registry);
        return 0;
    }
    if (argc < 3) {
        print_help(registry);
        return 1;
    }

    // 3. Transformar o que foi digitado em Command
    jarvis::Command cmd;
    cmd.module = argv[1];
    cmd.action = argv[2];
    if (argc >= 4) {
        cmd.args["texto"] = argv[3];
    }

    // 4. O registry decide quem atende
    jarvis::Response resp = registry.dispatch(cmd);

    if (resp.ok) {
        std::cout << resp.message << "\n";
        return 0;
    }

    std::cerr << "Erro: " << resp.message << "\n";
    return 1;
}