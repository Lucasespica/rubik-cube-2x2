#include "Busca.hpp"

namespace {
    // Cada canto = 3 indices de adesivo
    // que pertencem ao mesmo cubo fisico
    using Canto = std::array<int, 3>;

    const std::array<Canto, 8> CANTOS = {{
        {{0, 4, 17}},
        {{1, 13, 16}},
        {{2, 5, 8}},
        {{3, 9, 12}},
        {{6, 19, 22}},
        {{7, 10, 20}},
        {{11, 14, 21}},
        {{15, 18, 23}}
    }};
}

// Fila (usada pelo bfs)
void Fila::inserir(const EstadoBusca& estado) {
    dados_.push_back(estado);
}

EstadoBusca Fila::remover(){
    //Tira do inicio, que e o mais antigo
    EstadoBusca proximo = dados_.front();
    dados_.pop_front();
    return proximo;
}

bool Fila::vazia() const {
    return dados_.empty();
}

// Pilha (usado pelo DFS/IDDFS)
void Pilha:: inserir(const EstadoBusca& estado) {
    dados_.push_back(estado);
}

EstadoBusca Pilha::remover() {
    // Tira do fim, que e o mais recente
    EstadoBusca proximo = dados_.back();
    dados_.pop_back();
    return proximo;
}

bool Pilha::vazia() const {
    return dados_.empty();
}

int heuristicaCantos(const Cubo& cubo) {
    // static: so cria o cubo resolvido uma vez, na primeira chamada,
    // e reaproveita nas seguintes
    static const Cubo RESOLVIDO;

    int fora = 0;
    for (const Canto& canto : CANTOS) {
        bool certo = true;
        for (int idx: canto) {
            int face = idx / Cubo::STICKERS_POR_FACE;
            int sticker = idx % Cubo::STICKERS_POR_FACE;
            if (cubo.corNaPosicao(face, sticker) != RESOLVIDO.corNaPosicao(face, sticker)) {
                certo = false;
                break;
            }
        }
        if (!certo) {
            ++fora;
        }
    }
    // Arredonda pra cima com divisao inteira
    // Ex: fora = 5 -> (5 + 3)/4 = 2
    return (fora + 3) / 4;
}

bool ComparaEstados::operator()(const EstadoBusca& a, const EstadoBusca& b) const {
    int fa = static_cast<int>(a.caminho.size()) + heuristicaCantos(a.cubo);
    int fb = static_cast<int>(b.caminho.size()) + heuristicaCantos(b.cubo);
    return fa > fb;
}

void FilaPrioridade::inserir(const EstadoBusca& estado) {
    dados_.push(estado);
}

EstadoBusca FilaPrioridade::remover() {
    EstadoBusca proximo = dados_.top();
    dados_.pop();
    return proximo;
}

bool FilaPrioridade::vazia() const {
    return dados_.empty();
}

std::string nomeMovimento(Movimento m) {
    switch(m) {
        case Movimento::U: return "U";
        case Movimento::U_LINHA: return "U'";
        case Movimento::D: return "D";
        case Movimento::D_LINHA: return "D'";
        case Movimento::L: return "L";
        case Movimento::L_LINHA: return "L'";
        case Movimento::R: return "R'";
        case Movimento::R_LINHA: return "R'";
        case Movimento::F: return "F";
        case Movimento::F_LINHA: return "F'";
        case Movimento::B: return "B";
        case Movimento::B_LINHA: return "B'";
    }
    return "?";     // nunca deve chegar aqui
}

ResultadoBusca buscar(const Cubo& inicial,
                      IEstruturaDeDados& estrutura,
                      int limiteProfundidade) {
    ResultadoBusca resultado;

    // Adiciona estado inicial na estrutura
    estrutura.inserir(EstadoBusca{inicial, {}});

    // Enquanto a estrutura nao estiver vazia
    while (!estrutura.vazia()) {
        // Remove o proximo estado
        EstadoBusca atual = estrutura.remover();
        ++resultado.visitados;

        // Avalia se o estado e final e encerra
        if (atual.cubo.estaResolvido()) {
            resultado.encontrou = true;
            resultado.caminho = atual.caminho;
            return resultado;
        }

        // Poda por profundidade (so importa para o IDDFS)
        if (limiteProfundidade >= 0 &&
            static_cast<int>(atual.caminho.size()) >= limiteProfundidade) {
            continue;   // nao expande, mas o laco continua com outros estados
        }

        // Adiciona sucessores na estrutura
        std::optional<Movimento> ultimo;
        if (!atual.caminho.empty()) {
            ultimo = atual.caminho.back();
        }
        for (const auto& par : atual.cubo.gerarSucessores(ultimo)) {
            EstadoBusca filho;
            filho.cubo = par.second;
            filho.caminho = atual.caminho;       // copia o caminho do pai
            filho.caminho.push_back(par.first);  // acrescenta o novo passo
            estrutura.inserir(filho);
        }
    }
    // Retorna 'sem solucao' (resultado.encontrou continua false)
    return resultado;
}

ResultadoBusca buscarBFS(const Cubo& inicial) {
    Fila fila;
    return buscar(inicial, fila, -1);
}

ResultadoBusca buscarAEstrela(const Cubo& inicial) {
    FilaPrioridade filaPrioridade;
    return buscar(inicial, filaPrioridade, -1);
}

ResultadoBusca buscarIDDFS(const Cubo& inicial, int limiteMaximo) {
    // laco externo do IDDFS: aumenta o limite aos poucos, reaproveitando
    // sempre o mesmo 'buscar'.
    long long totalVisitados = 0;
    for (int limite = 0; limite <= limiteMaximo; ++limite) {
        Pilha pilha;
        ResultadoBusca resultado = buscar(inicial, pilha, limite);
        totalVisitados += resultado.visitados;

        if (resultado.encontrou) {
            resultado.visitados = totalVisitados;   // soma de todas as tentativas
            return resultado;
        }
    }

    ResultadoBusca semSolucao;
    semSolucao.visitados = totalVisitados;
    return semSolucao;   // encontrou == false
}