#pragma once

#include <string>
#include <vector>

#include "jarvis/command.hpp"
#include "jarvis/module.hpp"

namespace jarvis::ping {

// Módulo de teste: responde "pong" e repete texto
class PingModule final : public jarvis::IModule {
public:
    std::string name() const override;
    std::vector<jarvis::ActionInfo> actions() const override;
    jarvis::Response handle(const jarvis::Command& cmd) override;
};

}  // namespace jarvis::ping
