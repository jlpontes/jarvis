# Avisos do compilador para os targets do Jarvis.
# Uso: jarvis_set_warnings(<target>)

option(JARVIS_WARNINGS_AS_ERRORS "Tratar avisos do compilador como erros" OFF)

function(jarvis_set_warnings target)
    set(common_warnings
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow                 # variável escondendo outra de escopo externo
        -Wconversion             # conversões que podem perder dados (int64 -> int)
        -Wsign-conversion        # mistura de com sinal e sem sinal
        -Wnon-virtual-dtor       # classe com virtual sem destrutor virtual
        -Wold-style-cast         # (int)x em vez de static_cast<int>(x)
        -Woverloaded-virtual     # função que esconde uma virtual da base
        -Wcast-align
        -Wnull-dereference
        -Wdouble-promotion       # float promovido para double sem querer
        -Wformat=2
        -Wimplicit-fallthrough   # case sem break nem [[fallthrough]]
    )

    set(gcc_warnings
        -Wduplicated-cond        # if/else if com a mesma condição
        -Wlogical-op             # && / || suspeitos
        -Wmisleading-indentation
    )

    target_compile_options(${target} PRIVATE
        ${common_warnings}
        $<$<CXX_COMPILER_ID:GNU>:${gcc_warnings}>
        $<$<BOOL:${JARVIS_WARNINGS_AS_ERRORS}>:-Werror>
    )
endfunction()
