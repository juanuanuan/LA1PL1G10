#ifndef PROJETO_H
#define PROJETO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 50
#define MAX_HISTORICO 100  // Máximo de ações que podem ser desfeitas

// Estrutura para armazenar um estado do tabuleiro
typedef struct {
    char tabuleiro[MAX][MAX];
    int linhas, colunas;
} EstadoTabuleiro;

// Definições de estruturas de dados
extern char tabuleiro[MAX][MAX];
extern int linhas, colunas;

// Protótipos das funções
void mostrar_tabuleiro();
void salvar_jogo(char *nome_arquivo);
void carregar_jogo(char *nome_arquivo);
void pintar_branco(char coluna, int linha);
void riscar_casa(char coluna, int linha);
void desfazer_ultimo_comando();
void ajuda_inferencias();
int verificar_estado();
void ajuda_automatica(void);
void resolver_jogo();
void mostrar_ajuda();
void novo_jogo();
void inicializar_historico();

// Funções para testes e validações
int testar_arquivo(char *nome_arquivo, char modo);
int testar_coordenadas(char coluna, int linha);
void salvar_estado_anterior();
int aplicar_inferencias_basicas();

// Funções para a etapa 3 - Verificar caminho ortogonal
int verificar_caminho_ortogonal();

// Funções para a etapa 4 - Resolver o jogo
int resolver_jogo_automatico();

#endif /* PROJETO_H */