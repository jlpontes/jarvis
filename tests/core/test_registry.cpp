#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "jarvis/database.hpp"
#include "jarvis/registry.hpp"

namespace {

// Módulo falso: existe só para os testes
class FakeModule : public jarvis::IModule {
public:
    explicit FakeModule(std::string name) : name_(std::move(name)) {}

    std::string name() const override {
        return name_;
    }

    std::vector<jarvis::ActionInfo> actions() const override {
        return {
            {"ola", "Diz olá"},
            {"quebrar", "Lança uma exceção de propósito"},
        };
    }
    void init(jarvis::Database& /*db*/) override {
        ++inits;
    }
    jarvis::Response handle(const jarvis::Command& cmd) override {
        ++calls;

        if (cmd.action == "ola") {
            return jarvis::Response::success("olá de " + name_);
        }
        if (cmd.action == "quebrar") {
            throw std::runtime_error("bug proposital");
        }
        return jarvis::Response::error("Ação desconhecida: " + cmd.action);
    }

    int calls = 0;  // quantas vezes handle() foi chamado
    int inits = 0;  // quantas vezes init() foi chamado
private:
    std::string name_;
};

// Módulo que NÃO sobrescreve init(): usa a implementação padrão
class NoInitModule : public jarvis::IModule {
public:
    std::string name() const override {
        return "sem_init";
    }

    std::vector<jarvis::ActionInfo> actions() const override {
        return {};
    }

    jarvis::Response handle(const jarvis::Command& /*cmd*/) override {
        return jarvis::Response::success("ok");
    }
};

jarvis::Command make_cmd(std::string module, std::string action) {
    jarvis::Command cmd;
    cmd.module = std::move(module);
    cmd.action = std::move(action);
    return cmd;
}

}  // namespace

TEST_CASE("Registry aceita módulos com nomes diferentes", "[registry]") {
    jarvis::ModuleRegistry registry;

    REQUIRE(registry.add(std::make_unique<FakeModule>("a")));
    REQUIRE(registry.add(std::make_unique<FakeModule>("b")));

    REQUIRE(registry.has("a"));
    REQUIRE(registry.has("b"));
    REQUIRE_FALSE(registry.has("c"));
}

TEST_CASE("Registry recusa nome duplicado e ponteiro vazio", "[registry]") {
    jarvis::ModuleRegistry registry;

    REQUIRE(registry.add(std::make_unique<FakeModule>("a")));
    REQUIRE_FALSE(registry.add(std::make_unique<FakeModule>("a")));
    REQUIRE_FALSE(registry.add(nullptr));

    REQUIRE(registry.list().size() == 1);
}

TEST_CASE("dispatch entrega o comando ao módulo certo", "[registry]") {
    jarvis::ModuleRegistry registry;

    auto a = std::make_unique<FakeModule>("a");
    FakeModule* espiao_a = a.get();  // guardamos um "olho" antes de entregar a posse
    registry.add(std::move(a));

    auto b = std::make_unique<FakeModule>("b");
    FakeModule* espiao_b = b.get();
    registry.add(std::move(b));

    jarvis::Response resp = registry.dispatch(make_cmd("a", "ola"));

    REQUIRE(resp.ok);
    REQUIRE(resp.message == "olá de a");
    REQUIRE(espiao_a->calls == 1);  // "a" foi chamado
    REQUIRE(espiao_b->calls == 0);  // "b" não foi incomodado
}

TEST_CASE("dispatch para módulo inexistente devolve erro", "[registry]") {
    jarvis::ModuleRegistry registry;
    registry.add(std::make_unique<FakeModule>("a"));

    jarvis::Response resp = registry.dispatch(make_cmd("xyz", "ola"));

    REQUIRE_FALSE(resp.ok);
    REQUIRE(resp.message == "Módulo desconhecido: xyz");
}

TEST_CASE("dispatch captura exceções lançadas pelo módulo", "[registry]") {
    jarvis::ModuleRegistry registry;
    registry.add(std::make_unique<FakeModule>("a"));

    jarvis::Response resp = registry.dispatch(make_cmd("a", "quebrar"));

    REQUIRE_FALSE(resp.ok);
    REQUIRE(resp.message.find("bug proposital") != std::string::npos);
}

TEST_CASE("list devolve os módulos em ordem alfabética", "[registry]") {
    jarvis::ModuleRegistry registry;
    registry.add(std::make_unique<FakeModule>("c"));
    registry.add(std::make_unique<FakeModule>("a"));
    registry.add(std::make_unique<FakeModule>("b"));

    std::vector<std::string> nomes;
    for (const jarvis::IModule* module : registry.list()) {
        nomes.push_back(module->name());
    }

    REQUIRE(nomes == std::vector<std::string>{"a", "b", "c"});
}

TEST_CASE("init_all inicializa todos os módulos uma vez", "[registry]") {
    jarvis::Database db(":memory:");
    jarvis::ModuleRegistry registry;

    auto a = std::make_unique<FakeModule>("a");
    FakeModule* espiao_a = a.get();
    registry.add(std::move(a));

    auto b = std::make_unique<FakeModule>("b");
    FakeModule* espiao_b = b.get();
    registry.add(std::move(b));

    registry.add(std::make_unique<NoInitModule>());  // usa o init() padrão (vazio)

    registry.init_all(db);

    REQUIRE(espiao_a->inits == 1);
    REQUIRE(espiao_b->inits == 1);
}
