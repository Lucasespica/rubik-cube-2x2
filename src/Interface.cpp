#include "Interface.hpp"
#include <iostream>
#include <string>

#ifdef _WIN32
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