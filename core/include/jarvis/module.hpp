#pragma once

#include <string>
#include <vector>

#include "jarvis/command.hpp"

namespace jarvis {

// Contrato que todo módulo do Jarvis precisa cumprir
class IModule {
public:
    virtual ~IModule() = default;

    // Nome único do módulo, ex.: "tasks"
    virtual std::string name() const = 0;

    // Lista do que o módulo sabe fazer
    virtual std::vector<ActionInfo> actions() const = 0;

    // Executa um comando e devolve a resposta
    virtual Response handle(const Command& cmd) = 0;
};

}  // namespace jarvis