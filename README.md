# Jarvis

[![CI](https://github.com/jlpontes/jarvis/actions/workflows/ci.yml/badge.svg)](https://github.com/jlpontes/jarvis/actions/workflows/ci.yml)

Assistente pessoal modular escrito em **C++20**. É um projeto de estudo (C++ moderno e arquitetura
de software) que também serve para o dia a dia: tarefas hoje e, no futuro, finanças, saúde,
lembretes e um assistente com IA.

A ideia central é um **núcleo único** (`core/`) que não conhece nenhuma funcionalidade
específica, **módulos plugáveis** (`modules/`) que implementam as funcionalidades, e **clientes
finos** (`apps/`) que só traduzem a entrada do usuário em comandos. Hoje o cliente é a linha de
comando. Servidor HTTP, bot do Telegram e app Qt vêm depois, todos usando os mesmos módulos.

## Pré-requisitos

- Compilador com C++20 (Apple clang 15+, GCC 13+ ou Clang 16+)
- [CMake](https://cmake.org/) 3.25 ou mais novo
- [Ninja](https://ninja-build.org/)
- [vcpkg](https://github.com/microsoft/vcpkg), com a variável de ambiente `VCPKG_ROOT` apontando
  para ele

As bibliotecas ([nlohmann/json](https://github.com/nlohmann/json),
[SQLiteCpp](https://github.com/SRombauts/SQLiteCpp) e [Catch2](https://github.com/catchorg/Catch2))
são baixadas e compiladas automaticamente pelo vcpkg a partir do `vcpkg.json`.

No macOS:

```bash
brew install cmake ninja
git clone https://github.com/microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh -disableMetrics
echo 'export VCPKG_ROOT="$HOME/vcpkg"' >> ~/.zshrc
```

## Como compilar

```bash
cmake --preset debug           # configura (na 1ª vez o vcpkg compila as dependências)
cmake --build --preset debug   # compila
ctest --preset debug           # roda os testes
```

Para compilar otimizado: `cmake --preset release && cmake --build --preset release`.

Opções do CMake:

| Opção                        | Padrão | Efeito                                   |
|------------------------------|--------|------------------------------------------|
| `JARVIS_BUILD_TESTS`         | `ON`   | Compila os testes (`jarvis_tests`)       |
| `JARVIS_WARNINGS_AS_ERRORS`  | `OFF`  | Avisos do compilador viram erros (a CI liga) |

## Como usar

```bash
./build/debug/apps/cli/jarvis help                          # lista módulos e ações

./build/debug/apps/cli/jarvis tarefas add Comprar pão integral
./build/debug/apps/cli/jarvis tarefas listar
./build/debug/apps/cli/jarvis tarefas feito 1               # também aceita "#1"
./build/debug/apps/cli/jarvis tarefas listar todas
./build/debug/apps/cli/jarvis tarefas remover 1

./build/debug/apps/cli/jarvis relogio agora
./build/debug/apps/cli/jarvis ping echo olá
```

Os dados ficam em `~/.jarvis/jarvis.db` (SQLite).

## Estrutura

```
core/                  libjarvis (jarvis::core): Command/Response, IModule, ModuleRegistry, Database
modules/
  tasks/               tarefas (SQLite, padrão Repository)
  clock/               relógio
  ping/                módulo de teste
    include/jarvis/<nome>/   headers públicos
    src/                     implementação e headers privados
apps/cli/              executável `jarvis`
tests/                 testes com Catch2 (core/ e modules/)
cmake/                 funções CMake do projeto (avisos do compilador)
```

**Regra de dependência:** `apps/` → `modules/` → `core/`, nunca ao contrário. O core não conhece
módulos nem clientes.

Um módulo implementa a interface `jarvis::IModule`: `name()`, `actions()`, `init(Database&)`
(se usar banco) e `handle(const Command&)`, que devolve uma `Response` com uma mensagem para
humanos e dados em JSON para máquinas.

## Qualidade

- **Avisos:** todos os targets compilam com `-Wall -Wextra -Wpedantic -Wshadow -Wconversion` e
  outros (veja `cmake/CompilerWarnings.cmake`).
- **Formatação:** `.clang-format` (base LLVM, 4 espaços, 100 colunas). Para formatar:
  ```bash
  git ls-files '*.hpp' '*.cpp' | xargs clang-format -i
  ```
- **CI:** cada push e pull request para `main` compila e testa no Ubuntu (GCC) e no macOS
  (Apple clang), com os avisos tratados como erros, e confere a formatação.

## Roteiro

**Fase 1: fechar a base**
- [x] Núcleo: comandos, registry de módulos, banco SQLite com migrations
- [x] Módulo de tarefas
- [x] Ping e relógio como módulos
- [x] Avisos do compilador, formatação e CI
- [ ] Configuração e log
- [ ] Tarefas v2: prazo e prioridade

**Fase 2: sair do terminal**
- [ ] Servidor HTTP
- [ ] Bot do Telegram
- [ ] Deploy 24h

**Fase 3: novos módulos**
- [ ] Finanças
- [ ] Agendador e resumos automáticos
- [ ] Saúde: importar dados do Garmin
- [ ] Relatório de saúde para médico + Google Drive

**Fase 4: inteligência**
- [ ] IA com tool calling (as `actions()` dos módulos viram ferramentas)
- [ ] Insights de saúde com IA

**Fase 5: plataformas e arquitetura avançada**
- [ ] Plugins dinâmicos
- [ ] App Qt (PC e celular)
