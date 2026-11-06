#include "jarvis/clock/clock_module.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace jarvis::clock {

std::string ClockModule::name() const {
    return "relogio";
}

std::vector<jarvis::ActionInfo> ClockModule::actions() const {
    return {
        {"agora", "Mostra a data e a hora atuais"},
    };
}

jarvis::Response ClockModule::handle(const jarvis::Command& cmd) {
    if (cmd.action == "agora") {
        return now();
    }
    return jarvis::Response::error("Ação desconhecida: " + cmd.action);
}

jarvis::Response ClockModule::now() const {
    const std::time_t current = std::time(nullptr);
    const std::tm local = *std::localtime(&current);

    std::ostringstream text;
    text << "Agora são " << std::put_time(&local, "%H:%M de %d/%m/%Y");

    return jarvis::Response::success(text.str());
}

}  // namespace jarvis::clock
