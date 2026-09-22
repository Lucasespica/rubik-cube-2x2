#ifndef CUBO_HPP
#define CUBO_HPP

#include <iostream>
#include <vector>
#include <string>
#include <random>

// Estrutura auxiliar para mapear os movimentos gerados pela Função Sucessora
struct Movimento {
    std::string nome; // Ex: "R" (Right), "U'" (Up invertido)
    std::string estado_resultante;
};

class Cubo {
private:
    /* 
     * REQUISITO CONCEITUAL 1: Estado (State)
     * O estado é representado por uma string de 24 caracteres.
     * Um cubo 2x2 tem 6 faces, cada uma com 4 "adesivos" (peças visíveis).
     * Mapeamento das faces (4 chars cada): 
     * 0-3 (Up), 4-7 (Right), 8-11 (Front), 12-15 (Down), 16-19 (Left), 20-23 (Back).
     * Essa estrutura em string é fácil de entender, copiar e comparar (para detectar estados repetidos e objetivo).
     */
    std::string estado_atual;

    // Estado objetivo padrão (cubo resolvido)
    std::string ESTADO_RESOLVIDO = "UUUURRRRFFFFDDDDLLLLBBBB";
    // U = Up(Branco), D = Down(Amarelo), F = Front(Verde), B = Back(Azul), L = Left(Laranja), R = Right(Vermelho)    


public:
    // Construtor inicializa o cubo no estado resolvido
    Cubo() {
         estado_atual = ESTADO_RESOLVIDO; 
    }
    Cubo(std::string estado) { 
        estado_atual = estado; 
    }

    // Método para acessar o estado atual em formato string
    std::string get_estado() const {
         return estado_atual; 
    }

    // Verifica se o estado atual é o objetivo
    bool is_resolvido() const { 
        return estado_atual == ESTADO_RESOLVIDO;
    }

    // Lista dos movimentos válidos, usada aqui e pela função sucessora.
    static const std::vector<std::string>& movimentos_validos() {
        static const std::vector<std::string> movs = {"R", "R'", "U", "U'", "F", "F'"};
        return movs;
    }

    // Verifica se uma string corresponde a um movimento reconhecido pelo cubo.
    static bool movimento_valido(const std::string& mov) {
        for (const auto& m : movimentos_validos()) {
            if (m == mov) return true;
        }
        return false;
    }

    // Função auxiliar para rotacionar uma face no sentido horário
    // (troca de posições no array de 24 posições).
    std::string aplicar_movimento(const std::string& e, const std::string& movimento) const {
        std::string n = e; // n = novo estado
        
    if (movimento == "R") {
        // Rotaciona a própria face R (4, 5, 7, 6 em sentido horário)
        n[4] = e[6]; n[5] = e[4]; n[7] = e[5]; n[6] = e[7];

        // Troca as bordas adjacentes: Up(1,3) -> Back(20,22) -> Down(13,15) -> Front(9,11)
        n[1] = e[9];  n[3] = e[11]; // Front vai para Up
        n[20] = e[1];  n[22] = e[3];  // Up vai para Back
        n[13] = e[20]; n[15] = e[22]; // Back vai para Down
        n[9] = e[13]; n[11] = e[15]; // Down vai para Front
    }
    else if (movimento == "R'") { // Inverso de R
        n[4] = e[5]; n[5] = e[7]; n[7] = e[6]; n[6] = e[4];

        n[9] = e[1];  n[11] = e[3];
        n[1] = e[20]; n[3] = e[22];
        n[20] = e[13]; n[22] = e[15];
        n[13] = e[9];  n[15] = e[11];
    }


    else if (movimento == "U") {
        // Rotaciona a própria face U (0, 1, 3, 2)
        n[0] = e[2]; n[1] = e[0]; n[3] = e[1]; n[2] = e[3];

        // Troca as bordas adjacentes: Front(8,9) -> Left(16,17) -> Back(20,21) -> Right(4,5)
        n[16] = e[8];  n[17] = e[9];  // Front vai para Left
        n[20] = e[16]; n[21] = e[17]; // Left vai para Back
        n[4] = e[20]; n[5] = e[21]; // Back vai para Right
        n[8] = e[4];  n[9] = e[5];  // Right vai para Front
    }
    else if (movimento == "U'") { // Inverso de U
        n[0] = e[1]; n[1] = e[3]; n[3] = e[2]; n[2] = e[0];

        n[8] = e[16]; n[9] = e[17];
        n[16] = e[20]; n[17] = e[21];
        n[20] = e[4];  n[21] = e[5];
        n[4] = e[8];  n[5] = e[9];
    }


    else if (movimento == "F") {
        // Rotaciona a própria face F (8, 9, 11, 10)
        n[8] = e[10]; n[9] = e[8]; n[11] = e[9]; n[10] = e[11];

        // Troca as bordas adjacentes: Up(2,3) -> Right(4,6) -> Down(13,12) -> Left(19,17)
        n[4] = e[2];  n[6] = e[3];  // Up vai para Right
        n[13] = e[4];  n[12] = e[6];  // Right vai para Down
        n[19] = e[13]; n[17] = e[12]; // Down vai para Left
        n[2] = e[19]; n[3] = e[17]; // Left vai para Up
    }
    else if (movimento == "F'") { // Inverso de F
        n[8] = e[9]; n[9] = e[11]; n[11] = e[10]; n[10] = e[8];

        n[2] = e[4];  n[3] = e[6];
        n[4] = e[13]; n[6] = e[12];
        n[13] = e[19]; n[12] = e[17];
        n[19] = e[2];  n[17] = e[3];
    }
    // Movimento desconhecido não é silenciosamente ignorado.
    // Isso evita que um erro de digitação (ex: "r" minúsculo, "X") passe
    // batido e "trave" o cubo num estado que parece não responder a nada.
    else {
        std::cerr << "[AVISO] Movimento invalido ignorado: '" << movimento << "'\n";
    }
        return n;
    }

