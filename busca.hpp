#ifndef BUSCA_HPP
#define BUSCA_HPP

#include "cubo.hpp"
#include <queue>
#include <stack>
#include <set>
#include <vector>

struct NoBusca {
    Cubo cubo;
    std::vector<std::string> caminho;
    int g; // Custo real do caminho
    int h; // Estimativa da heurística
    int f; // f(n) = g(n) + h(n)
    NoBusca() : g(0), h(0), f(0) {}
};

// Interface Genérica obrigatória para atender ao Laço Único
class EstruturaBusca {
public:
    virtual void inserir(const NoBusca& no) = 0;
    virtual NoBusca remover() = 0;
    virtual bool vazia() = 0;
    virtual void limpar() = 0;
    virtual ~EstruturaBusca() {}
};

// Implementação para a Busca em Largura (BFS).
class FilaBFS : public EstruturaBusca {
private:
    std::queue<NoBusca> fila;
public:
    void inserir(const NoBusca& no) override { fila.push(no); }
    NoBusca remover() override { NoBusca n = fila.front(); fila.pop(); return n; }
    bool vazia() override { return fila.empty(); }
    void limpar() override { while(!fila.empty()) fila.pop(); }
};

// Implementação para a Busca em Profundidade (Usada no IDDFS).
class PilhaDFS : public EstruturaBusca {
private:
    std::stack<NoBusca> pilha;
public:
    void inserir(const NoBusca& no) override { pilha.push(no); }
    NoBusca remover() override { NoBusca n = pilha.top(); pilha.pop(); return n; }
    bool vazia() override { return pilha.empty(); }
    void limpar() override { while(!pilha.empty()) pilha.pop(); }
};

// Comparador para ordenar a Fila de Prioridade do A* (Menor F(n) no topo).
struct ComparadorAEstrela {
    bool operator()(const NoBusca& a, const NoBusca& b) {
        return a.f > b.f; 
    }
};

// Implementação para a Busca A*.
class FilaPrioridadeAEstrela : public EstruturaBusca {
private:
    std::priority_queue<NoBusca, std::vector<NoBusca>, ComparadorAEstrela> pq;
public:
    void inserir(const NoBusca& no) override { pq.push(no); }
    NoBusca remover() override { NoBusca n = pq.top(); pq.pop(); return n; }
    bool vazia() override { return pq.empty(); }
    void limpar() override { while(!pq.empty()) pq.pop(); }
};

// UM ÚNICO Laço genérico de busca.
bool busca_generica(Cubo inicial, EstruturaBusca* estrutura, int limite_profundidade = -1) {
    std::set<std::string> visitados;
    NoBusca raiz;
    raiz.cubo = inicial;
    raiz.h = inicial.calcular_heuristica();
    raiz.f = raiz.g + raiz.h;
    
    estrutura->limpar();
    estrutura->inserir(raiz);
    int estados_visitados = 0;

    // Enquanto a estrutura não estiver vazia.
    while (!estrutura->vazia()) {
        NoBusca atual = estrutura->remover(); // Remover próximo estado.
        estados_visitados++;

        // SE estado é final: Mostrar solução e encerrar.
        if (atual.cubo.is_resolvido()) {
            std::cout << "\n[!] SOLUCAO ENCONTRADA!\n";
            std::cout << "Estados avaliados: " << estados_visitados << "\n";
            std::cout << "Movimentos (" << atual.caminho.size() << "): ";
            for (const std::string& p : atual.caminho) std::cout << p << " ";
            std::cout << "\n\n";
            return true;
        }


        // Só marcamos como visitado (fechamos o nó) quando ele
        // realmente vai ser expandido.
        if (limite_profundidade != -1 && atual.g >= limite_profundidade) {
            continue; // Poda pela profundidade (Para o IDDFS).
        }

        visitados.insert(atual.cubo.get_estado());

        // Adicionar sucessores na estrutura.
        for (const Movimento& mov : atual.cubo.gerar_sucessores()) {
            if (visitados.find(mov.estado_resultante) == visitados.end()) {
                NoBusca filho;
                filho.cubo = Cubo(mov.estado_resultante);
                filho.caminho = atual.caminho;
                filho.caminho.push_back(mov.nome);
                filho.g = atual.g + 1;
                filho.h = filho.cubo.calcular_heuristica();
                filho.f = filho.g + filho.h;
                estrutura->inserir(filho);
            }
        }
    }
    return false; // Retornar "Sem solução".
}

// O IDDFS precisa de um laço externo que incremente o limite iterativamente.
void busca_iddfs_iterativa(Cubo cubo) {
    PilhaDFS pilha;
    int limite = 0;
    int limite_maximo = 20; // Prevenção de loop eterno
    
    while (limite <= limite_maximo) {
        std::cout << "Testando IDDFS com profundidade: " << limite << "...\n";
        if (busca_generica(cubo, &pilha, limite)) {
            return;
        }
        limite++;
    }
    std::cout << "Sem solucao no limite maximo.\n";
}

#endif