#ifndef CUBE_STATE_H
#define CUBE_STATE_H

#include <string>
#include <vector>

// =====================================================================
// REQUISITO CONCEITUAL 1: ESTADO (State)
// =====================================================================
// O cubo 2x2x2 possui apenas 8 peças de CANTO (não tem arestas nem
// centros como o cubo 3x3x3 tradicional). Cada canto ocupa uma das
// 8 "casas" (posições) do cubo e pode estar torcido em 3 formas.
//
// Nomeamos as 8 posições pelas 3 faces que se encontram ali:
//   0 = UFR (Up-Front-Right)      4 = DFR (Down-Front-Right)
//   1 = UFL (Up-Front-Left)       5 = DFL (Down-Front-Left)
//   2 = UBL (Up-Back-Left)        6 = DBL (Down-Back-Left)
//   3 = UBR (Up-Back-Right)       7 = DBR (Down-Back-Right)
//
// O Estado guarda, para cada POSIÇÃO i (0..7):
//   perm[i] = qual peça (0..7) está fisicamente ocupando a posição i
//   ori[i]  = como essa peça está torcida naquela posição (0, 1 ou 2)
//
// O estado OBJETIVO (cubo resolvido) é perm[i] == i e ori[i] == 0
// para todo i, ou seja, cada peça está na sua casa de origem e sem
// nenhuma torção.
// =====================================================================
struct CubeState {
    int perm[8]; // perm[posicao] = id da peca que esta ali
    int ori[8];  // ori[posicao]  = orientacao (0,1,2) da peca que esta ali

    CubeState(); // constrói o cubo já resolvido (estado objetivo)

    bool operator==(const CubeState& other) const;

    // Serializa o estado como uma string curta (ex: "01234567" + "00000000")
    // usada para comparar/guardar estados já visitados na busca.
    std::string encode() const;
};

// Nomes das 8 posições e das 6 faces, usados para imprimir o cubo.
enum Posicao { UFR = 0, UFL = 1, UBL = 2, UBR = 3, DFR = 4, DFL = 5, DBL = 6, DBR = 7 };
enum Face    { U = 0, D = 1, F = 2, B = 3, L = 4, R = 5 };

// Os 12 movimentos possíveis: cada uma das 6 faces, no sentido horário
// (CW) ou anti-horário (CCW). Guardamos como um único índice 0..11:
// pares (CW, CCW) consecutivos: U,U', D,D', F,F', B,B', L,L', R,R'.
enum Movimento {
    MOV_U = 0, MOV_U_LINHA = 1,
    MOV_D = 2, MOV_D_LINHA = 3,
    MOV_F = 4, MOV_F_LINHA = 5,
    MOV_B = 6, MOV_B_LINHA = 7,
    MOV_L = 8, MOV_L_LINHA = 9,
    MOV_R = 10, MOV_R_LINHA = 11,
    NUM_MOVIMENTOS = 12
};

// Verifica se o estado é o objetivo (cubo resolvido).
bool ehObjetivo(const CubeState& estado);

// Aplica um movimento (0..11) ao estado e RETORNA o novo estado
// (não altera o estado recebido).
CubeState aplicarMovimento(const CubeState& estado, int movimento);

// Devolve o índice do movimento inverso (ex.: inverso de U é U').
int movimentoInverso(int movimento);

// Nome legível do movimento, ex: "U", "U'", "R2"... (aqui só "U"/"U'").
std::string nomeMovimento(int movimento);

// =====================================================================
// REQUISITO CONCEITUAL 2: FUNÇÃO SUCESSORA
// =====================================================================
// Dado um estado, devolve todos os estados alcançáveis a partir dele
// aplicando cada um dos 12 movimentos válidos.
//
// Pequena otimização (opcional, mas comum em quebra-cabeças de busca):
// não geramos o movimento que simplesmente desfaz o último movimento
// aplicado (ex.: se o último foi U, não geramos U'), pois isso levaria
// de volta ao estado anterior e nunca ajuda a encontrar uma solução
// mais curta. Use ultimoMovimento = -1 quando não houver movimento anterior.
// =====================================================================
struct Sucessor {
    CubeState estado;
    int movimento; // qual movimento gerou este estado
};

std::vector<Sucessor> gerarSucessores(const CubeState& estado, int ultimoMovimento);

// =====================================================================
// REQUISITO CONCEITUAL 3: FUNÇÃO AVALIADORA / HEURÍSTICA (para o A*)
// =====================================================================
// Conta quantas peças estão fora do lugar (posição errada OU orientação
// errada) e divide por 4 (arredondando para cima), pois cada movimento
// do cubo mexe em, no máximo, 4 peças de canto ao mesmo tempo.
// Isso garante que a heurística NUNCA superestima o custo real restante
// (heurística admissível), condição necessária para o A* encontrar a
// solução ótima (menor número de movimentos).
// =====================================================================
int heuristica(const CubeState& estado);

// Embaralha o cubo a partir de uma SEED, para permitir reproduzir
// sempre o mesmo embaralhamento. Devolve também a lista de movimentos
// aplicados (útil só para conferência/depuração).
std::vector<int> embaralhar(CubeState& estado, unsigned int seed, int quantidadeMovimentos);

// Imprime o cubo no terminal, com as 6 faces "desdobradas" em formato
// de cruz, cada uma mostrando seus 4 adesivos (2x2), usando cores ANSI.
void imprimirCubo(const CubeState& estado);

#endif
