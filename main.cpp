#include "cube_state.h"
#include "search.h"
#include <iostream>
#include <chrono>
#include <limits>

static void mostrarMenuMovimentos() {
    std::cout << "Movimentos disponiveis: U U' D D' F F' B B' L L' R R'\n";
    std::cout << "Digite um movimento (ou 0 para voltar ao menu): ";
}

static int lerMovimentoDigitado(const std::string& texto) {
    for (int m = 0; m < NUM_MOVIMENTOS; m++) {
        if (nomeMovimento(m) == texto) return m;
    }
    return -1;
}

// ------------------------- MODO MANUAL -------------------------
static void jogarManualmente(CubeState estado) {
    std::cout << "\n=== MODO MANUAL ===\n";
    imprimirCubo(estado);
    while (true) {
        mostrarMenuMovimentos();
        std::string entrada;
        std::cin >> entrada;
        if (entrada == "0") break;

        int mov = lerMovimentoDigitado(entrada);
        if (mov == -1) {
            std::cout << "Movimento invalido.\n";
            continue;
        }
        estado = aplicarMovimento(estado, mov);
        imprimirCubo(estado);
        if (ehObjetivo(estado)) {
            std::cout << ">>> Parabens, cubo resolvido! <<<\n";
        }
    }
}

// ------------------------- MODO SOLUCIONADOR -------------------------
static void mostrarSolucao(const ResultadoBusca& resultado, double segundos) {
    std::cout << "\n--- RESULTADO ---\n";
    std::cout << "Estados visitados: " << resultado.estadosVisitados << "\n";
    std::cout << "Tempo: " << segundos << " s\n";

    if (!resultado.encontrou) {
        std::cout << "SEM SOLUCAO dentro do limite estabelecido.\n";
        return;
    }

    std::cout << "Solucao encontrada em " << resultado.solucao.size() << " movimento(s):\n";
    for (size_t i = 0; i < resultado.solucao.size(); i++) {
        std::cout << "  Passo " << (i + 1) << ": " << nomeMovimento(resultado.solucao[i]) << "\n";
    }
}

static void resolverComIA(const CubeState& estadoEmbaralhado) {
    std::cout << "\n=== RESOLVER COM IA ===\n";
    std::cout << "Escolha o algoritmo:\n";
    std::cout << "  1) Busca em Largura (BFS)\n";
    std::cout << "  2) Busca em Profundidade Limitada Iterativa (IDDFS)\n";
    std::cout << "  3) A*\n";
    std::cout << "Opcao: ";
    int opcao;
    std::cin >> opcao;

    // Limites de seguranca: BFS/IDDFS crescem MUITO rapido (o numero
    // de estados cresce como (numero de movimentos)^profundidade), por
    // isso usamos limites conservadores para cubos pouco embaralhados.
    const int PROFUNDIDADE_MAXIMA_BUSCA_CEGA = 8;
    const long LIMITE_ESTADOS_ASTAR = 5000000;

    ResultadoBusca resultado;
    auto inicio = std::chrono::steady_clock::now();

    if (opcao == 1) {
        resultado = resolverComBFS(estadoEmbaralhado, PROFUNDIDADE_MAXIMA_BUSCA_CEGA);
    } else if (opcao == 2) {
        resultado = resolverComIDDFS(estadoEmbaralhado, PROFUNDIDADE_MAXIMA_BUSCA_CEGA);
    } else if (opcao == 3) {
        resultado = resolverComAStar(estadoEmbaralhado, LIMITE_ESTADOS_ASTAR);
    } else {
        std::cout << "Opcao invalida.\n";
        return;
    }

    auto fim = std::chrono::steady_clock::now();
    double segundos = std::chrono::duration<double>(fim - inicio).count();
    mostrarSolucao(resultado, segundos);
}

int main() {
    std::cout << "==========================================\n";
    std::cout << " SIMULADOR E SOLUCIONADOR DE CUBO 2x2x2\n";
    std::cout << "==========================================\n";

    std::cout << "Digite uma SEED (numero inteiro) para embaralhar o cubo: ";
    unsigned int seed;
    std::cin >> seed;

    std::cout << "Quantos movimentos de embaralhamento (ex: 5)? ";
    int quantidade;
    std::cin >> quantidade;

    CubeState cuboBase; // comeca resolvido
    std::vector<int> movimentosEmbaralho = embaralhar(cuboBase, seed, quantidade);

    std::cout << "\nCubo embaralhado com seed " << seed << " (" << quantidade << " movimentos): ";
    for (int m : movimentosEmbaralho) std::cout << nomeMovimento(m) << " ";
    std::cout << "\n";
    imprimirCubo(cuboBase);

    while (true) {
        std::cout << "\n===== MENU PRINCIPAL =====\n";
        std::cout << "1) Jogar manualmente\n";
        std::cout << "2) Resolver com IA (BFS / IDDFS / A*)\n";
        std::cout << "3) Reembaralhar (nova seed)\n";
        std::cout << "0) Sair\n";
        std::cout << "Opcao: ";

        int opcao;
        std::cin >> opcao;

        if (opcao == 0) {
            break;
        } else if (opcao == 1) {
            jogarManualmente(cuboBase);
        } else if (opcao == 2) {
            resolverComIA(cuboBase);
        } else if (opcao == 3) {
            std::cout << "Nova seed: ";
            std::cin >> seed;
            std::cout << "Quantidade de movimentos: ";
            std::cin >> quantidade;
            cuboBase = CubeState();
            movimentosEmbaralho = embaralhar(cuboBase, seed, quantidade);
            imprimirCubo(cuboBase);
        } else {
            std::cout << "Opcao invalida.\n";
        }
    }

    std::cout << "Ate mais!\n";
    return 0;
}
