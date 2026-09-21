#include "cube_state.h"
#include <random>
#include <cmath>
#include <iostream>

CubeState::CubeState() {
    for (int i = 0; i < 8; i++) {
        perm[i] = i; // cada peca comeca na sua propria casa
        ori[i] = 0;  // sem nenhuma torcao
    }
}

bool CubeState::operator==(const CubeState& other) const {
    for (int i = 0; i < 8; i++) {
        if (perm[i] != other.perm[i] || ori[i] != other.ori[i]) return false;
    }
    return true;
}

std::string CubeState::encode() const {
    std::string s;
    s.reserve(16);
    for (int i = 0; i < 8; i++) s += char('0' + perm[i]);
    for (int i = 0; i < 8; i++) s += char('0' + ori[i]);
    return s;
}

bool ehObjetivo(const CubeState& estado) {
    CubeState objetivo; // construtor padrao ja e o objetivo
    return estado == objetivo;
}

// ---------------------------------------------------------------------
// TABELA DE MOVIMENTOS BASE (sentido horario)
// ---------------------------------------------------------------------
// Cada face, ao girar 90 graus no sentido horario, movimenta um ciclo
// de 4 posicoes: a peca que estava em pos[i] passa a ocupar pos[(i+1)%4].
// "delta[k]" e a torcao (0,1 ou 2) somada a peca que CHEGA na posicao
// pos[k]. Para os movimentos U/D as pecas nao torcem (delta = 0,0,0,0).
// Para F/B/L/R as pecas torcem alternadamente (+1,+2,+1,+2), o que soma
// 6 (multiplo de 3) apos o ciclo completo de 4 giros -- ou seja, girar
// a mesma face 4 vezes sempre devolve o cubo exatamente como estava.
//
// Observacao: a direcao exata (horario "visto de que lado") e uma
// escolha de convencao interna deste simulador; o importante, para os
// algoritmos de busca, e que o movimento seja uma bijecao consistente
// e reversivel, o que garantimos matematicamente acima.
// ---------------------------------------------------------------------
struct MovimentoBase {
    int pos[4];
    int delta[4];
};

static const MovimentoBase TABELA[6] = {
    /* U */ { {UFR, UBR, UBL, UFL}, {0, 0, 0, 0} },
    /* D */ { {DFR, DFL, DBL, DBR}, {0, 0, 0, 0} },
    /* F */ { {UFR, UFL, DFL, DFR}, {1, 2, 1, 2} },
    /* B */ { {UBL, UBR, DBR, DBL}, {1, 2, 1, 2} },
    /* L */ { {UFL, UBL, DBL, DFL}, {1, 2, 1, 2} },
    /* R */ { {UBR, UFR, DFR, DBR}, {1, 2, 1, 2} },
};

static CubeState aplicarMovimentoBase(const CubeState& estado, int face) {
    CubeState novo = estado;
    const MovimentoBase& m = TABELA[face];

    int permAntigo[4], oriAntigo[4];
    for (int i = 0; i < 4; i++) {
        permAntigo[i] = estado.perm[m.pos[i]];
        oriAntigo[i] = estado.ori[m.pos[i]];
    }
    for (int i = 0; i < 4; i++) {
        int destino = (i + 1) % 4;
        novo.perm[m.pos[destino]] = permAntigo[i];
        novo.ori[m.pos[destino]] = (oriAntigo[i] + m.delta[destino]) % 3;
    }
    return novo;
}

CubeState aplicarMovimento(const CubeState& estado, int movimento) {
    int face = movimento / 2;         // 0=U,1=D,2=F,3=B,4=L,5=R
    bool sentidoHorario = (movimento % 2 == 0);
    int vezes = sentidoHorario ? 1 : 3; // 3 giros horarios = 1 giro anti-horario

    CubeState atual = estado;
    for (int i = 0; i < vezes; i++) {
        atual = aplicarMovimentoBase(atual, face);
    }
    return atual;
}

int movimentoInverso(int movimento) {
    return movimento ^ 1; // troca CW <-> CCW da mesma face (0<->1, 2<->3, ...)
}

std::string nomeMovimento(int movimento) {
    static const char* nomesFace[6] = {"U", "D", "F", "B", "L", "R"};
    int face = movimento / 2;
    bool sentidoHorario = (movimento % 2 == 0);
    std::string nome = nomesFace[face];
    if (!sentidoHorario) nome += "'";
    return nome;
}

