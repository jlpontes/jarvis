#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "jarvis/command.hpp"
#include "jarvis/module.hpp"

// Módulo de teste: só serve para provar que o contrato funciona
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

int main(int argc, char* argv[]) {
    // 1. Ler o que o usuário digitou
    if (argc < 3) {
        std::cerr << "Uso: jarvis <modulo> <acao> [texto]\n";
        return 1;
    }

    // 2. Transformar em Command
    jarvis::Command cmd;
    cmd.module = argv[1];
    cmd.action = argv[2];
    if (argc >= 4) {
        cmd.args["texto"] = argv[3];
    }

    // 3. Criar o módulo (guardado como IModule!)
    std::unique_ptr<jarvis::IModule> modulo = std::make_unique<PingModule>();

    if (cmd.module != modulo->name()) {
        std::cerr << "Módulo desconhecido: " << cmd.module << "\n";
        return 1;
    }

    // 4. Executar e mostrar a resposta
    jarvis::Response resp = modulo->handle(cmd);

    if (resp.ok) {
        std::cout << resp.message << "\n";
        return 0;
    }

    std::cerr << "Erro: " << resp.message << "\n";
    return 1;
}