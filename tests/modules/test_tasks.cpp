#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <SQLiteCpp/Statement.h>

#include "jarvis/database.hpp"
#include "jarvis/tasks/tasks_module.hpp"

namespace {

jarvis::Command make_cmd(std::string action, std::string texto = {}) {
    jarvis::Command cmd;
    cmd.module = "tarefas";
    cmd.action = std::move(action);
    if (!texto.empty()) {
        cmd.args["texto"] = std::move(texto);
    }
    return cmd;
}

bool table_exists(jarvis::Database& db, const std::string& table) {
    SQLite::Statement query(db.raw(),
        "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = ?");
    query.bind(1, table);
    query.executeStep();
    return query.getColumn(0).getInt() == 1;
}

}  // namespace

TEST_CASE("Sem init o módulo devolve erro amigável", "[tasks]") {
    jarvis::tasks::TasksModule tasks;

    jarvis::Response resp = tasks.handle(make_cmd("listar"));

    REQUIRE_FALSE(resp.ok);
    REQUIRE(resp.message.find("não foi inicializado") != std::string::npos);
}

TEST_CASE("init pode ser chamado mais de uma vez", "[tasks]") {
    jarvis::Database db(":memory:");  // o banco vem ANTES: precisa viver mais que o módulo
    jarvis::tasks::TasksModule tasks;

    tasks.init(db);
    REQUIRE_NOTHROW(tasks.init(db));

    REQUIRE(db.schema_version("tarefas") == 1);
    REQUIRE(tasks.handle(make_cmd("listar")).ok);
}

TEST_CASE("Ciclo completo: add, listar, feito, remover", "[tasks]") {
    jarvis::Database db(":memory:");
    jarvis::tasks::TasksModule tasks;
    tasks.init(db);

    jarvis::Response added = tasks.handle(make_cmd("add", "Comprar pão"));
    REQUIRE(added.ok);
    REQUIRE(added.message == "Tarefa #1 adicionada: Comprar pão");
    REQUIRE(added.data["id"] == 1);
    REQUIRE(added.data["title"] == "Comprar pão");
    REQUIRE(added.data["done"] == false);
    REQUIRE_FALSE(added.data["created_at"].get<std::string>().empty());

    REQUIRE(tasks.handle(make_cmd("add", "Estudar C++")).ok);

    jarvis::Response pending = tasks.handle(make_cmd("listar"));
    REQUIRE(pending.ok);
    REQUIRE(pending.message == "#1 [ ] Comprar pão\n#2 [ ] Estudar C++");
    REQUIRE(pending.data.size() == 2);

    jarvis::Response done = tasks.handle(make_cmd("feito", "1"));
    REQUIRE(done.ok);
    REQUIRE(done.message == "Tarefa #1 concluída: Comprar pão");

    // Concluída some da lista padrão...
    pending = tasks.handle(make_cmd("listar"));
    REQUIRE(pending.message == "#2 [ ] Estudar C++");

    // ...mas aparece em "todas", depois das pendentes
    jarvis::Response all = tasks.handle(make_cmd("listar", "todas"));
    REQUIRE(all.message == "#2 [ ] Estudar C++\n#1 [x] Comprar pão");
    REQUIRE(all.data[1]["done"] == true);

    REQUIRE(tasks.handle(make_cmd("remover", "#1")).ok);
    REQUIRE(tasks.handle(make_cmd("remover", "2")).ok);

    all = tasks.handle(make_cmd("listar", "todas"));
    REQUIRE(all.ok);
    REQUIRE(all.message == "Nenhuma tarefa.");
    REQUIRE(all.data.empty());
}

TEST_CASE("add recusa título vazio", "[tasks]") {
    jarvis::Database db(":memory:");
    jarvis::tasks::TasksModule tasks;
    tasks.init(db);

    REQUIRE_FALSE(tasks.handle(make_cmd("add")).ok);
    REQUIRE_FALSE(tasks.handle(make_cmd("add", "   ")).ok);
    REQUIRE(tasks.handle(make_cmd("listar")).message == "Nenhuma tarefa pendente.");
}

TEST_CASE("listar recusa argumento desconhecido", "[tasks]") {
    jarvis::Database db(":memory:");
    jarvis::tasks::TasksModule tasks;
    tasks.init(db);

    jarvis::Response resp = tasks.handle(make_cmd("listar", "algumas"));
    REQUIRE_FALSE(resp.ok);
    REQUIRE(resp.message == "Use: listar [todas]");
}

TEST_CASE("Ids inválidos ou inexistentes dão erro", "[tasks]") {
    jarvis::Database db(":memory:");
    jarvis::tasks::TasksModule tasks;
    tasks.init(db);
    REQUIRE(tasks.handle(make_cmd("add", "Uma tarefa")).ok);

    for (const std::string action : {"feito", "remover"}) {
        for (const std::string id : {"", "abc", "3abc", "-1", "0", "#", "##1", "1.5"}) {
            INFO(action << " '" << id << "'");
            jarvis::Response resp = tasks.handle(make_cmd(action, id));
            REQUIRE_FALSE(resp.ok);
            REQUIRE(resp.message.find("Id inválido") != std::string::npos);
        }

        jarvis::Response missing = tasks.handle(make_cmd(action, "99"));
        REQUIRE_FALSE(missing.ok);
        REQUIRE(missing.message == "Tarefa #99 não encontrada.");
    }

    // A tarefa continua lá, intacta
    REQUIRE(tasks.handle(make_cmd("listar")).message == "#1 [ ] Uma tarefa");
}

TEST_CASE("feito aceita '#3' e recusa concluir duas vezes", "[tasks]") {
    jarvis::Database db(":memory:");
    jarvis::tasks::TasksModule tasks;
    tasks.init(db);
    REQUIRE(tasks.handle(make_cmd("add", "Tarefa")).ok);

    REQUIRE(tasks.handle(make_cmd("feito", "#1")).ok);

    jarvis::Response again = tasks.handle(make_cmd("feito", "1"));
    REQUIRE_FALSE(again.ok);
    REQUIRE(again.message == "Tarefa #1 já estava concluída.");
}

TEST_CASE("Tarefas persistem ao recriar o módulo", "[tasks]") {
    jarvis::Database db(":memory:");

    {
        auto first = std::make_unique<jarvis::tasks::TasksModule>();
        first->init(db);
        REQUIRE(first->handle(make_cmd("add", "Sobrevivo ao módulo")).ok);
    }  // first é destruído aqui; o banco continua vivo

    jarvis::tasks::TasksModule second;
    second.init(db);

    REQUIRE(second.handle(make_cmd("listar")).message == "#1 [ ] Sobrevivo ao módulo");
}

TEST_CASE("Título com SQL malicioso é salvo como texto comum", "[tasks]") {
    jarvis::Database db(":memory:");
    jarvis::tasks::TasksModule tasks;
    tasks.init(db);

    const std::string evil = "x'); DROP TABLE tasks; --";

    jarvis::Response added = tasks.handle(make_cmd("add", evil));
    REQUIRE(added.ok);
    REQUIRE(added.data["title"] == evil);

    REQUIRE(table_exists(db, "tasks"));
    REQUIRE(tasks.handle(make_cmd("listar")).message == "#1 [ ] " + evil);
}

TEST_CASE("Ação desconhecida devolve erro", "[tasks]") {
    jarvis::Database db(":memory:");
    jarvis::tasks::TasksModule tasks;
    tasks.init(db);

    REQUIRE_FALSE(tasks.handle(make_cmd("voar")).ok);
}
