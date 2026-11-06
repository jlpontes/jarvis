#include <string>

#include <catch2/catch_test_macros.hpp>

#include "jarvis/ping/ping_module.hpp"

namespace {

jarvis::Command make_cmd(std::string action) {
    jarvis::Command cmd;
    cmd.module = "ping";
    cmd.action = std::move(action);
    return cmd;
}

}  // namespace

TEST_CASE("Ping se apresenta com nome e ações", "[ping]") {
    jarvis::ping::PingModule ping;

    REQUIRE(ping.name() == "ping");
    REQUIRE(ping.actions().size() == 2);
}

TEST_CASE("ping responde pong", "[ping]") {
    jarvis::ping::PingModule ping;

    jarvis::Response resp = ping.handle(make_cmd("ping"));

    REQUIRE(resp.ok);
    REQUIRE(resp.message == "pong");
}

TEST_CASE("echo repete o texto recebido", "[ping]") {
    jarvis::ping::PingModule ping;
    jarvis::Command cmd = make_cmd("echo");
    cmd.args["texto"] = "olá mundo";

    jarvis::Response resp = ping.handle(cmd);

    REQUIRE(resp.ok);
    REQUIRE(resp.message == "olá mundo");
}

TEST_CASE("echo sem texto devolve mensagem vazia", "[ping]") {
    jarvis::ping::PingModule ping;

    jarvis::Response resp = ping.handle(make_cmd("echo"));

    REQUIRE(resp.ok);
    REQUIRE(resp.message.empty());
}

TEST_CASE("Ping recusa ação desconhecida", "[ping]") {
    jarvis::ping::PingModule ping;

    jarvis::Response resp = ping.handle(make_cmd("pong"));

    REQUIRE_FALSE(resp.ok);
    REQUIRE(resp.message == "Ação desconhecida: pong");
}
