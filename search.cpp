#include "search.h"
#include <unordered_set>

ResultadoBusca executarBusca(EstruturaDeBusca& estrutura, const CubeState& inicial,
                              int limiteProfundidade, long limiteEstados,
                              bool usarConjuntoVisitados) {
    // --- Adicionar estado inicial na estrutura ---
    No noInicial;
    noInicial.estado = inicial;
    noInicial.profundidade = 0;
    noInicial.ultimoMovimento = -1;
    estrutura.inserir(noInicial);

    // Guarda estados ja avaliados para nao reprocessar o mesmo estado
    // (evita loops e trabalho repetido; usado igualmente pelos 3 algoritmos).
    std::unordered_set<std::string> visitados;
    long contador = 0;

    // --- Enquanto a estrutura nao estiver vazia ---
    while (!estrutura.vazia()) {
        // Remover proximo estado da estrutura
        No atual = estrutura.remover();

        // Avaliar estado
        contador++;
        if (limiteEstados != -1 && contador > limiteEstados) {
            ResultadoBusca r;
            r.encontrou = false;
            r.estadosVisitados = contador;
            return r;
        }

        // SE estado e final: mostrar solucao e encerrar
        if (ehObjetivo(atual.estado)) {
            ResultadoBusca r;
            r.encontrou = true;
            r.solucao = atual.caminho;
            r.estadosVisitados = contador;
            return r;
        }

        if (usarConjuntoVisitados) {
            std::string chave = atual.estado.encode();
            if (visitados.count(chave)) continue; // ja avaliamos esse estado antes
            visitados.insert(chave);
        }

        // no IDDFS, nao expandimos alem do limite de profundidade da iteracao
        if (limiteProfundidade != -1 && atual.profundidade >= limiteProfundidade) {
            continue;
        }

        // Adicionar estados seguintes (sucessores) na estrutura
        std::vector<Sucessor> sucessores = gerarSucessores(atual.estado, atual.ultimoMovimento);
        for (size_t i = 0; i < sucessores.size(); i++) {
            No filho;
            filho.estado = sucessores[i].estado;
            filho.caminho = atual.caminho;
            filho.caminho.push_back(sucessores[i].movimento);
            filho.profundidade = atual.profundidade + 1;
            filho.ultimoMovimento = sucessores[i].movimento;
            estrutura.inserir(filho);
        }
    }

    // Retornar "Sem solucao"
    ResultadoBusca r;
    r.encontrou = false;
    r.estadosVisitados = contador;
    return r;
}

ResultadoBusca resolverComBFS(const CubeState& inicial, int profundidadeMaxima) {
    Fila fila;
    // BFS nao precisa de limite de profundidade para funcionar (ele
    // encontra a solucao mais curta naturalmente), mas usamos o
    // parametro como uma trava de seguranca contra buscas longas demais.
    return executarBusca(fila, inicial, profundidadeMaxima);
}

ResultadoBusca resolverComIDDFS(const CubeState& inicial, int profundidadeMaxima) {
    long totalEstadosVisitados = 0;
    for (int limite = 0; limite <= profundidadeMaxima; limite++) {
        Pilha pilha; // uma pilha NOVA a cada iteracao (busca recomeça do zero)
        ResultadoBusca r = executarBusca(pilha, inicial, limite, -1, /*usarConjuntoVisitados=*/false);
        totalEstadosVisitados += r.estadosVisitados;
        if (r.encontrou) {
            r.estadosVisitados = totalEstadosVisitados;
            return r;
        }
    }
    ResultadoBusca r;
    r.encontrou = false;
    r.estadosVisitados = totalEstadosVisitados;
    return r;
}

ResultadoBusca resolverComAStar(const CubeState& inicial, long limiteEstados) {
    FilaDePrioridade fila;
    return executarBusca(fila, inicial, -1, limiteEstados);
}
