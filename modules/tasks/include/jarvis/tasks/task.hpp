#pragma once

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>

namespace jarvis::tasks {

// Uma tarefa como o resto do programa enxerga (sem nada de SQL aqui)
struct Task {
    std::int64_t id = 0;
    std::string title;
    bool done = false;
    std::string created_at;  // "AAAA-MM-DD HH:MM:SS", hora local
};

// Conversão Task -> JSON. Fica no MESMO namespace do Task para ser achada por ADL:
// quando alguém escreve `nlohmann::json j = task;`, o compilador procura `to_json`
// também em jarvis::tasks (o namespace do argumento) e encontra esta função.
// `inline` porque a função está definida no header: sem ele, cada .cpp que incluir
// este arquivo geraria uma cópia e o linker reclamaria de símbolo duplicado.
inline void to_json(nlohmann::json& j, const Task& task) {
    j = nlohmann::json{
        {"id", task.id},
        {"title", task.title},
        {"done", task.done},
        {"created_at", task.created_at},
    };
}

}  // namespace jarvis::tasks
