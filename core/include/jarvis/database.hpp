#pragma once

#include <string>

#include <SQLiteCpp/Database.h>

namespace jarvis {

// Embrulho do SQLite com controle de versão do esquema (migrations)
class Database {
public:
    // Abre (ou cria) o arquivo do banco. Use ":memory:" para um banco temporário.
    explicit Database(const std::string& path);

    // Executa SQL que não devolve linhas (CREATE, INSERT, UPDATE, DELETE...)
    void execute(const std::string& sql);

    // Aplica a migration se ela ainda não foi aplicada.
    // Devolve true se aplicou agora, false se já estava aplicada.
    bool migrate(const std::string& module, int version, const std::string& sql);

    // Última versão de migration aplicada para o módulo (0 = nenhuma)
    int schema_version(const std::string& module) const;

    // Acesso direto ao SQLiteCpp, para consultas com parâmetros
    SQLite::Database& raw();

private:
    SQLite::Database db_;
};

}  // namespace jarvis