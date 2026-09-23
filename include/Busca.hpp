#pragma once

#include <vector>
#include <memory>
#include <deque>
#include <queue>
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