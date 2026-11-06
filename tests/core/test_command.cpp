#include <catch2/catch_test_macros.hpp>

#include "jarvis/command.hpp"

TEST_CASE("Response::success cria uma resposta de sucesso", "[command]") {
    jarvis::Response resp = jarvis::Response::success("tudo certo");

    REQUIRE(resp.ok);
    REQUIRE(resp.message == "tudo certo");
    REQUIRE(resp.data.is_null());
}

TEST_CASE("Response::success pode levar dados", "[command]") {
    jarvis::Response resp = jarvis::Response::success("ok", {{"total", 3}});

    REQUIRE(resp.ok);
    REQUIRE(resp.data["total"] == 3);
}

TEST_CASE("Response::error cria uma resposta de erro", "[command]") {
    jarvis::Response resp = jarvis::Response::error("deu ruim");

    REQUIRE_FALSE(resp.ok);
    REQUIRE(resp.message == "deu ruim");
}

TEST_CASE("Command novo começa com args vazio", "[command]") {
    jarvis::Command cmd;

    REQUIRE(cmd.module.empty());
    REQUIRE(cmd.action.empty());
    REQUIRE(cmd.args.is_object());
    REQUIRE(cmd.args.empty());
}
