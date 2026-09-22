#include "Busca.hpp"

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