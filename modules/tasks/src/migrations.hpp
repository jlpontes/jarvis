#pragma once

namespace jarvis::tasks::migrations {

// V1: tabela de tarefas. NUNCA editar depois de publicada: criar kV2.
inline constexpr const char* kV1 = R"sql(
    CREATE TABLE tasks (
        id         INTEGER PRIMARY KEY AUTOINCREMENT,
        title      TEXT    NOT NULL CHECK (length(trim(title)) > 0),
        done       INTEGER NOT NULL DEFAULT 0 CHECK (done IN (0, 1)),
        created_at TEXT    NOT NULL DEFAULT (datetime('now', 'localtime')),
        done_at    TEXT
    );
)sql";

}  // namespace jarvis::tasks::migrations