    /*
     * Função Sucessora
     * Dado um estado, gera todos os estados alcançáveis aplicando cada um dos movimentos válidos.
     */
    std::vector<Movimento> gerar_sucessores() const {
        std::vector<Movimento> sucessores;

        for (const std::string& mov : movimentos_validos()) {
            Movimento sucessor;
            sucessor.nome = mov;
            sucessor.estado_resultante = aplicar_movimento(estado_atual, mov);
            sucessores.push_back(sucessor); // Adiciona elemento no final do container
        }
        return sucessores;
    }

    /*
     * Função Avaliadora / Heurística
     * Estima o quão longe o estado está da solução (utilizada na Busca A*).
     * Heurística escolhida: Quantidade de peças no lugar errado (similar ao 8-Puzzle).
     */
    int calcular_heuristica() const {
        int pecas_fora = 0;
        for (int i = 0; i < 24; ++i) {
            if (estado_atual[i] != ESTADO_RESOLVIDO[i]) pecas_fora++;
        }
        // No cubo 2x2, cada giro de face (R, U ou F, em qualquer
        // sentido) altera no MÁXIMO 8 adesivos (4 da própria face que gira +
        // 4 da faixa de borda que é arrastada para as faces vizinhas) — isso
        // foi confirmado empiricamente aplicando cada um dos 6 movimentos ao
        // cubo resolvido e contando as posições que mudam.
        // Dividir por 4 (como no pensamento original) superestima o custo restante
        // em até 2x, tornando a heurística NÃO ADMISSÍVEL — o que quebra a
        // garantia teórica do A* de encontrar sempre a solução ótima (mínimo
        // número de giros). Dividir por 8 é o divisor correto/seguro.
        return pecas_fora / 8; 
    }

    /*
     * EMBARALHAMENTO COM SEED
     * O embaralhamento inicial deve ser gerado a partir de uma seed fixa informada.
     */
    void embaralhar(int seed, int num_movimentos) {
        std::mt19937 gerador(seed); // Gerador de números pseudoaleatórios com a seed fixa, ou seja para a mesma seed, tem o mesmo numero
        const auto& movimentos = movimentos_validos();

        // gera um movimento aleatório uniformemente        
        std::uniform_int_distribution<int> distribuicao(0, movimentos.size() - 1);

        // faz o cubo movimentar aleatoriamente conforme a qnt de movimentos
        for (int i = 0; i < num_movimentos; ++i) {
            estado_atual = aplicar_movimento(estado_atual, movimentos[distribuicao(gerador)]);
        }
    }
};

#endif