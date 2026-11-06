#include <string>

#include <SQLiteCpp/Statement.h>
#include <catch2/catch_test_macros.hpp>

#include "jarvis/database.hpp"

namespace {

// Conta quantas linhas uma tabela tem
int count_rows(jarvis::Database& db, const std::string& table) {
    SQLite::Statement query(db.raw(), "SELECT COUNT(*) FROM " + table);
    query.executeStep();
    return query.getColumn(0).getInt();
}

// A tabela existe no banco?
bool table_exists(jarvis::Database& db, const std::string& table) {
    SQLite::Statement query(db.raw(),
                            "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = ?");
    query.bind(1, table);
    query.executeStep();
    return query.getColumn(0).getInt() == 1;
}

}  // namespace

TEST_CASE("Banco novo começa sem nenhuma migration", "[database]") {
    jarvis::Database db(":memory:");

    REQUIRE(table_exists(db, "schema_migrations"));
    REQUIRE(db.schema_version("qualquer") == 0);
}

TEST_CASE("migrate aplica o SQL e registra a versão", "[database]") {
    jarvis::Database db(":memory:");

    bool aplicou =
        db.migrate("teste", 1, "CREATE TABLE itens (id INTEGER PRIMARY KEY, nome TEXT NOT NULL)");

    REQUIRE(aplicou);
    REQUIRE(db.schema_version("teste") == 1);
    REQUIRE(table_exists(db, "itens"));

    db.execute("INSERT INTO itens (nome) VALUES ('primeiro')");
    REQUIRE(count_rows(db, "itens") == 1);
}

TEST_CASE("migrate não aplica a mesma versão duas vezes", "[database]") {
    jarvis::Database db(":memory:");
    const std::string sql = "CREATE TABLE itens (id INTEGER PRIMARY KEY)";

    REQUIRE(db.migrate("teste", 1, sql));
    REQUIRE_FALSE(db.migrate("teste", 1, sql));  // se rodasse de novo, daria erro
    REQUIRE(db.schema_version("teste") == 1);
}

TEST_CASE("migrations em sequência evoluem o esquema", "[database]") {
    jarvis::Database db(":memory:");

    REQUIRE(db.migrate("teste", 1, "CREATE TABLE itens (id INTEGER PRIMARY KEY)"));
    REQUIRE(db.migrate("teste", 2, "ALTER TABLE itens ADD COLUMN nome TEXT"));
    REQUIRE(db.schema_version("teste") == 2);

    db.execute("INSERT INTO itens (nome) VALUES ('com nome')");
    REQUIRE(count_rows(db, "itens") == 1);
}

TEST_CASE("Versões de módulos diferentes são independentes", "[database]") {
    jarvis::Database db(":memory:");

    db.migrate("tarefas", 1, "CREATE TABLE tarefas (id INTEGER PRIMARY KEY)");
    db.migrate("tarefas", 2, "ALTER TABLE tarefas ADD COLUMN titulo TEXT");
    db.migrate("financas", 1, "CREATE TABLE gastos (id INTEGER PRIMARY KEY)");

    REQUIRE(db.schema_version("tarefas") == 2);
    REQUIRE(db.schema_version("financas") == 1);
}

TEST_CASE("Migration com erro é desfeita por inteiro", "[database]") {
    jarvis::Database db(":memory:");

    // O segundo CREATE falha (tabela repetida), então o primeiro também é desfeito
    REQUIRE_THROWS(
        db.migrate("teste", 1, "CREATE TABLE a (id INTEGER); CREATE TABLE a (id INTEGER);"));

    REQUIRE(db.schema_version("teste") == 0);
    REQUIRE_FALSE(table_exists(db, "a"));
}
