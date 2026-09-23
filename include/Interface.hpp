#pragma once

#include <optional>
#include <string>
#include "Busca.hpp"
#include "Cubo.hpp"

// Liga o suporte a cores ANSI no console do Windows. Sem isso,
// terminais mais antigos (cmd.exe classico) imprimem os codigos de
// cor como texto cru, tipo "\033[48;5;46m", em vez de pintar.
// Em outros sistemas (Linux/Mac) nao faz nada, pois ja vem ligado.
void habilitarCoresWindows();

// Imprime o cubo inteiro na tela, na planificacao em cruz, com cada
// adesivo mostrado como uma letra sobre um fundo da cor dele.
void imprimirCubo(const Cubo& cubo);

// Imprime, uma vez, a legenda letra -> cor, pro usuario se situar.
void imprimirLegenda();

// Converte o texto digitado pelo usuario (ex: "U", "u'") no Movimento
// correspondente. Devolve nullopt se nao reconhecer.
std::optional<Movimento> movimentoDoTexto(const std::string& texto);

// Le um numero inteiro do usuario, repetindo a pergunta se a
// entrada nao for um numero valido (evita travar o programa).
int lerInteiro(const std::string& pergunta);

// Mesma ideia, mas pra seed do embaralhamento (numero sem sinal,
// que e o que std::mt19937 espera).
unsigned lerSeed(const std::string& pergunta);

// Imprime o resultado de uma busca (visitados + movimentos legiveis).
void imprimirResultado(const std::string& nome, const ResultadoBusca& resultado);

// As duas opcoes do menu principal.
void jogar();
void iaJoga();

// Reroda os testes de regressao que fizemos ao longo do desenvolvimento.
void rodarTestes();