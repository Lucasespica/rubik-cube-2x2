#include <iostream>
#include <optional>
#include <chrono>
#include "Cubo.hpp"
#include "Busca.hpp"
#include "Interface.hpp"

int main() {
    habilitarCoresWindows();
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

    // Visualizacao do cubo
    imprimirLegenda();
    std::cout << "\nCubo resolvido:\n";
    imprimirCubo(original);
    std::cout << "\nCubo apos 1x U:\n";
    imprimirCubo(u);

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

    // estaResolvido teste
    std::cout << "resolvido? original=" << original.estaResolvido() << " (1)"
              << "  apos U=" << u.estaResolvido() << " (0)\n";

    // U e D' juntos giram o CUBO INTEIRO: continua resolvido, mas
    // nao e igual ao cubo original.
    Cubo girado = original.aplicarMovimento(Movimento::U)
                          .aplicarMovimento(Movimento::D_LINHA);
    std::cout << "U D' resolvido? " << girado.estaResolvido() << " (1)"
              << "  igual ao original? " << (girado == original) << " (0)\n";
    
    // gerarSucessores teste
    auto suc1 = original.gerarSucessores(std::nullopt);   // sem movimento anterior
    auto suc2 = original.gerarSucessores(Movimento::U);   // anterior foi U
    std::cout << "sucessores sem anterior: " << suc1.size() << " (12)"
              << "  apos U: " << suc2.size() << " (11)\n";
    
    bool achouInverso = false;
    for (const auto& par : suc2) {
        if (par.first == Movimento::U_LINHA) achouInverso = true;
    }
    std::cout << "U_LINHA aparece apos U? " << achouInverso << " (0)\n";

    // embaralhado (3 seeds) teste
    const unsigned seeds[] = {42, 123, 2026};
    for (unsigned seed : seeds) {
        Cubo e1 = Cubo::embaralhado(seed, 6);
        Cubo e2 = Cubo::embaralhado(seed, 6);
        std::cout << "seed " << seed
                  << ": repetivel=" << (e1 == e2) << " (1)"
                  << " resolvido=" << e1.estaResolvido() << " (0)\n";
    }
    std::cout << "seeds 42 e 123 diferentes? "
              << !(Cubo::embaralhado(42, 6) == Cubo::embaralhado(123, 6)) << " (1)\n";


    // teste heuristica de cantos
    std::cout << "heuristica(original) = " << heuristicaCantos(original)
              << "  (esperado 0)\n";
    Cubo embaralhado5 = Cubo::embaralhado(42, 5);
    std::cout << "heuristica(embaralhado) = " << heuristicaCantos(embaralhado5)
              << "  (esperado > 0)\n";

    // teste FilaPrioridade: confirma que sai sempre o menor f = g + h
    FilaPrioridade fp;
    fp.inserir(EstadoBusca{original, {Movimento::U, Movimento::D, Movimento::L}}); // g=3
    fp.inserir(EstadoBusca{embaralhado5, {}});                                     // g=0, h alto
    fp.inserir(EstadoBusca{original, {Movimento::U}});                             // g=1, h=0 -> f=1 (menor)

    EstadoBusca primeiro = fp.remover();
    std::cout << "FilaPrioridade tirou caminho.size()=" << primeiro.caminho.size()
              << "  (esperado 1)\n";

    // Teste de Busca: gera um cubo facil de resolver (poucos giros)
    Cubo embaralhadoFacil = Cubo::embaralhado(7, 4);

    auto imprimeResultado = [](const char* nome, const ResultadoBusca& r) {
        std::cout << "\n== " << nome << " ==\n";
        std::cout << "achou solucao? " << r.encontrou << "\n";
        std::cout << "estados visitados: " << r.visitados << "\n";
        std::cout << "movimentos (" << r.caminho.size() << "): ";
        for (Movimento m : r.caminho) {
            std::cout << nomeMovimento(m) << " ";
        }
        std::cout << "\n";
    };

    imprimeResultado("BFS", buscarBFS(embaralhadoFacil));
    imprimeResultado("A*",  buscarAEstrela(embaralhadoFacil));
    imprimeResultado("IDDFS", buscarIDDFS(embaralhadoFacil, 6));
            
    return 0;
}