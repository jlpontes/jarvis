#include "jarvis/database.hpp"

#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>

namespace jarvis {

Database::Database(const std::string& path)
    : db_(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    db_.exec("PRAGMA foreign_keys = ON");

    db_.exec(R"sql(
        CREATE TABLE IF NOT EXISTS schema_migrations (
            module     TEXT    NOT NULL,
            version    INTEGER NOT NULL,
            applied_at TEXT    NOT NULL DEFAULT (datetime('now')),
            PRIMARY KEY (module, version)
        )
    )sql");
}

void Database::execute(const std::string& sql) {
    db_.exec(sql);
}

int Database::schema_version(const std::string& module) const {
    SQLite::Statement query(
        db_, "SELECT COALESCE(MAX(version), 0) FROM schema_migrations WHERE module = ?");
    query.bind(1, module);
    query.executeStep();
    return query.getColumn(0).getInt();
}

bool Database::migrate(const std::string& module, int version, const std::string& sql) {
    if (version <= schema_version(module)) {
        return false;  // já aplicada
    }

    SQLite::Transaction transaction(db_);

    db_.exec(sql);

    SQLite::Statement insert(db_, "INSERT INTO schema_migrations (module, version) VALUES (?, ?)");
    insert.bind(1, module);
    insert.bind(2, version);
    insert.exec();

    transaction.commit();
    return true;
}

SQLite::Database& Database::raw() {
    return db_;
}

}  // namespace jarvis
