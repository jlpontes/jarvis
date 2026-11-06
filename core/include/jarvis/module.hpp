#pragma once

#include <string>
#include <vector>

#include "jarvis/command.hpp"

namespace jarvis {

class Database;  // declaração antecipada: "existe uma classe com esse nome"

// Contrato que todo módulo do Jarvis precisa cumprir
class IModule {
public:
    virtual ~IModule() = default;

    // Nome único do módulo, ex.: "tasks"
    virtual std::string name() const = 0;

    // Lista do que o módulo sabe fazer
    virtual std::vector<ActionInfo> actions() const = 0;

    // Chamado uma vez na inicialização: criar tabelas, guardar o banco.
    // Tem implementação padrão (vazia): módulos sem banco não precisam escrever nada.
    virtual void init(Database& /*db*/) {}

    // Executa um comando e devolve a resposta
    virtual Response handle(const Command& cmd) = 0;
};

}  // namespace jarvis
