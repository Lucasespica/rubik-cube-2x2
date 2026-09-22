#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include <optional>

// Movimento.hpp faz mais sentido como arquivo separado,
// depois, irei mover pra include/Movimento.hpp
// Por enquanto vou deixar

enum class Movimento {
    U, U_LINHA,
    D, D_LINHA,
    L, L_LINHA,
    R, R_LINHA,
    F, F_LINHA,
    B, B_LINHA
};

// Cor de cada "sticker". Facilita trocar de char pra enum de
// cor de verdade depois, sem afetar quem usa o Cubo.
enum class Cor : uint8_t {
    BRANCO, AMARELO, VERDE, AZUL, LARANJA, VERMELHO
};

// CONVENCAO DE INDICES
//
// Planificacao em cruz (cada face vista de fora do cubo, na
// orientacao em que aparece na planificacao):
//
//             +---+
//             | U |   0
//         +---+---+---+---+
//         | L | F | R | B |   1 2 3 4
//         +---+---+---+---+
//             | D |   5
//             +---+
//
// Faces:  0=U (cima)   1=L (esquerda)  2=F (frente)
//         3=R (direita) 4=B (tras)     5=D (baixo)
//
// Stickers dentro de cada face:
//         +---+---+
//         | 0 | 1 |
//         +---+---+
//         | 2 | 3 |
//         +---+---+
//
// Indice no array stickers_:  face * STICKERS_POR_FACE + sticker
// Ex: topo-direita da face F  ->  2 * 4 + 1 = 9

class Cubo {
public:
    static constexpr int NUM_FACES = 6;
    static constexpr int STICKERS_POR_FACE = 4;
    static constexpr int TOTAL_STICKERS = NUM_FACES * STICKERS_POR_FACE;

    // construtor: cubo resolvido (estado inicial "de fábrica")
    Cubo();

    // Aplica um movimento e retorna o NOVO estado resultante
    // (não altera o estado atual, facilita manter os nós de
    // busca imutáveis, o que ajuda a não ter bug de referência
    // compartilhada entre nós da fila/pilha).
    Cubo aplicarMovimento(Movimento m) const;

    // Gera todos os estados sucessores possíveis a partir daqui.
    // 'ultimoMovimento' é opcional — usado pra podar o movimento
    // inverso (evita desfazer o passo anterior).
    std::vector<std::pair<Movimento, Cubo>> gerarSucessores(
        std::optional<Movimento> ultimoMovimento) const;

    // Função avaliadora
    bool estaResolvido() const;

    // Embaralha a partir de uma seed, aplicando N movimentos
    // aleatórios válidos. Guarda a seed usada, se quiser exibir
    // pro usuário depois.
    static Cubo embaralhado(unsigned seed, int numMovimentos);

    // Consulta somente leitura de uma posição específica
    Cor corNaPosicao(int indiceFace, int indiceSticker) const;

    // Útil pra heurística do A* e pra depuração/testes.
    bool operator==(const Cubo& outro) const;

private:
    std::array<Cor, TOTAL_STICKERS> stickers_;

    // Provavelmente vai ter funções privadas auxiliares aqui,
    // tipo rotacionarFace(int indiceFace) e ajustarFacesVizinhas(...),
    // que fazem o trabalho pesado de cada movimento individual
};