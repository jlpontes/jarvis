#include "jarvis/ping/ping_module.hpp"

namespace jarvis::ping {

std::string PingModule::name() const {
    return "ping";
}

std::vector<jarvis::ActionInfo> PingModule::actions() const {
    return {
        {"ping", "Responde pong"},
        {"echo", "Repete o texto recebido"},
    };
}

jarvis::Response PingModule::handle(const jarvis::Command& cmd) {
    if (cmd.action == "ping") {
        return jarvis::Response::success("pong");
    }
    if (cmd.action == "echo") {
        return jarvis::Response::success(cmd.args.value("texto", std::string{}));
    }
    return jarvis::Response::error("Ação desconhecida: " + cmd.action);
}

}  // namespace jarvis::ping
