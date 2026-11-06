#pragma once

#include <string>
#include <vector>

#include "jarvis/command.hpp"
#include "jarvis/module.hpp"

namespace jarvis::clock {

// Módulo "relogio": data e hora atuais
class ClockModule final : public jarvis::IModule {
public:
    std::string name() const override;
    std::vector<jarvis::ActionInfo> actions() const override;
    jarvis::Response handle(const jarvis::Command& cmd) override;

private:
    jarvis::Response now() const;
};

}  // namespace jarvis::clock
