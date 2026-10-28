#pragma once

#include <string>
#include <utility>

#include <nlohmann/json.hpp>

namespace jarvis {

// O pedido: "módulo X, faça a ação Y com estes argumentos"
struct Command {
    std::string module;
    std::string action;
    nlohmann::json args = nlohmann::json::object();
};

// A resposta: deu certo? mensagem para o humano + dados para máquinas
struct Response {
    bool ok = true;
    std::string message;
    nlohmann::json data = nullptr;

    static Response success(std::string msg, nlohmann::json data = nullptr) {
        return Response{true, std::move(msg), std::move(data)};
    }

    static Response error(std::string msg) {
        return Response{false, std::move(msg), nullptr};
    }
};

// Descrição de uma ação: vira o "help" e, no futuro, as ferramentas da IA
struct ActionInfo {
    std::string name;
    std::string description;
};

}  // namespace jarvis