#pragma once

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