std::vector<Sucessor> gerarSucessores(const CubeState& estado, int ultimoMovimento) {
    std::vector<Sucessor> sucessores;
    sucessores.reserve(NUM_MOVIMENTOS);
    for (int mov = 0; mov < NUM_MOVIMENTOS; mov++) {
        // otimizacao: nao desfazer imediatamente o ultimo movimento
        if (ultimoMovimento != -1 && mov == movimentoInverso(ultimoMovimento)) {
            continue;
        }
        Sucessor s;
        s.movimento = mov;
        s.estado = aplicarMovimento(estado, mov);
        sucessores.push_back(s);
    }
    return sucessores;
}

int heuristica(const CubeState& estado) {
    int foraDoLugar = 0;
    for (int i = 0; i < 8; i++) {
        if (estado.perm[i] != i || estado.ori[i] != 0) foraDoLugar++;
    }
    // cada movimento mexe em no maximo 4 pecas -> heuristica admissivel
    return (foraDoLugar + 3) / 4; // divisao inteira arredondando para cima
}

std::vector<int> embaralhar(CubeState& estado, unsigned int seed, int quantidadeMovimentos) {
    std::mt19937 gerador(seed); // gerador pseudo-aleatorio com SEED fixa
    std::uniform_int_distribution<int> dist(0, NUM_MOVIMENTOS - 1);

    std::vector<int> aplicados;
    int ultimo = -1;
    for (int i = 0; i < quantidadeMovimentos; i++) {
        int mov;
        do {
            mov = dist(gerador);
        } while (ultimo != -1 && mov == movimentoInverso(ultimo)); // evita desfazer o anterior

        estado = aplicarMovimento(estado, mov);
        aplicados.push_back(mov);
        ultimo = mov;
    }
    return aplicados;
}

// ---------------------------------------------------------------------
// IMPRESSAO DO CUBO (planificado em cruz)
// ---------------------------------------------------------------------
// Cada peca de canto "nasce" (estado resolvido) com 3 cores fixas, uma
// para cada eixo: eixo Up/Down, eixo Front/Back, eixo Left/Right.
// A tabela abaixo diz, para cada peca (0..7), essas 3 cores de origem.
static const char CORES_PECA[8][3] = {
    /* 0 UFR */ {'W', 'G', 'R'},
    /* 1 UFL */ {'W', 'G', 'O'},
    /* 2 UBL */ {'W', 'B', 'O'},
    /* 3 UBR */ {'W', 'B', 'R'},
    /* 4 DFR */ {'Y', 'G', 'R'},
    /* 5 DFL */ {'Y', 'G', 'O'},
    /* 6 DBL */ {'Y', 'B', 'O'},
    /* 7 DBR */ {'Y', 'B', 'R'},
};
// eixo: 0 = Up/Down, 1 = Front/Back, 2 = Left/Right

// Dada a peca que ocupa uma posicao e sua orientacao, devolve a cor
// exibida no eixo pedido (0,1,2), aplicando a rotacao da orientacao.
static char corNoEixo(int peca, int orientacao, int eixo) {
    // orientacao 0: cor do proprio eixo
    // orientacao 1: roda ciclicamente (eixo 0 mostra a cor do eixo 1, etc)
    // orientacao 2: roda no sentido contrario
    int eixoOrigem = (eixo + orientacao) % 3;
    return CORES_PECA[peca][eixoOrigem];
}

// Para cada posicao, quais eixos correspondem as faces U/D, F/B, L/R
// que aquela posicao toca (usado para achar a cor certa a mostrar).
struct EixosDaPosicao { int eixoUD, eixoFB, eixoLR; };
static const EixosDaPosicao EIXOS[8] = {
    /*0 UFR*/ {0, 1, 2}, /*1 UFL*/ {0, 1, 2}, /*2 UBL*/ {0, 1, 2}, /*3 UBR*/ {0, 1, 2},
    /*4 DFR*/ {0, 1, 2}, /*5 DFL*/ {0, 1, 2}, /*6 DBL*/ {0, 1, 2}, /*7 DBR*/ {0, 1, 2},
};
// (os 3 eixos fisicos de qualquer posicao sao sempre U/D=0, F/B=1, L/R=2;
//  a tabela existe so para deixar o codigo abaixo mais legivel/explicito)

static char corExibidaNaFace(const CubeState& estado, int posicao, int eixoDaFace) {
    int peca = estado.perm[posicao];
    int orientacao = estado.ori[posicao];
    return corNoEixo(peca, orientacao, eixoDaFace);
}

