#pragma once

#include <vector>
#include <memory>
#include <deque>
#include <queue>
#include <string>
#include <optional>
#include "Cubo.hpp"

// Um "no" da arvore de busca: um cubo, mais o caminho de
// movimentos percorrido desde o cubo inicial ate chegar nele
struct EstadoBusca {
    Cubo cubo;
    std::vector<Movimento> caminho;
};

// Interface comum as 3 estruturas
class IEstruturaDeDados {
public:
    // destrutor virtual: obrigatorio quando uma classe vai ser usada
    // por ponteiro base (IEstruturaDeDados*) apontando pra uma
    // classe filha (Fila, Pilha, ...). Sem isso, o "delete" poderia
    // nao chamar o destrutor certo.
    virtual ~IEstruturaDeDados() = default;

    virtual void inserir(const EstadoBusca& estado) = 0;
    virtual EstadoBusca remover() = 0;
    virtual bool vazia() const = 0;
};

// BFS: primeiro que entra, primeiro que sai
class Fila : public IEstruturaDeDados {
public:
    void inserir(const EstadoBusca& estado) override;
    EstadoBusca remover() override;
    bool vazia() const override;
private:
    std::deque<EstadoBusca> dados_;
};

// DFS/IDDFS, ultimo que entra, primeiro que sai
class Pilha : public IEstruturaDeDados {
public:
    void inserir(const EstadoBusca& estado) override;
    EstadoBusca remover() override;
    bool vazia() const override;
private:
    std::vector<EstadoBusca> dados_;
};

// Quantos cantos fora do lugar (funcao usada por A*)
// Fica declarada aqui pro comparador do priority_queue poder usa-la
int heuristicaCantos(const Cubo& cubo);

// Decide qual EstadoBusca sai primeiro da fila de prioridade
struct ComparaEstados {
    bool operator()(const EstadoBusca& a, const EstadoBusca& b) const;
};

// A* sempre remove o estado de menor f = g + h
// onde g = movimentos ja dados e h = heuristicaCantos
class FilaPrioridade : public IEstruturaDeDados {
public:
    void inserir(const EstadoBusca& estado) override;
    EstadoBusca remover() override;
    bool vazia() const override;
private:
    // std::priority_queue tira por padrao o maior primeiro
    // E como queremos o menor f primeiro, o operator() (acima) devolve o inverso
    // da comparacao usual (fa > fb, nao fa < fb) 
    std::priority_queue<EstadoBusca, std::vector<EstadoBusca>, ComparaEstados> dados_;
};

// Nome legivel de um movimento, tipo "U" ou "U'" 
std::string nomeMovimento(Movimento m);

// O que a busca devolve: se achou, o caminho ate a solucao, e quantos
// estados foram removidos da estrutura (requisito de saida)
struct ResultadoBusca {
    bool encontrou = false;
    std::vector<Movimento> caminho;
    long long visitados = 0;
};

// O laco unico, reaproveitando por BFS, IDDFS e A*
// So enxerga IEstruturaDeDados, nao se sabe se 'estrutura' e uma 
// Fila, uma Pilha ou uma FilaPrioridade
// limiteProfundidade = -1 significa "sem limite" (BFS, A*)
// O IDDFS chama esta mesma funcao varias vezes, com limite 0, 1, 2...
ResultadoBusca buscar(const Cubo& inicial,
                      IEstruturaDeDados& estrutura,
                      int limiteProfundidade);

// As 3 buscas do trabalho, cada uma so escolhendo a estrutura certa
ResultadoBusca buscarBFS(const Cubo& inicial);
ResultadoBusca buscarAEstrela(const Cubo& inicial);
ResultadoBusca buscarIDDFS(const Cubo& inicial, int limiteMaximo);
