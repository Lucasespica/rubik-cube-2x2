# Cubo Mágico 2x2 — Simulador com IA

Trabalho de PI/ML - simulador de cubo mágico 2x2x2 em C++17, em que o jogador pode jogar manualmente ou pedir pra uma IA resolver o cubo, usando três estratégias de busca: **Busca em Largura (BFS)**, **Busca em Profundidade Limitada Iterativa (IDDFS)** e **A\***.

## Como compilar e rodar

Pré-requisitos: CMake >= 3.15 e um compilador C++17 (testado com o gerador do Visual Studio no Windows).

```bash
cmake -S . -B build
cmake --build build --config Debug
```

Rodar (Windows, build multi-config do Visual Studio):

```bash
build/Debug/cubo.exe
```

Sempre que um `.cpp` novo for adicionado ao projeto, é preciso rodar o `cmake -S . -B build` de novo (o `CMakeLists.txt` lista os fontes manualmente, sem busca automática). Para uma mudança só em código de um arquivo já existente, basta o `cmake --build`.

## Como usar

Ao abrir, o programa mostra um menu:

```
1 - Jogar
2 - IA Joga
3 - Rodar testes (regressao)
4 - Sair
```

- **Jogar**: você informa uma seed e a quantidade de movimentos pra embaralhar o cubo (a mesma seed sempre gera o mesmo embaralhamento). Depois, digita os movimentos pra tentar resolver: `U`, `D`, `L`, `R`, `F`, `B` (giro horário) ou com aspa pro giro anti-horário (`U'`, `D'`, ...). Digite `sair` pra voltar ao menu.
- **IA Joga**: mesma ideia de seed + quantidade de movimentos, mas você escolhe qual algoritmo resolve o cubo (BFS, IDDFS, A\* ou os três, pra comparar). O programa mostra quantos estados foram visitados e a sequência de movimentos da solução, com a opção de revisar passo a passo (pressionando Enter a cada movimento).
- **Rodar testes**: reexecuta os testes de regressão usados durante o desenvolvimento (movimentos, buscas, heurística, etc.), úteis pra conferir que nada quebrou.

A cruz do cubo é mostrada em texto, com cada adesivo como uma letra sobre um fundo colorido (a legenda letra → cor aparece ao abrir o programa):

```
B = Branco   A = Amarelo   V = Verde
Z = Azul     L = Laranja   R = Vermelho
```

## Estrutura do projeto

```
include/
  Cubo.hpp       # Estado do cubo (Cubo), movimentos (Movimento) e cores (Cor)
  Busca.hpp       # No de busca, estrutura de dados, o laco unico e as 3 buscas
  Interface.hpp    # Menu, exibicao do cubo, modo Jogar e modo IA Joga
src/
  Cubo.cpp        # Implementacao dos 12 movimentos, construtor, embaralhamento
  Busca.cpp        # Fila / Pilha / FilaPrioridade, heuristica do A*, laco de busca
  Interface.cpp    # Cores no terminal, impressao da cruz, modos de jogo, menu
  main.cpp         # Ponto de entrada: menu principal + testes de regressao
CMakeLists.txt      # Configuracao do build (lista os .cpp manualmente)
```

## Como o problema foi modelado

- **Estado**: `Cubo` (`include/Cubo.hpp`) - um `std::array` de 24 cores (6 faces × 4 adesivos). Imutável, todo movimento devolve um cubo novo.
- **Função sucessora**: `Cubo::gerarSucessores` - devolve, a partir de um estado, os até 12 estados alcançáveis com 1 movimento (11 se um movimento anterior for informado, pra não desfazer o passo anterior).
- **Função avaliadora**: `Cubo::estaResolvido` - verdadeiro se as 6 faces tiverem os 4 adesivos da mesma cor (aceita o cubo resolvido em qualquer orientação, já que todas as 6 faces podem girar).
- **Heurística do A\***: `heuristicaCantos` (`src/Busca.cpp`) - conta quantos dos 8 cantos físicos do cubo não estão no lugar certo, dividido por 4 (um giro move exatamente 4 cantos de uma vez).
- **Laço único de busca**: `buscar` (`src/Busca.cpp`) - implementado uma única vez, reaproveitado por BFS, IDDFS e A\* trocando apenas a estrutura de dados por trás da interface comum `IEstruturaDeDados` (fila, pilha ou fila de prioridade). Sem recursão. O IDDFS chama essa mesma função repetidamente, aumentando o limite de profundidade (0, 1, 2, ...).
