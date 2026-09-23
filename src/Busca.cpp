#include "Busca.hpp"

namespace {
    // Cada canto = 3 indices de adesivo
    // que pertencem ao mesmo cubo fisico
    using Canto = std::array<int, 3>;

    const std::array<Canto, 8> CANTOS = {{
        {{0, 4, 17}},
        {{1, 13, 16}},
        {{2, 5, 8}},
        {{3, 9, 12}},
        {{6, 19, 22}},
        {{7, 10, 20}},
        {{11, 14, 21}},
        {{15, 18, 23}}
    }};
}

// Fila (usada pelo bfs)
void Fila::inserir(const EstadoBusca& estado) {
    dados_.push_back(estado);
}

EstadoBusca Fila::remover(){
    //Tira do inicio, que e o mais antigo
    EstadoBusca proximo = dados_.front();
    dados_.pop_front();
    return proximo;
}

bool Fila::vazia() const {
    return dados_.empty();
}

// Pilha (usado pelo DFS/IDDFS)
void Pilha:: inserir(const EstadoBusca& estado) {
    dados_.push_back(estado);
}

EstadoBusca Pilha::remover() {
    // Tira do fim, que e o mais recente
    EstadoBusca proximo = dados_.back();
    dados_.pop_back();
    return proximo;
}

bool Pilha::vazia() const {
    return dados_.empty();
}

int heuristicaCantos(const Cubo& cubo) {
    // static: so cria o cubo resolvido uma vez, na primeira chamada,
    // e reaproveita nas seguintes
    static const Cubo RESOLVIDO;

    int fora = 0;
    for (const Canto& canto : CANTOS) {
        bool certo = true;
        for (int idx: canto) {
            int face = idx / Cubo::STICKERS_POR_FACE;
            int sticker = idx % Cubo::STICKERS_POR_FACE;
            if (cubo.corNaPosicao(face, sticker) != RESOLVIDO.corNaPosicao(face, sticker)) {
                certo = false;
                break;
            }
        }
        if (!certo) {
            ++fora;
        }
    }
    // Arredonda pra cima com divisao inteira
    // Ex: fora = 5 -> (5 + 3)/4 = 2
    return (fora + 3) / 4;
}

bool ComparaEstados::operator()(const EstadoBusca& a, const EstadoBusca& b) const {
    int fa = static_cast<int>(a.caminho.size()) + heuristicaCantos(a.cubo);
    int fb = static_cast<int>(b.caminho.size()) + heuristicaCantos(b.cubo);
    return fa > fb;
}

void FilaPrioridade::inserir(const EstadoBusca& estado) {
    dados_.push(estado);
}

EstadoBusca FilaPrioridade::remover() {
    EstadoBusca proximo = dados_.top();
    dados_.pop();
    return proximo;
}

bool FilaPrioridade::vazia() const {
    return dados_.empty();
}