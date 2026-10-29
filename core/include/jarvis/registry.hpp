#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "jarvis/command.hpp"
#include "jarvis/module.hpp"

namespace jarvis {

// Guarda os módulos e entrega cada Command ao módulo certo
class ModuleRegistry {
public:
    // Registra um módulo. Retorna false se já existir um com o mesmo nome.
    bool add(std::unique_ptr<IModule> module);

    // Existe um módulo com este nome?
    bool has(const std::string& name) const;

    // Chama init(db) em todos os módulos registrados
    void init_all(Database& db);

    // Entrega o comando ao módulo dono dele
    Response dispatch(const Command& cmd);

    // Lista os módulos registrados (só para leitura)
    std::vector<const IModule*> list() const;

private:
    std::map<std::string, std::unique_ptr<IModule>> modules_;
};

}  // namespace jarvis