#include "jarvis/registry.hpp"

#include <exception>
#include <utility>

namespace jarvis {

bool ModuleRegistry::add(std::unique_ptr<IModule> module) {
    if (!module) {
        return false;  // ponteiro vazio: nada para registrar
    }

    const std::string name = module->name();

    if (modules_.contains(name)) {
        return false;  // já existe um módulo com esse nome
    }

    modules_.emplace(name, std::move(module));
    return true;
}

bool ModuleRegistry::has(const std::string& name) const {
    return modules_.contains(name);
}

Response ModuleRegistry::dispatch(const Command& cmd) {
    auto it = modules_.find(cmd.module);

    if (it == modules_.end()) {
        return Response::error("Módulo desconhecido: " + cmd.module);
    }

    try {
        return it->second->handle(cmd);
    } catch (const std::exception& e) {
        return Response::error("Falha no módulo '" + cmd.module + "': " + e.what());
    }
}

std::vector<const IModule*> ModuleRegistry::list() const {
    std::vector<const IModule*> result;
    result.reserve(modules_.size());

    for (const auto& [name, module] : modules_) {
        result.push_back(module.get());
    }

    return result;
}

}  // namespace jarvis