#pragma once

#include <vector>
#include <memory>
#include <deque>
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