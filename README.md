# Simulador e Solucionador de Cubo Mágico 2x2x2

Projeto em C++ padrão (sem recursos avançados/idiomáticos) que simula um
cubo 2x2x2, permite jogar manualmente e resolve o cubo automaticamente
usando três algoritmos de busca: **BFS**, **IDDFS** e **A\***.

## Como compilar e rodar

```bash
g++ -std=c++17 -Wall -O2 -o cubo main.cpp cube_state.cpp search.cpp
./cubo
```

O programa primeiro pede uma **seed** (número inteiro) e uma quantidade de
movimentos de embaralhamento. A mesma seed + mesma quantidade sempre geram
o **mesmo cubo embaralhado**, permitindo comparar o desempenho dos três
algoritmos sobre exatamente o mesmo problema. Depois disso, um menu permite:

1. Jogar manualmente (digitar movimentos como `U`, `R'`, `F`, etc.);
2. Resolver com IA, escolhendo BFS, IDDFS ou A\*;
3. Reembaralhar com uma nova seed.

## Como o Estado foi representado

O cubo 2x2x2 só tem **8 peças de canto** (não existem arestas nem centros
como no cubo 3x3x3). Guardamos, para cada uma das 8 **posições** do cubo:

- `perm[posição]` → qual peça (0 a 7) está fisicamente ali;
- `ori[posição]` → como essa peça está torcida ali (0, 1 ou 2).

O objetivo (cubo resolvido) é simplesmente `perm[i] == i` e `ori[i] == 0`
para todo `i`. Essa representação é compacta (16 números inteiros de 0-7),
fácil de comparar (para detectar o objetivo e estados repetidos) e fácil de
converter em uma string curta para guardar em um `unordered_set` de
estados já visitados.

Veja os comentários de `REQUISITO CONCEITUAL 1` em `cube_state.h`.

## Como funciona a Função Sucessora

Existem 12 movimentos possíveis: as 6 faces (U, D, F, B, L, R), cada uma
podendo girar 90° no sentido horário ou anti-horário. Cada movimento é
implementado como um **ciclo de 4 posições** (as 4 peças daquela camada
trocam de lugar) mais uma **torção** aplicada às peças de F/B/L/R (as
faces U/D não torcem peças; isso é fisicamente correto: apenas o eixo de
giro perpendicular à orientação "natural" da peça altera sua torção).

Os movimentos anti-horários (`U'`, `R'`, etc.) são implementados apenas
aplicando o movimento horário correspondente **3 vezes seguidas** — já que
4 giros de 90° na mesma face sempre voltam ao estado original, girar 3
vezes é equivalente a girar 1 vez no sentido contrário. Isso evita ter que
programar (e testar) 12 tabelas de movimento diferentes: só precisamos de
6, e a corretude do restante decorre matematicamente disso (validado nos
testes automatizados, veja a seção "Testes" abaixo).

A função sucessora (`gerarSucessores`) aplica cada um dos 12 movimentos ao
estado atual e devolve a lista de estados alcançáveis. Uma pequena
otimização: ela nunca gera o movimento que desfaz imediatamente o último
movimento aplicado (ex.: depois de `U` não gera `U'`), pois isso só levaria
de volta ao estado anterior.

Veja `REQUISITO CONCEITUAL 2` em `cube_state.h` / `cube_state.cpp`.

## Qual heurística foi usada no A\* e por quê

A heurística conta quantas peças estão **fora do lugar** (posição errada
OU torção errada) e divide o resultado por 4, arredondando para cima:

```
h(estado) = ceil(peças_fora_do_lugar / 4)
```

O motivo da divisão por 4: cada movimento do cubo mexe em, no máximo, 4
peças de canto simultaneamente. Logo, nenhuma sequência de movimentos pode
reduzir o número de peças fora do lugar mais rápido que 4 por movimento —
ou seja, essa heurística **nunca superestima** a distância real até a
solução. Isso a torna **admissível**, condição necessária para o A\*
garantir a solução ótima (menor número de movimentos). É a mesma ideia
usada na heurística clássica de "peças fora do lugar" do problema do
8-puzzle, adaptada para o fato de que aqui um movimento afeta 4 peças em
vez de 1.

Veja `REQUISITO CONCEITUAL 3` em `cube_state.h` / `cube_state.cpp`.

## O laço de busca genérico

