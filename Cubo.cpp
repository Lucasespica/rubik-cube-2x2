#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

// Estrutura auxiliar para mapear os movimentos gerados pela Função Sucessora
struct Movimento {
    std::string nome; // Ex: "R" (Right), "U'" (Up invertido)
    std::string estado_resultante;
};

class Cubo2x2 {
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
    const std::string ESTADO_RESOLVIDO = "UUUURRRRFFFFDDDDLLLLBBBB";
    // U = Up(Branco), D = Down(Amarelo), F = Front(Verde), B = Back(Azul), L = Left(Laranja), R = Right(Vermelho)


    // Função auxiliar para rotacionar uma face no sentido horário
    // (troca de posições no array de 24 posições).
    std::string aplicar_movimento(const std::string& e, const std::string& movimento) const {
        std::string n = e; // n = novo estado
        
        if (movimento == "R") {
        // Rotaciona a própria face R (4, 5, 7, 6 em sentido horário)
        n[4] = e[6]; n[5] = e[4]; n[7] = e[5]; n[6] = e[7];

        // Troca as bordas adjacentes: Up(1,3) -> Back(20,22) -> Down(13,15) -> Front(9,11)
        n[1]  = e[9];  n[3]  = e[11]; // Front vai para Up
        n[20] = e[1];  n[22] = e[3];  // Up vai para Back
        n[13] = e[20]; n[15] = e[22]; // Back vai para Down
        n[9]  = e[13]; n[11] = e[15]; // Down vai para Front
    }
    else if (movimento == "R'") { // Inverso de R
        n[4] = e[5]; n[5] = e[7]; n[7] = e[6]; n[6] = e[4];

        n[9]  = e[1];  n[11] = e[3];
        n[1]  = e[20]; n[3]  = e[22];
        n[20] = e[13]; n[22] = e[15];
        n[13] = e[9];  n[15] = e[11];
    }

    // -------------------------------------------------------------
    // movimento U (Giro na Face Superior / Up no sentido horário)
    // -------------------------------------------------------------
    else if (movimento == "U") {
        // Rotaciona a própria face U (0, 1, 3, 2)
        n[0] = e[2]; n[1] = e[0]; n[3] = e[1]; n[2] = e[3];

        // Troca as bordas adjacentes: Front(8,9) -> Left(16,17) -> Back(20,21) -> Right(4,5)
        n[16] = e[8];  n[17] = e[9];  // Front vai para Left
        n[20] = e[16]; n[21] = e[17]; // Left vai para Back
        n[4]  = e[20]; n[5]  = e[21]; // Back vai para Right
        n[8]  = e[4];  n[9]  = e[5];  // Right vai para Front
    }
    else if (movimento == "U'") { // Inverso de U
        n[0] = e[1]; n[1] = e[3]; n[3] = e[2]; n[2] = e[0];

        n[8]  = e[16]; n[9]  = e[17];
        n[16] = e[20]; n[17] = e[21];
        n[20] = e[4];  n[21] = e[5];
        n[4]  = e[8];  n[5]  = e[9];
    }

    // -------------------------------------------------------------
    // movimento F (Giro na Face Frontal / Front no sentido horário)
    // -------------------------------------------------------------
    else if (movimento == "F") {
        // Rotaciona a própria face F (8, 9, 11, 10)
        n[8] = e[10]; n[9] = e[8]; n[11] = e[9]; n[10] = e[11];

        // Troca as bordas adjacentes: Up(2,3) -> Right(4,6) -> Down(13,12) -> Left(19,17)
        n[4]  = e[2];  n[6]  = e[3];  // Up vai para Right
        n[13] = e[4];  n[12] = e[6];  // Right vai para Down
        n[19] = e[13]; n[17] = e[12]; // Down vai para Left
        n[2]  = e[19]; n[3]  = e[17]; // Left vai para Up
    }
    else if (movimento == "F'") { // Inverso de F
        n[8] = e[9]; n[9] = e[11]; n[11] = e[10]; n[10] = e[8];

        n[2]  = e[4];  n[3]  = e[6];
        n[4]  = e[13]; n[6]  = e[12];
        n[13] = e[19]; n[12] = e[17];
        n[19] = e[2];  n[17] = e[3];
    }

        return n;
    }

public:
    // Construtor inicializa o cubo no estado resolvido
    Cubo2x2() {
        estado_atual = ESTADO_RESOLVIDO;
    }

    Cubo2x2(std::string estado) {
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

    /*
     * Função Sucessora
     * Dado um estado, gera todos os estados alcançáveis aplicando cada um dos movimentos válidos.
     */
    std::vector<Movimento> gerar_sucessores() const {
        std::vector<Movimento> sucessores;
        
        // Lista de movimentos válidos (rotações de faces)
        std::vector<std::string> movimentos_validos = {"R", "R'", "U", "U'", "F", "F'"};

        for (const std::string& mov : movimentos_validos) {
            Movimento sucessor;
            sucessor.nome = mov;
            sucessor.estado_resultante = aplicar_movimento(estado_atual, mov);
            sucessores.push_back(sucessor); // Adiciona elkemento no final do conteiner
        }

        return sucessores;
    }

    /*
     * Função Avaliadora / Heurística
     * Estima o quão longe o estado está da solução (utilizada na Busca A*).
     * Heurística escolhida: Quantidade de peças no lugar errado (similar ao 8-Puzzle).
     */
    int calcular_heuristica() const {
        int peças_fora = 0;
        for (int i = 0; i < 24; ++i) {
            if (estado_atual[i] != ESTADO_RESOLVIDO[i]) {
                peças_fora++;
            }
        }
        // Como cada rotação move várias peças, a heurística baseada em adesivos individuais 
        // precisaria ser dividida por um fator para se manter "admissível" (não superestimar o custo real)
        // Ao pegar a quantidade de peças fora do lugar e dividir por 4 (o número máximo de peças alteradas por movimento), 
        // garantimos que o valor da estimativa seja sempre menor ou igual ao número real de giros necessários
        // pq por exemplo, se tivessemos uma rotação mudando 4 peças do lugar e as 4 se encaixarem pereitamente, o custo foi apenas 1 e não 4
        return peças_fora / 4; 
    }

    /*
     * EMBARALHAMENTO COM SEED
     * O embaralhamento inicial deve ser gerado a partir de uma seed fixa informada.
     */
    void embaralhar(int seed, int num_movimentos = 20) {
        std::mt19937 gerador(seed); // Gerador de números pseudoaleatórios com a seed fixa, ou seja para a mesma seed, tem o mesmo numero
        std::vector<std::string> movimentos = {"R", "R'", "U", "U'", "F", "F'"};
        
        // gera um movimento aleatório uniformemente
        std::uniform_int_distribution<int> distribuicao(0, movimentos.size() - 1);

        // faz o cubo movimentar aleatoriamente conforme a qnt de movimentos
        for (int i = 0; i < num_movimentos; ++i) {
            int index_movimento = distribuicao(gerador);
            estado_atual = aplicar_movimento(estado_atual, movimentos[index_movimento]);
        }
    }
};