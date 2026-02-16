#include "projeto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    linhas = 0;
    colunas = 0;
    char comando[50];
    char nome_arquivo[100];
    char coluna;
    int linha;

    mostrar_ajuda();

    while (1) {
        printf("> ");
        if (scanf("%s", comando) != 1) {
            printf("Erro ao ler comando.\n");
            continue;
        }

        if (strcmp(comando, "g") == 0) {
            scanf("%s", nome_arquivo);
            salvar_jogo(nome_arquivo);
        } else if (strcmp(comando, "l") == 0) {
            scanf("%s", nome_arquivo);
            carregar_jogo(nome_arquivo);
        } else if (strcmp(comando, "b") == 0) {
            if (scanf(" %c%d", &coluna, &linha) != 2) {
                printf("Erro: formato de coordenada inválido.\n");
                continue;
            }
            pintar_branco(coluna, linha);
        } else if (strcmp(comando, "r") == 0) {
            if (scanf(" %c%d", &coluna, &linha) != 2) {
                printf("Erro: formato de coordenada inválido.\n");
                continue;
            }
            riscar_casa(coluna, linha);
        } else if (strcmp(comando, "a") == 0) {
            ajuda_inferencias();
        } else if (strcmp(comando, "A") == 0) {
            ajuda_automatica();
        } else if (strcmp(comando, "v") == 0) {
            verificar_estado();
        } else if (strcmp(comando, "R") == 0) {
            resolver_jogo();
        } else if (strcmp(comando, "d") == 0) {
            desfazer_ultimo_comando();
        } else if (strcmp(comando, "n") == 0) {
            novo_jogo();
        } else if (strcmp(comando, "s") == 0) {
            printf("A sair do jogo.\n");
            break;
        } else if (strcmp(comando, "h") == 0 || strcmp(comando, "?") == 0) {
            mostrar_ajuda();
        } else {
            printf("Comando indefinido. Digite 'h' para ajuda.\n");
        }
    }

    return 0;
}