O núcleo dos três algoritmos é a função `executarBusca` (`search.cpp`),
que implementa **exatamente** o pseudocódigo pedido:

```
Adicionar estado inicial na estrutura
Enquanto a estrutura não estiver vazia:
    Remover próximo estado da estrutura
    Avaliar estado
    SE estado é final: mostrar solução e encerrar
    Adicionar estados seguintes (sucessores) na estrutura
Retornar "Sem solução"
```

Os três algoritmos usam **o mesmo laço**; a única coisa que muda é qual
implementação de `EstruturaDeBusca` (interface com `inserir`, `remover`,
`vazia`) é passada para dentro dele:

| Algoritmo | Estrutura de dados      | Ordem de remoção                         |
|-----------|--------------------------|-------------------------------------------|
| BFS       | `Fila` (FIFO)             | o estado inserido há mais tempo            |
| IDDFS     | `Pilha` (LIFO)            | o estado inserido mais recentemente        |
| A\*       | `FilaDePrioridade` (heap) | o estado com menor `f = g + h`             |

O IDDFS é obtido chamando o mesmo `executarBusca` repetidamente, uma vez
para cada limite de profundidade crescente (0, 1, 2, ...), até achar uma
solução ou estourar o limite máximo.

**Detalhe importante de corretude:** BFS e A\* usam um conjunto de
"estados já visitados" para não reprocessar o mesmo estado duas vezes —
isso é seguro para eles porque, na ordem em que exploram (por camadas /
por menor custo), a primeira vez que um estado sai da estrutura já é pelo
caminho ótimo até ele. Já o IDDFS **não** usa esse conjunto global: como
ele explora em pilha (LIFO), um estado poderia ser alcançado primeiro por
um ramo mais longo (marcando-o como "visitado") antes de ser alcançado por
um ramo mais curto que levaria à solução dentro do limite de profundidade
— isso causaria respostas erradas de "sem solução". Por isso o IDDFS
confia apenas no limite de profundidade para cortar a busca. Esse
comportamento foi conferido nos testes automatizados.

## Testes realizados

Um programa de teste (`teste_automatico.cpp`, não faz parte da entrega
final, apenas usado durante o desenvolvimento) verificou:

- Para várias seeds, BFS, IDDFS e A\* encontram soluções que, quando
  aplicadas ao cubo embaralhado, realmente levam ao estado objetivo.
- Os três algoritmos encontram o **mesmo número mínimo de movimentos**
  (todos são "ótimos" em número de passos, como esperado).
- O A\* visita **dezenas a centenas de vezes menos estados** que BFS e
  IDDFS para o mesmo problema — evidência de que a heurística está
  guiando a busca de forma eficaz.
- Aplicar um movimento seguido do seu inverso sempre devolve o estado
  original (12 movimentos testados).
- Aplicar a mesma face 4 vezes seguidas sempre devolve o estado original
  (6 faces testadas) — confirma que a tabela de torções está matematicamente
  consistente.

## Limitações conhecidas / observações

- BFS e IDDFS têm fator de ramificação ~11 (12 movimentos menos o que
  desfaz o anterior), então o número de estados cresce muito rápido com a
  profundidade. Por isso o menu usa um limite de profundidade padrão de 8
  movimentos para essas duas buscas — cubos embaralhados com muito mais
  de 5-6 movimentos podem demorar bastante ou não resolver dentro do
  limite. O A\*, graças à heurística, lida bem com embaralhamentos maiores.
- A "planificação" do cubo desenhada no terminal (função `imprimirCubo`)
  usa uma convenção própria e simplificada de disposição das faces — o
  importante para os fins deste projeto é a consistência interna (o
  mesmo estado sempre é desenhado da mesma forma, e giros refletem
  corretamente a mudança de cor), e não a conformidade estrita com a
  notação oficial de competição de cubo mágico.

## Arquivos

- `cube_state.h` / `cube_state.cpp` — Estado, movimentos, sucessora,
  heurística, embaralhamento com seed e desenho do cubo.
- `search.h` / `search.cpp` — Estrutura de dados de busca (Fila, Pilha,
  Fila de Prioridade), laço genérico de busca e as 3 funções
  `resolverComBFS` / `resolverComIDDFS` / `resolverComAStar`.
- `main.cpp` — Menu, modo manual e modo de resolução com IA.
