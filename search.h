#ifndef SEARCH_H
#define SEARCH_H

#include "cube_state.h"
#include <vector>
#include <deque>
#include <queue>
#include <string>

// Um "No" de busca guarda o estado atual, o caminho de movimentos
// percorrido desde o inicio (para depois mostrar a solucao) e a
// profundidade (numero de movimentos ja aplicados = custo g).
struct No {
    CubeState estado;
    std::vector<int> caminho; // sequencia de movimentos ate aqui
    int profundidade;         // = caminho.size(), guardado à parte por clareza
    int ultimoMovimento;      // -1 se for o no inicial
};

// =====================================================================
// ESTRUTURA DE DADOS DE BUSCA (interface comum)
// =====================================================================
// Cada algoritmo (BFS, IDDFS, A*) so difere na ORDEM em que os nos sao
// removidos. Toda a logica do laço de busca (ver executarBusca, em
// search.cpp) e IDENTICA para os tres; o que muda é qual classe abaixo
// é usada por baixo dos panos.
// =====================================================================
class EstruturaDeBusca {
public:
    virtual ~EstruturaDeBusca() {}
    virtual void inserir(const No& no) = 0;
    virtual No remover() = 0;
    virtual bool vazia() const = 0;
};

// FILA (FIFO) -> usada pela Busca em Largura (BFS)
class Fila : public EstruturaDeBusca {
public:
    void inserir(const No& no) override { dados.push_back(no); }
    No remover() override { No n = dados.front(); dados.pop_front(); return n; }
    bool vazia() const override { return dados.empty(); }
private:
    std::deque<No> dados;
};

// PILHA (LIFO) -> usada pela Busca em Profundidade Limitada Iterativa (IDDFS)
class Pilha : public EstruturaDeBusca {
public:
    void inserir(const No& no) override { dados.push_back(no); }
    No remover() override { No n = dados.back(); dados.pop_back(); return n; }
    bool vazia() const override { return dados.empty(); }
private:
    std::vector<No> dados;
};

// FILA DE PRIORIDADE (menor f = g+h primeiro) -> usada pelo A*
// g = profundidade (numero de movimentos ate aqui), h = heuristica().
class FilaDePrioridade : public EstruturaDeBusca {
public:
    void inserir(const No& no) override { dados.push(no); }
    No remover() override { No n = dados.top(); dados.pop(); return n; }
    bool vazia() const override { return dados.empty(); }
private:
    struct Comparador {
        // std::priority_queue e um MAX-heap por padrao; para tirar
        // sempre o MENOR f primeiro, invertemos a comparacao (">" em
        // vez de "<").
        bool operator()(const No& a, const No& b) const {
            int fa = a.profundidade + heuristica(a.estado);
            int fb = b.profundidade + heuristica(b.estado);
            return fa > fb;
        }
    };
    std::priority_queue<No, std::vector<No>, Comparador> dados;
};

// Resultado devolvido por qualquer uma das buscas.
struct ResultadoBusca {
    bool encontrou;
    std::vector<int> solucao;   // sequencia de movimentos (vazio se nao encontrou)
    long estadosVisitados;      // quantos nos foram removidos/avaliados
};

// =====================================================================
// O LAÇO GENÉRICO DE BUSCA
// =====================================================================
// Segue exatamente a receita pedida na especificacao:
//   Adicionar estado inicial na estrutura
//   Enquanto a estrutura nao estiver vazia:
//       Remover proximo estado da estrutura
//       Avaliar estado
//       SE estado e final: mostrar solucao e encerrar
//       Adicionar estados seguintes (sucessores) na estrutura
//   Retornar "Sem solucao"
//
// O parametro "limiteProfundidade" so e usado pelo IDDFS (para nao
// expandir nos alem do limite da iteracao atual); nas demais buscas
// deve ser passado como -1 (sem limite).
// =====================================================================
// "limiteEstados" e uma trava de seguranca (-1 = sem limite): se a
// busca visitar mais estados que isso sem achar solucao, ela desiste
// e devolve encontrou=false, em vez de rodar indefinidamente.
//
// "usarConjuntoVisitados" controla se descartamos estados ja avaliados
// antes (necessario para BFS e A* funcionarem bem, e SEGURO para eles
// porque a primeira vez que um estado sai da fila/fila-de-prioridade
// ja e pelo caminho mais curto/barato ate ele). Para o IDDFS (pilha,
// ordem LIFO) isso NAO pode ser usado: um estado poderia ser marcado
// como visitado ao ser alcançado por um ramo mais longo da pilha antes
// de ser alcançado por um ramo mais curto que levaria a solucao dentro
// do limite de profundidade -- por isso o IDDFS usa apenas o limite de
// profundidade para cortar a busca, sem conjunto de visitados global.
ResultadoBusca executarBusca(EstruturaDeBusca& estrutura, const CubeState& inicial,
                              int limiteProfundidade, long limiteEstados = -1,
                              bool usarConjuntoVisitados = true);

// Funcoes de conveniencia que escolhem a estrutura de dados certa
// para cada algoritmo e chamam o mesmo executarBusca() por baixo.
ResultadoBusca resolverComBFS(const CubeState& inicial, int profundidadeMaxima);
ResultadoBusca resolverComIDDFS(const CubeState& inicial, int profundidadeMaxima);
ResultadoBusca resolverComAStar(const CubeState& inicial, long limiteEstados);

#endif
