#include <iostream>
#include <limits>
#include "cubo.hpp"
#include "busca.hpp"

void exibir_cubo(const Cubo& cubo) {
    std::cout << "\nEstado Atual do Cubo: " << cubo.get_estado() << "\n";
    if (cubo.is_resolvido()) {
        std::cout << "-> O CUBO ESTA RESOLVIDO!\n";
    }
}

// Le um inteiro validando a entrada. Se o usuario digitar algo que nao e
// numero, "cin >> valor" falharia e a variavel ficaria com lixo; aqui ele
// avisa e pede de novo em vez de travar/terminar silenciosamente.
int ler_inteiro(const std::string& prompt) {
    int valor;
    while (true) {
        std::cout << prompt;
        if (std::cin >> valor) {
            return valor;
        }
        std::cout << "Entrada invalida. Digite um numero inteiro.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Modo manual: o jogador tenta resolver o cubo (ja embaralhado) na mao.
void jogar_manualmente(Cubo cubo) {
    std::string mov;
    std::cout << "\nTente resolver o cubo! Digite comandos (R, R', U, U', F, F') ou 'S' para sair.\n";
    while (true) {
        exibir_cubo(cubo);
        if (cubo.is_resolvido()) break; // parabens, ja resolveu
        std::cout << "Movimento: ";
        std::cin >> mov;
        if (mov == "S" || mov == "s") break;

        if (!Cubo::movimento_valido(mov)) {
            std::cout << "Movimento invalido! Use um destes: R, R', U, U', F, F'\n";
            continue;
        }
        cubo = Cubo(cubo.aplicar_movimento(cubo.get_estado(), mov));
    }
}

// Modo IA: resolve o cubo (ja embaralhado) com o algoritmo escolhido.
void resolver_com_ia(const Cubo& cubo_embaralhado) {
    int alg = ler_inteiro("\nEscolha o Algoritmo:\n1. Busca em Largura (BFS)\n2. IDDFS (Iterativa)\n3. Busca A*\nEscolha: ");

    if (alg == 1) {
        FilaBFS fila;
        std::cout << "\nExecutando BFS...\n";
        if (!busca_generica(cubo_embaralhado, &fila)) std::cout << "Sem solucao.\n";
    }
    else if (alg == 2) {
        std::cout << "\nExecutando IDDFS...\n";
        busca_iddfs_iterativa(cubo_embaralhado);
    }
    else if (alg == 3) {
        FilaPrioridadeAEstrela pq;
        std::cout << "\nExecutando A*...\n";
        if (!busca_generica(cubo_embaralhado, &pq)) std::cout << "Sem solucao.\n";
    }
    else {
        std::cout << "Algoritmo invalido.\n";
    }
}

int main() {
    std::cout << "=== SIMULADOR DE CUBO MAGICO 2X2 ===\n";

    // 1) Primeiro embaralha, com seed escolhida pelo usuario.
    int seed = ler_inteiro("Defina a Seed para embaralhar: ");
    int num_movs = ler_inteiro("Quantos movimentos de embaralhamento? (Recomendado 5 a 12 para testes rapidos): ");
    if (num_movs < 0) {
        std::cout << "Numero de movimentos invalido, usando 0.\n";
        num_movs = 0;
    }

    Cubo cubo_embaralhado;
    cubo_embaralhado.embaralhar(seed, num_movs);
    std::cout << "\nCubo embaralhado!";
    exibir_cubo(cubo_embaralhado);

    // 2) So depois de embaralhado, escolhe como quer resolver.
    std::cout << "\n1. Jogar Manualmente (tentar resolver este cubo)\n";
    std::cout << "2. Resolver com Inteligencia Artificial (BFS, IDDFS, A*)\n";
    int opcao = ler_inteiro("Escolha: ");

    if (opcao == 1) {
        jogar_manualmente(cubo_embaralhado);
    }
    else if (opcao == 2) {
        resolver_com_ia(cubo_embaralhado);
    }
    else {
        std::cout << "Opcao invalida.\n";
    }
    return 0;
}