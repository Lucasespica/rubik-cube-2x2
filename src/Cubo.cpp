#include "Cubo.hpp"

// Tudo dentro de "namespace" sem nome fica visível
// somente neste arquivo .cpp. Que serve para esconder
// detalhes internos do resto do programa.
namespace {
    // Um ciclo = 4 posicoes que trocam de lugar em roda
    // O conteúdo da posicao [0] vai para a [1], o da [1] vai
    // para a [2], o da [2] vai para a [3] e o da [3] volta para [0]
    using Ciclo = std::array<int, 4>;

    // Um movimento mexe em 12 stickers, que formam 3 ciclos de 4
    using DefinicaoMovimento = std::array<Ciclo, 3>;

    // Movimento U (horario, visto de cima)
    // As chaves duplas sao necessarias porque std::array guarda um
    // array "de verdade" por dentro
    const DefinicaoMovimento MOVIMENTO_U = {{
        {{ 0, 1, 3, 2}},        // Face U
        {{ 8, 4, 16, 12}},      // F -> L -> B -> R
        {{ 9, 5, 17, 13}}
    }};

    // Movimento D (horario, visto de baixo)
    const DefinicaoMovimento MOVIMENTO_D = {{
        {{20, 21, 23, 22}},     // Face D
        {{10, 14, 18, 6}},      // F -> R -> B -> L
        {{11, 15, 19, 7}}
    }};

   // Movimento L (horario, visto da esquerda)
    const DefinicaoMovimento MOVIMENTO_L = {{
        {{ 4,  5,  7,  6}},   // Face L
        {{ 0,  8, 20, 19}},   // U -> F -> D -> B
        {{ 2, 10, 22, 17}}
    }};

    // Movimento R (horario, visto da direita).
    const DefinicaoMovimento MOVIMENTO_R = {{
        {{12, 13, 15, 14}},   // Face R
        {{ 1, 18, 21,  9}},   // U -> B -> D -> F
        {{ 3, 16, 23, 11}}
    }};

    // Movimento F (horario, visto de frente).
    const DefinicaoMovimento MOVIMENTO_F = {{
        {{ 8,  9, 11, 10}},   // Face F
        {{ 2, 12, 21,  7}},   // U -> R -> D -> L
        {{ 3, 14, 20,  5}}
    }};

    // Movimento B (horario, visto de tras).
    const DefinicaoMovimento MOVIMENTO_B = {{
        {{16, 17, 19, 18}},   // Face B
        {{ 0,  6, 23, 13}},   // U -> L -> D -> R
        {{ 1,  4, 22, 15}}
    }};

    // Aplica uma definicao de movimento (as 3 rodas)
    // antigo: estado do cubo antes do giro
    // novo: copia onde vamos escrever o resultado
    // horario: true = cada conteudo vai para a proxima posicao da roda
    //          false = cada conteudo vai para a posicao anterior (movimento inverso) 
    void aplicarCiclos(const std::array<Cor, Cubo::TOTAL_STICKERS>& antigo,
                        std::array<Cor, Cubo::TOTAL_STICKERS>& novo,
                        const DefinicaoMovimento& def,
                        bool horario) {
        // Para cada movimento
        for (const Ciclo& c : def) {
            // e para cada uma das 4 posicoes da roda
            for(int i = 0; i < 4; i++) {
                int atual = c[i];
                // (i + 1) % 4 volta para 0 quando i chega em 3
                int proximo = c[(i + 1) % 4];

                if (horario) {
                    novo[proximo] = antigo[atual];
                } else {
                    novo[atual] = antigo[proximo];
                }
            }
        }
    }
}

// Cubo resolvido
Cubo::Cubo(){
    // Cor de cada face, na ordem dos indices: U, L, F, R, B, D
    const std::array<Cor, NUM_FACES> corDaFace = {{
        Cor::BRANCO, Cor::LARANJA, Cor::VERDE,
        Cor::VERMELHO, Cor::AZUL, Cor::AMARELO
    }};

    for (int face = 0; face < NUM_FACES; ++face) {
        for(int s = 0; s < STICKERS_POR_FACE; ++s) {
            // indice plano = face * 4 + sticker
            stickers_[face * STICKERS_POR_FACE + s] = corDaFace[face];
        }
    }
}

// Leitura somente de uma posicao especifica
Cor Cubo::corNaPosicao(int indiceFace, int indiceSticker) const {
    return stickers_[indiceFace *STICKERS_POR_FACE + indiceSticker];
}

// Dois cubos sao iguais se todos os 24 stickers forem iguais.
// std::array ja sabe se comparar com ==.
bool Cubo::operator==(const Cubo& outro) const {
    return stickers_ == outro.stickers_;
}

Cubo Cubo:: aplicarMovimento(Movimento m) const{
    // Copia do cubo atual. So a copia sera alterada, o original
    // (this) continua igual, como o header prometeu
    Cubo novo = *this;

    switch (m) {
        case Movimento::U:       aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_U, true);  break;
        case Movimento::U_LINHA: aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_U, false); break;
        case Movimento::D:       aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_D, true);  break;
        case Movimento::D_LINHA: aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_D, false); break;
        case Movimento::L:       aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_L, true);  break;
        case Movimento::L_LINHA: aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_L, false); break;
        case Movimento::R:       aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_R, true);  break;
        case Movimento::R_LINHA: aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_R, false); break;
        case Movimento::F:       aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_F, true);  break;
        case Movimento::F_LINHA: aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_F, false); break;
        case Movimento::B:       aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_B, true);  break;
        case Movimento::B_LINHA: aplicarCiclos(stickers_, novo.stickers_, MOVIMENTO_B, false); break;
    }
    return novo;
}