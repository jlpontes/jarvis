#include <regex>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "jarvis/clock/clock_module.hpp"

namespace {

jarvis::Command make_cmd(std::string action) {
    jarvis::Command cmd;
    cmd.module = "relogio";
    cmd.action = std::move(action);
    return cmd;
}

}  // namespace

TEST_CASE("Relógio se apresenta com nome e ações", "[clock]") {
    jarvis::clock::ClockModule clock;

    REQUIRE(clock.name() == "relogio");
    REQUIRE(clock.actions().size() == 1);
}

TEST_CASE("agora devolve a hora no formato esperado", "[clock]") {
    jarvis::clock::ClockModule clock;

    jarvis::Response resp = clock.handle(make_cmd("agora"));

    // A hora muda a cada execução: conferimos o FORMATO, não o valor
    const std::regex format(R"(Agora são \d{2}:\d{2} de \d{2}/\d{2}/\d{4})");
    INFO("mensagem: " << resp.message);
    REQUIRE(resp.ok);
    REQUIRE(std::regex_match(resp.message, format));
}

TEST_CASE("Relógio recusa ação desconhecida", "[clock]") {
    jarvis::clock::ClockModule clock;

    jarvis::Response resp = clock.handle(make_cmd("ontem"));

    REQUIRE_FALSE(resp.ok);
    REQUIRE(resp.message == "Ação desconhecida: ontem");
}
