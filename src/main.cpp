#include <iostream>
#include "Cubo.hpp"

int main() {
    Cubo original;  // chama o construtor (cubo resolvido)

    // Teste 1: girar U quatro vezes deve voltar ao inicio
    Cubo c = original;
    for (int i = 0; i < 4; ++i) {
        c = c.aplicarMovimento(Movimento::U);
    }
    std::cout << "U x4 == original? " << (c == original) << " (esperado 1)\n";

    // Teste 2: 
    // Teste 2: U seguido de U' deve voltar ao inicio
    Cubo d = original.aplicarMovimento(Movimento::U)
                     .aplicarMovimento(Movimento::U_LINHA);
    std::cout << "U U' == original?  " << (d == original) << "  (esperado 1)\n";

    // Teste 3: um U so NAO pode ser igual ao original
    Cubo u = original.aplicarMovimento(Movimento::U);
    std::cout << "U x1 == original?  " << (u == original) << "  (esperado 0)\n";

    // Teste 4: confere a direcao do giro. Depois de U, a fileira de
    // cima da face L recebe a cor da F (verde = 2), e a da F recebe
    // a cor da R (vermelho = 5).
    // static_cast<int> transforma o enum em numero pra podermos imprimir.
    std::cout << "L[0] apos U: " << static_cast<int>(u.corNaPosicao(1, 0))
              << "  (esperado 2, VERDE)\n";
    std::cout << "F[0] apos U: " << static_cast<int>(u.corNaPosicao(2, 0))
              << "  (esperado 5, VERMELHO)\n";

    // Teste 5: movimentos base
    const Movimento normais[] = {Movimento::U, Movimento::D, Movimento::L,
                                   Movimento::R, Movimento::F, Movimento::B};
    const Movimento inversos[] = {Movimento::U_LINHA, Movimento::D_LINHA, Movimento::L_LINHA,
                                   Movimento::R_LINHA, Movimento::F_LINHA, Movimento::B_LINHA};
    const char nomes[] = {'U', 'D', 'L', 'R', 'F', 'B'};

    for (int i = 0; i < 6; ++i) {
        // 4 giros iguais devem voltar ao original
        Cubo x = original;
        for (int k = 0; k < 4; ++k) {
            x = x.aplicarMovimento(normais[i]);
        }
        // movimento seguido do inverso deve voltar ao original
        Cubo y = original.aplicarMovimento(normais[i])
                         .aplicarMovimento(inversos[i]);
        // um giro so deve mudar o cubo
        Cubo z = original.aplicarMovimento(normais[i]);

        std::cout << nomes[i] << ": x4=" << (x == original)
                  << " inverso=" << (y == original)
                  << " muda=" << !(z == original)
                  << "  (esperado 1 1 1)\n";
    }
    // Teste 7: sequencia R U R' U'
    // Repetida 6 vezes, volta ao original (e nenhuma vez antes disso).
    Cubo s = original;
    for (int k = 1; k <= 6; ++k) {
        s = s.aplicarMovimento(Movimento::R)
             .aplicarMovimento(Movimento::U)
             .aplicarMovimento(Movimento::R_LINHA)
             .aplicarMovimento(Movimento::U_LINHA);
        std::cout << "R U R' U' x" << k << " == original? " << (s == original)
                  << "  (esperado " << (k == 6) << ")\n";
    }

    return 0;
}