// codigo ANSI de cor para cada letra de cor
static std::string ansi(char cor) {
    switch (cor) {
        case 'W': return "\033[47m\033[30m"; // fundo branco, texto preto
        case 'Y': return "\033[43m\033[30m"; // fundo amarelo
        case 'G': return "\033[42m\033[30m"; // fundo verde
        case 'B': return "\033[44m\033[97m"; // fundo azul
        case 'O': return "\033[48;5;208m\033[30m"; // fundo laranja
        case 'R': return "\033[41m\033[97m"; // fundo vermelho
        default:  return "\033[0m";
    }
}
static const char* RESET = "\033[0m";

static void imprimirAdesivo(char cor) {
    std::cout << ansi(cor) << " " << cor << " " << RESET;
}

void imprimirCubo(const CubeState& e) {
    // Cada face e impressa como uma grade 2x2. Definimos, para cada
    // face, quais posicoes/eixo aparecem no canto superior-esquerdo,
    // superior-direito, inferior-esquerdo e inferior-direito.
    // (E uma planificacao simplificada, escolhida para ser facil de
    // programar e de entender -- nao segue rigorosamente a notacao
    // oficial de competicao, mas mantem a coerencia entre as faces.)

    auto U_TL = corExibidaNaFace(e, UBL, 0), U_TR = corExibidaNaFace(e, UBR, 0);
    auto U_BL = corExibidaNaFace(e, UFL, 0), U_BR = corExibidaNaFace(e, UFR, 0);

    auto D_TL = corExibidaNaFace(e, DFL, 0), D_TR = corExibidaNaFace(e, DFR, 0);
    auto D_BL = corExibidaNaFace(e, DBL, 0), D_BR = corExibidaNaFace(e, DBR, 0);

    auto F_TL = corExibidaNaFace(e, UFL, 1), F_TR = corExibidaNaFace(e, UFR, 1);
    auto F_BL = corExibidaNaFace(e, DFL, 1), F_BR = corExibidaNaFace(e, DFR, 1);

    auto B_TL = corExibidaNaFace(e, UBR, 1), B_TR = corExibidaNaFace(e, UBL, 1);
    auto B_BL = corExibidaNaFace(e, DBR, 1), B_BR = corExibidaNaFace(e, DBL, 1);

    auto L_TL = corExibidaNaFace(e, UBL, 2), L_TR = corExibidaNaFace(e, UFL, 2);
    auto L_BL = corExibidaNaFace(e, DBL, 2), L_BR = corExibidaNaFace(e, DFL, 2);

    auto R_TL = corExibidaNaFace(e, UFR, 2), R_TR = corExibidaNaFace(e, UBR, 2);
    auto R_BL = corExibidaNaFace(e, DFR, 2), R_BR = corExibidaNaFace(e, DBR, 2);

    std::cout << "\n";
    // linha da face U
    std::cout << "         ";
    imprimirAdesivo(U_TL); imprimirAdesivo(U_TR); std::cout << "\n";
    std::cout << "         ";
    imprimirAdesivo(U_BL); imprimirAdesivo(U_BR); std::cout << "\n";

    // linha L F R B
    imprimirAdesivo(L_TL); imprimirAdesivo(L_TR);
    imprimirAdesivo(F_TL); imprimirAdesivo(F_TR);
    imprimirAdesivo(R_TL); imprimirAdesivo(R_TR);
    imprimirAdesivo(B_TL); imprimirAdesivo(B_TR);
    std::cout << "\n";
    imprimirAdesivo(L_BL); imprimirAdesivo(L_BR);
    imprimirAdesivo(F_BL); imprimirAdesivo(F_BR);
    imprimirAdesivo(R_BL); imprimirAdesivo(R_BR);
    imprimirAdesivo(B_BL); imprimirAdesivo(B_BR);
    std::cout << "\n";

    // linha da face D
    std::cout << "         ";
    imprimirAdesivo(D_TL); imprimirAdesivo(D_TR); std::cout << "\n";
    std::cout << "         ";
    imprimirAdesivo(D_BL); imprimirAdesivo(D_BR); std::cout << "\n\n";
    std::cout << "Legenda: W=Branco Y=Amarelo G=Verde B=Azul O=Laranja R=Vermelho\n";
    std::cout << "Ordem no desenho: U em cima | L F R B na faixa do meio | D embaixo\n\n";
}
