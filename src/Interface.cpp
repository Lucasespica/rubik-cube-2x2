#include "Interface.hpp"
#include <iostream>
#include <string>
#include <limits>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

void habilitarCoresWindows() {
#ifdef _WIN32
    // Pega o "handle" da saida padrao do console e liga a flag que
    // faz ele interpretar codigos ANSI em vez de imprimi-los crus.
    HANDLE saida = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo = 0;
    if (GetConsoleMode(saida, &modo)) {
        SetConsoleMode(saida, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
}

namespace {

    // "\033[48;5;Nm" muda so a COR DE FUNDO, usando a paleta
    // estendida de 256 cores (mais fiel que as 8 cores basicas do
    // ANSI). O segundo codigo muda a cor do TEXTO, pra letra
    // continuar legivel em cima do fundo.
    const char* corParaAnsi(Cor c) {
        switch (c) {
            case Cor::BRANCO:   return "\033[48;5;231m\033[30m"; // fundo branco, texto preto
            case Cor::AMARELO:  return "\033[48;5;226m\033[30m"; // fundo amarelo, texto preto
            case Cor::VERDE:    return "\033[48;5;46m\033[30m";  // fundo verde, texto preto
            case Cor::AZUL:     return "\033[48;5;21m\033[97m";  // fundo azul, texto branco
            case Cor::LARANJA:  return "\033[48;5;208m\033[30m"; // fundo laranja, texto preto
            case Cor::VERMELHO: return "\033[48;5;196m\033[97m"; // fundo vermelho, texto branco
        }
        return "\033[0m";
    }

    const char RESET[] = "\033[0m";  // volta a cor padrao do terminal

    char corParaLetra(Cor c) {
        switch (c) {
            case Cor::BRANCO:   return 'B';
            case Cor::AMARELO:  return 'A';
            case Cor::VERDE:    return 'V';
            case Cor::AZUL:     return 'Z';
            case Cor::LARANJA:  return 'L';
            case Cor::VERMELHO: return 'R';
        }
        return '?';
    }

    // Imprime UM adesivo, como "[X]" colorido, sem pular linha.
    void imprimirSticker(const Cubo& cubo, int face, int sticker) {
        Cor cor = cubo.corNaPosicao(face, sticker);
        std::cout << corParaAnsi(cor) << '[' << corParaLetra(cor) << ']' << RESET;
    }

    // Imprime 2 adesivos lado a lado (uma "linha" de uma face).
    // primeiroSticker = 0 -> imprime os stickers 0 e 1 (fileira de cima)
    // primeiroSticker = 2 -> imprime os stickers 2 e 3 (fileira de baixo)
    void imprimirLinhaFace(const Cubo& cubo, int face, int primeiroSticker) {
        imprimirSticker(cubo, face, primeiroSticker);
        imprimirSticker(cubo, face, primeiroSticker + 1);
    }

}  // namespace

void imprimirCubo(const Cubo& cubo) {
    // Um "espaco de face" = 2 adesivos de 3 caracteres cada = 6 espacos.
    // E o quanto U e D precisam recuar pra ficar alinhados com a
    // fileira do meio (L F R B).
    const std::string indent(6, ' ');

    // Face U (topo)
    std::cout << indent; imprimirLinhaFace(cubo, 0, 0); std::cout << '\n';
    std::cout << indent; imprimirLinhaFace(cubo, 0, 2); std::cout << '\n';

    // Fileira do meio: L, F, R, B lado a lado
    imprimirLinhaFace(cubo, 1, 0); imprimirLinhaFace(cubo, 2, 0);
    imprimirLinhaFace(cubo, 3, 0); imprimirLinhaFace(cubo, 4, 0);
    std::cout << '\n';
    imprimirLinhaFace(cubo, 1, 2); imprimirLinhaFace(cubo, 2, 2);
    imprimirLinhaFace(cubo, 3, 2); imprimirLinhaFace(cubo, 4, 2);
    std::cout << '\n';

    // Face D (base)
    std::cout << indent; imprimirLinhaFace(cubo, 5, 0); std::cout << '\n';
    std::cout << indent; imprimirLinhaFace(cubo, 5, 2); std::cout << '\n';
}

void imprimirLegenda() {
    std::cout << "Legenda: ";
    for (Cor c : {Cor::BRANCO, Cor::AMARELO, Cor::VERDE,
                  Cor::AZUL, Cor::LARANJA, Cor::VERMELHO}) {
        std::cout << corParaAnsi(c) << '[' << corParaLetra(c) << ']' << RESET << ' ';
    }
    std::cout << '\n';
}

std::optional<Movimento> movimentoDoTexto(const std::string& texto) {
    if (texto.empty()) {
        return std::nullopt;
    }

    // toupper aceita minuscula tambem (ex: "u" vira "U")
    char letra = std::toupper(static_cast<unsigned char>(texto[0]));
    // se o texto tiver um segundo caractere e for aspa, e o movimento invertido
    bool linha = (texto.size() > 1 && texto[1] == '\'');

    switch (letra) {
        case 'U': return linha ? Movimento::U_LINHA : Movimento::U;
        case 'D': return linha ? Movimento::D_LINHA : Movimento::D;
        case 'L': return linha ? Movimento::L_LINHA : Movimento::L;
        case 'R': return linha ? Movimento::R_LINHA : Movimento::R;
        case 'F': return linha ? Movimento::F_LINHA : Movimento::F;
        case 'B': return linha ? Movimento::B_LINHA : Movimento::B;
        default:  return std::nullopt;
    }
}

void imprimirResultado(const std::string& nome, const ResultadoBusca& resultado) {
    std::cout << "\n== " << nome << " ==\n";
    std::cout << "Estados visitados: " << resultado.visitados << "\n";

    if (!resultado.encontrou) {
        std::cout << "Sem solucao.\n";
        return;
    }

    std::cout << "Movimentos (" << resultado.caminho.size() << "): ";
    for (Movimento m : resultado.caminho) {
        std::cout << nomeMovimento(m) << ' ';
    }
    std::cout << '\n';
}

int lerInteiro(const std::string& pergunta) {
    while (true) {
        std::cout << pergunta;
        int valor;
        if (std::cin >> valor) {
            return valor;
        }
        // cin.fail() aconteceu, a pessoa digitou algo que nao e numero.
        // clear() tira o cin do estado de erro; ignore() descarta o que
        // ela digitou. Sem isso, o mesmo texto invalido ficaria preso
        // no buffer e o programa reclamaria pra sempre, sem parar pra
        // esperar uma nova entrada.
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada invalida. Digite um numero.\n";
    }
}

unsigned lerSeed(const std::string& pergunta) {
    while (true) {
        std::cout << pergunta;
        unsigned valor;
        if (std::cin >> valor) {
            return valor;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada invalida. Digite um numero.\n";
    }
}

void jogar() {
    std::cout << "\n=== MODO JOGAR ===\n";
    unsigned seed = lerSeed("Seed do embaralhamento: ");
    int numMovimentos = lerInteiro("Quantos movimentos embaralhar: ");

    Cubo cubo = Cubo::embaralhado(seed, numMovimentos);
    int jogadas = 0;

    std::cout << "\nComandos: U D L R F B (com ' pro sentido inverso, ex: U')\n";
    std::cout << "Digite 'sair' pra voltar ao menu.\n";

    while (true) {
        std::cout << '\n';
        imprimirCubo(cubo);

        if (cubo.estaResolvido()) {
            std::cout << "\nParabens! Voce resolveu o cubo em " << jogadas << " jogadas.\n";
            return;
        }

        std::cout << "\nMovimento: ";
        std::string entrada;
        std::cin >> entrada;

        if (entrada == "sair") {
            std::cout << "Voltando ao menu.\n";
            return;
        }

        std::optional<Movimento> movimento = movimentoDoTexto(entrada);
        if (!movimento.has_value()) {
            std::cout << "Movimento nao reconhecido. Tente de novo.\n";
            continue;
        }

        cubo = cubo.aplicarMovimento(*movimento);
        ++jogadas;
    }
}

void iaJoga() {
    unsigned seed = lerSeed("Seed do embaralhamento: ");
    int numMovimentos = lerInteiro("Quantos movimentos embaralhar: ");

    Cubo embaralhado = Cubo::embaralhado(seed, numMovimentos);
    std::cout << "\nCubo embaralhado:\n";
    imprimirCubo(embaralhado);

    std::cout << "\nQual algoritmo?\n";
    std::cout << "1 - Busca em Largura (BFS)\n";
    std::cout << "2 - Busca em Profundidade Limitada Iterativa (IDDFS)\n";
    std::cout << "3 - A*\n";
    std::cout << "4 - Todos (compara)\n";
    int escolha = lerInteiro("Escolha: ");

    // O reverso exato do embaralhamento sempre resolve o cubo, entao
    // 'numMovimentos' e um limite de profundidade seguro pro IDDFS.
    ResultadoBusca resultado;
    if (escolha == 1) {
        resultado = buscarBFS(embaralhado);
        imprimirResultado("BFS", resultado);
    } else if (escolha == 2) {
        resultado = buscarIDDFS(embaralhado, numMovimentos);
        imprimirResultado("IDDFS", resultado);
    } else if (escolha == 3) {
        resultado = buscarAEstrela(embaralhado);
        imprimirResultado("A*", resultado);
    } else if (escolha == 4) {
        imprimirResultado("BFS", buscarBFS(embaralhado));
        imprimirResultado("IDDFS", buscarIDDFS(embaralhado, numMovimentos));
        resultado = buscarAEstrela(embaralhado);
        imprimirResultado("A*", resultado);
    } else {
        std::cout << "Opcao invalida.\n";
        return;
    }

    if (!resultado.encontrou) {
        return;
    }

    std::cout << "\nVer a solucao passo a passo? (s/n): ";
    std::string resposta;
    std::cin >> resposta;
    if (resposta != "s" && resposta != "S") {
        return;
    }

    Cubo passo = embaralhado;
    std::cout << '\n';
    imprimirCubo(passo);
    for (Movimento m : resultado.caminho) {
        std::cout << "\n[Enter] proximo movimento: " << nomeMovimento(m) << '\n';
        // limpa o resto da linha anterior no buffer, depois espera o Enter de verdade
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();

        passo = passo.aplicarMovimento(m);
        imprimirCubo(passo);
    }
    std::cout << "\nResolvido!";
}