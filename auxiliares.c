#include "projeto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char tabuleiro[MAX][MAX];
int linhas, colunas;
EstadoTabuleiro historico[MAX_HISTORICO];
int indice_historico = 0;

void mostrar_tabuleiro() {
    if (linhas == 0 || colunas == 0) {
        printf("Tabuleiro vazio\n");
        return;
    }
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            printf("%c ", tabuleiro[i][j]);
        }
        printf("\n");
    }
}

void inicializar_historico() {
    indice_historico = 0;
}

void novo_jogo() {
    inicializar_historico();
    linhas = colunas = 5;
    char exemplo[5][6] = { "ecadc", "dcdec", "bddce", "cdeeb", "accbb" };
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            tabuleiro[i][j] = exemplo[i][j];
        }
    }
    printf("Novo jogo 5x5 criado automaticamente!\n");
    mostrar_tabuleiro();
}

void salvar_jogo(char *nome_arquivo) {
    if (linhas == 0 || colunas == 0) {
        printf("Erro: Não há jogo para salvar.\n");
        return;
    }

    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo para salvar.\n");
        return;
    }

    fprintf(arquivo, "%d %d\n", linhas, colunas);
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            fprintf(arquivo, "%c", tabuleiro[i][j]);
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
    printf("Jogo salvo com sucesso em %s\n", nome_arquivo);
}

void carregar_jogo(char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo para carregar.\n");
        return;
    }

    if (fscanf(arquivo, "%d %d\n", &linhas, &colunas) != 2 || linhas <= 0 || linhas > MAX || colunas <= 0 || colunas > MAX) {
        printf("Erro: formato de arquivo inválido.\n");
        fclose(arquivo);
        return;
    }

    inicializar_historico();
    char linha[MAX + 1];
    for (int i = 0; i < linhas; i++) {
        if (!fgets(linha, MAX + 1, arquivo)) {
            printf("Erro ao ler o tabuleiro.\n");
            fclose(arquivo);
            return;
        }
        for (int j = 0; j < colunas; j++) {
            tabuleiro[i][j] = linha[j];
        }
    }

    fclose(arquivo);
    printf("Jogo carregado com sucesso de %s\n", nome_arquivo);
    mostrar_tabuleiro();
}

void salvar_estado_anterior() {
    if (indice_historico >= MAX_HISTORICO) {
        for (int i = 0; i < MAX_HISTORICO - 1; i++) {
            historico[i] = historico[i + 1];
        }
        indice_historico = MAX_HISTORICO - 1;
    }

    historico[indice_historico].linhas = linhas;
    historico[indice_historico].colunas = colunas;
    memcpy(historico[indice_historico].tabuleiro, tabuleiro, sizeof(tabuleiro));
    indice_historico++; // memcpy serve para copiar, neste caso, os dados atualizados do tabuleiro para o tabuleiro original
}

void pintar_branco(char coluna, int linha) {
    int col = coluna - 'a';
    int lin = linha - 1;

    if (!testar_coordenadas(coluna, linha)) {
        printf("Coordenadas inválidas!\n");
        return;
    }

    if (tabuleiro[lin][col] == '#') {
        printf("Erro: Não é possível pintar uma casa riscada.\n");
        return;
    }

    salvar_estado_anterior();
    tabuleiro[lin][col] = toupper(tabuleiro[lin][col]);
    printf("Casa %c%d pintada de branco.\n", coluna, linha);
    mostrar_tabuleiro();
}

void riscar_casa(char coluna, int linha) {
    int col = coluna - 'a';
    int lin = linha - 1;

    if (!testar_coordenadas(coluna, linha)) {
        printf("Coordenadas inválidas!\n");
        return;
    }

    if (isupper(tabuleiro[lin][col])) {
        printf("Erro: Não é possível riscar uma casa branca.\n");
        return;
    }

    salvar_estado_anterior();
    tabuleiro[lin][col] = '#';
    printf("Casa %c%d riscada.\n", coluna, linha);
    mostrar_tabuleiro();
}

void desfazer_ultimo_comando() {
    if (indice_historico <= 0) {
        printf("Não há ação anterior para desfazer.\n");
        return;
    }

    indice_historico--;
    linhas = historico[indice_historico].linhas;
    colunas = historico[indice_historico].colunas;
    memcpy(tabuleiro, historico[indice_historico].tabuleiro, sizeof(tabuleiro));
    printf("Ação desfeita. Você pode desfazer mais %d ações.\n", indice_historico);
    mostrar_tabuleiro();
}


static int pode_riscar_casa(int linha, int coluna) {
    int dir_i[] = {-1, 0, 1, 0}, dir_j[] = {0, 1, 0, -1};
    
    // Verificar se há casas riscadas adjacentes
    for (int d = 0; d < 4; d++) {
        int ni = linha + dir_i[d], nj = coluna + dir_j[d];
        if (ni >= 0 && ni < linhas && nj >= 0 && nj < colunas) {
            if (tabuleiro[ni][nj] == '#') {
                return 0; // Não pode riscar porque há casa riscada adjacente
            }
        }
    }
    return 1;
}

//
static void aplicar_inferencias_sem_salvar() { // usa se static void para não haver erros de compilação
    int dir_i[] = {-1, 0, 1, 0}, dir_j[] = {0, 1, 0, -1};
    
    // Riscar letras iguais a letras brancas
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (isupper(tabuleiro[i][j])) {
                char letra = tolower(tabuleiro[i][j]);
                for (int k = 0; k < colunas; k++)
                    if (k != j && tabuleiro[i][k] == letra && pode_riscar_casa(i, k)) 
                        tabuleiro[i][k] = '#';
                for (int k = 0; k < linhas; k++)
                    if (k != i && tabuleiro[k][j] == letra && pode_riscar_casa(k, j)) 
                        tabuleiro[k][j] = '#';
            }
        }
    }
    
    // Pintar vizinhos de casas riscadas
    int mudancas;
    do {
        mudancas = 0;
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                if (tabuleiro[i][j] == '#') {
                    for (int d = 0; d < 4; d++) {
                        int ni = i + dir_i[d], nj = j + dir_j[d];
                        if (ni >= 0 && ni < linhas && nj >= 0 && nj < colunas && 
                            tabuleiro[ni][nj] != '#' && !isupper(tabuleiro[ni][nj])) {
                            tabuleiro[ni][nj] = toupper(tabuleiro[ni][nj]);
                            mudancas = 1;
                        }
                    }
                }
            }
        }
    } while (mudancas);
}

void ajuda_inferencias() {
    salvar_estado_anterior();
    aplicar_inferencias_sem_salvar();
    printf("Inferências aplicadas!\n");
    mostrar_tabuleiro();
}

// FUNÇÃO MODIFICADA para incluir verificação de casas riscadas adjacentes:
int verificar_estado() {
    int violacoes = 0, dir_i[] = {-1, 0, 1, 0}, dir_j[] = {0, 1, 0, -1};
    
    // Verificar letras duplicadas
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (isupper(tabuleiro[i][j])) {
                char letra = tolower(tabuleiro[i][j]);
                for (int k = j + 1; k < colunas; k++)
                    if (tolower(tabuleiro[i][k]) == letra && isupper(tabuleiro[i][k])) {
                        printf("Violação: Múltiplas '%c' na linha %d.\n", letra, i + 1); violacoes++; break;
                    }
                for (int k = i + 1; k < linhas; k++)
                    if (tolower(tabuleiro[k][j]) == letra && isupper(tabuleiro[k][j])) {
                        printf("Violação: Múltiplas '%c' na coluna %c.\n", letra, 'a' + j); violacoes++; break;
                    }
            }
        }
    }
    
    // Verificar vizinhos de casas riscadas E casas riscadas adjacentes
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (tabuleiro[i][j] == '#') {
                for (int d = 0; d < 4; d++) {
                    int ni = i + dir_i[d], nj = j + dir_j[d];
                    if (ni >= 0 && ni < linhas && nj >= 0 && nj < colunas) {
                        // NOVA VERIFICAÇÃO: Casas riscadas não podem ser adjacentes
                        if (tabuleiro[ni][nj] == '#') {
                            printf("Violação: Casas riscadas adjacentes em (%c%d) e (%c%d).\n", 
                                   'a'+j, i+1, 'a'+nj, ni+1);
                            violacoes++;
                        }
                        // Verificação original: vizinhos devem estar brancos
                        else if (!isupper(tabuleiro[ni][nj]) && tabuleiro[ni][nj] != '#') {
                            printf("Violação: Casa riscada (%c%d) tem vizinho não-branco.\n", 'a'+j, i+1);
                            violacoes++;
                        }
                    }
                }
            }
        }
    }
    
    // Verificar caminho ortogonal (BFS)
    int primeira_linha = -1, primeira_coluna = -1, total_brancas = 0;
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (isupper(tabuleiro[i][j])) {
                total_brancas++;
                if (primeira_linha == -1) { primeira_linha = i; primeira_coluna = j; }
            }
        }
    }
    
    if (total_brancas == 0) {
        printf("Verificação: Não há casas brancas.\n");
        return violacoes == 0;
    }
    
    int visitado[MAX][MAX] = {0};              // Cria uma matriz 2D chamada visitado com dimensões MAX x MAX (supondo que MAX é uma constante definida antes).
                                              // Inicializa todos os elementos com zero.
                                             // A matriz vai marcar quais posições do tabuleiro já foram visitadas durante uma busca (para evitar visitar a mesma posição várias vezes).
    typedef struct { int i, j; } Pos;
    Pos fila[MAX*MAX];
    int inicio = 0, fim = 0, brancas_encontradas = 1;
    
    fila[fim++] = (Pos){primeira_linha, primeira_coluna};
    visitado[primeira_linha][primeira_coluna] = 1;
    
    while (inicio < fim) {
        Pos atual = fila[inicio++];
        for (int d = 0; d < 4; d++) {
            int ni = atual.i + dir_i[d], nj = atual.j + dir_j[d];
            if (ni >= 0 && ni < linhas && nj >= 0 && nj < colunas &&
                isupper(tabuleiro[ni][nj]) && !visitado[ni][nj]) {
                fila[fim++] = (Pos){ni, nj};
                visitado[ni][nj] = 1;
                brancas_encontradas++;
            }
        }
    }
    
    if (brancas_encontradas < total_brancas) {
        printf("Violação: Casas brancas desconectadas.\n");
        for (int i = 0; i < linhas; i++)
            for (int j = 0; j < colunas; j++)
                if (isupper(tabuleiro[i][j]) && !visitado[i][j])
                    printf("  - Casa isolada em (%c%d)\n", 'a'+j, i+1);
        violacoes++;
    }
    
    if (violacoes == 0) { printf("Verificação: Tabuleiro válido!\n"); return 1; }
    else { printf("Verificação: %d violações detectadas.\n", violacoes); return 0; }
}

// Função auxiliar para verificar se pode pintar uma letra numa posição
static int pode_pintar_letra(int linha, int coluna, char letra) {
    // Verificar se já existe essa letra maiúscula na linha
    for (int j = 0; j < colunas; j++) {
        if (j != coluna && isupper(tabuleiro[linha][j]) && tolower(tabuleiro[linha][j]) == letra) {
            return 0;
        }
    }
    
    // Verificar se já existe essa letra maiúscula na coluna
    for (int i = 0; i < linhas; i++) {
        if (i != linha && isupper(tabuleiro[i][coluna]) && tolower(tabuleiro[i][coluna]) == letra) {
            return 0;
        }
    }
    
    return 1;
}

// FUNÇÃO A CORRIGIDA - Ajuda automática
void ajuda_automatica() {
    salvar_estado_anterior();
    
    int mudancas;
    do {
        mudancas = 0;
        
        // 1. Riscar letras iguais às brancas na mesma linha/coluna
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                if (isupper(tabuleiro[i][j])) {
                    char letra = tolower(tabuleiro[i][j]);
                    
                    // Mesma linha
                    for (int k = 0; k < colunas; k++) {
                        if (k != j && tabuleiro[i][k] == letra) {
                            tabuleiro[i][k] = '#';
                            mudancas = 1;
                        }
                    }
                    
                    // Mesma coluna
                    for (int k = 0; k < linhas; k++) {
                        if (k != i && tabuleiro[k][j] == letra) {
                            tabuleiro[k][j] = '#';
                            mudancas = 1;
                        }
                    }
                }
            }
        }
        
        // 2. Pintar vizinhos de casas riscadas (COM VERIFICAÇÃO)
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                if (tabuleiro[i][j] == '#') {
                    int dir_i[] = {-1, 0, 1, 0}, dir_j[] = {0, 1, 0, -1};
                    for (int d = 0; d < 4; d++) {
                        int ni = i + dir_i[d], nj = j + dir_j[d];
                        if (ni >= 0 && ni < linhas && nj >= 0 && nj < colunas && 
                            !isupper(tabuleiro[ni][nj]) && tabuleiro[ni][nj] != '#') {
                            char letra = tabuleiro[ni][nj];
                            if (pode_pintar_letra(ni, nj, letra)) {
                                tabuleiro[ni][nj] = toupper(letra);
                                mudancas = 1;
                            }
                        }
                    }
                }
            }
        }
        
        // 3. Pintar última letra restante numa linha/coluna (COM VERIFICAÇÃO)
        for (int i = 0; i < linhas; i++) {
            for (char letra = 'a'; letra <= 'z'; letra++) {
                int count = 0, pos = -1;
                for (int j = 0; j < colunas; j++) {
                    if (tolower(tabuleiro[i][j]) == letra && !isupper(tabuleiro[i][j]) && tabuleiro[i][j] != '#') {
                        count++;
                        pos = j;
                    }
                }
                if (count == 1 && pode_pintar_letra(i, pos, letra)) {
                    tabuleiro[i][pos] = toupper(letra);
                    mudancas = 1;
                }
            }
        }
        
        for (int j = 0; j < colunas; j++) {
            for (char letra = 'a'; letra <= 'z'; letra++) {
                int count = 0, pos = -1;
                for (int i = 0; i < linhas; i++) {
                    if (tolower(tabuleiro[i][j]) == letra && !isupper(tabuleiro[i][j]) && tabuleiro[i][j] != '#') {
                        count++;
                        pos = i;
                    }
                }
                if (count == 1 && pode_pintar_letra(pos, j, letra)) {
                    tabuleiro[pos][j] = toupper(letra);
                    mudancas = 1;
                }
            }
        }
        
        // 4. Riscar letras que não podem ser pintadas
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                if (!isupper(tabuleiro[i][j]) && tabuleiro[i][j] != '#') {
                    char letra = tabuleiro[i][j];
                    if (!pode_pintar_letra(i, j, letra)) {
                        tabuleiro[i][j] = '#';
                        mudancas = 1;
                    }
                }
            }
        }
        
    } while (mudancas);
    
    printf("Inferência automática concluída.\n");
    mostrar_tabuleiro();
}

// Resolver jogo
void resolver_jogo() {
    printf("Iniciando resolução automática do puzzle...\n");
    salvar_estado_anterior();
    
    int iteracao = 0;
    int mudancas_globais;
    
    do {
        mudancas_globais = 0;
        iteracao++;
        
        // Aplicar regras básicas repetidamente
        int mudancas_locais;
        do {
            mudancas_locais = 0;
            
            // Regra 1: Riscar letras iguais às brancas
            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) {
                    if (isupper(tabuleiro[i][j])) {
                        char letra = tolower(tabuleiro[i][j]);
                        
                        for (int k = 0; k < colunas; k++) {
                            if (k != j && tabuleiro[i][k] == letra) {
                                tabuleiro[i][k] = '#';
                                mudancas_locais = 1;
                            }
                        }
                        
                        for (int k = 0; k < linhas; k++) {
                            if (k != i && tabuleiro[k][j] == letra) {
                                tabuleiro[k][j] = '#';
                                mudancas_locais = 1;
                            }
                        }
                    }
                }
            }
            
            // Regra 2: Pintar vizinhos de casas riscadas
            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) {
                    if (tabuleiro[i][j] == '#') {
                        int dir_i[] = {-1, 0, 1, 0}, dir_j[] = {0, 1, 0, -1};
                        for (int d = 0; d < 4; d++) {
                            int ni = i + dir_i[d], nj = j + dir_j[d];
                            if (ni >= 0 && ni < linhas && nj >= 0 && nj < colunas && 
                                !isupper(tabuleiro[ni][nj]) && tabuleiro[ni][nj] != '#') {
                                char letra = tabuleiro[ni][nj];
                                if (pode_pintar_letra(ni, nj, letra)) {
                                    tabuleiro[ni][nj] = toupper(letra);
                                    mudancas_locais = 1;
                                }
                            }
                        }
                    }
                }
            }
            
            // Regra 4: Riscar letras que não podem ser pintadas
            for (int i = 0; i < linhas; i++) {
                for (int j = 0; j < colunas; j++) {
                    if (!isupper(tabuleiro[i][j]) && tabuleiro[i][j] != '#') {
                        char letra = tabuleiro[i][j];
                        if (!pode_pintar_letra(i, j, letra)) {
                            tabuleiro[i][j] = '#';
                            mudancas_locais = 1;
                        }
                    }
                }
            }
            
            mudancas_globais |= mudancas_locais;
            
        } while (mudancas_locais);
        
        // Regra 3: Pintar última ocorrência de uma letra numa linha/coluna (COM VERIFICAÇÃO)
        for (int i = 0; i < linhas; i++) {
            for (char letra = 'a'; letra <= 'z'; letra++) {
                int count = 0, pos = -1;
                for (int j = 0; j < colunas; j++) {
                    if (tolower(tabuleiro[i][j]) == letra && !isupper(tabuleiro[i][j]) && tabuleiro[i][j] != '#') {
                        count++;
                        pos = j;
                    }
                }
                if (count == 1 && pode_pintar_letra(i, pos, letra)) {
                    tabuleiro[i][pos] = toupper(letra);
                    mudancas_globais = 1;
                }
            }
        }
        
        for (int j = 0; j < colunas; j++) {
            for (char letra = 'a'; letra <= 'z'; letra++) {
                int count = 0, pos = -1;
                for (int i = 0; i < linhas; i++) {
                    if (tolower(tabuleiro[i][j]) == letra && !isupper(tabuleiro[i][j]) && tabuleiro[i][j] != '#') {
                        count++;
                        pos = i;
                    }
                }
                if (count == 1 && pode_pintar_letra(pos, j, letra)) {
                    tabuleiro[pos][j] = toupper(letra);
                    mudancas_globais = 1;
                }
            }
        }
        
        printf("Iteração %d:\n", iteracao);
        mostrar_tabuleiro();
        
    } while (mudancas_globais && iteracao < 20);
    
    if (verificar_estado()) {
        printf("\n Puzzle resolvido com sucesso!\n");
    } else {
        printf("\nResolução não conseguiu completar o puzzle.\n");
        printf("Use 'v' para ver as violações restantes.\n");
    }
}

int testar_arquivo(char *nome_arquivo, char modo) {
    FILE *arquivo = fopen(nome_arquivo, modo == 'r' ? "r" : "w");
    if (!arquivo) return 0;
    fclose(arquivo);
    return 1;
}

int testar_coordenadas(char coluna, int linha) {
    int col = coluna - 'a', lin = linha - 1;
    return (lin >= 0 && lin < linhas && col >= 0 && col < colunas);
}

void mostrar_ajuda() {
    printf("\nComandos:\n");
    printf("  l <ficheiro>    - Ler jogo\n");
    printf("  g <ficheiro>    - Gravar jogo\n");
    printf("  b <col><linha>  - Pintar de branco (ex: b a1)\n");
    printf("  r <col><linha>  - Riscar casa (ex: r a1)\n");
    printf("  d               - Desfazer última ação\n");
    printf("  a               - Ajuda por inferência\n");
    printf("  A               - Ajuda automática\n");
    printf("  v               - Verificar estado\n");
    printf("  R               - Resolver automaticamente\n");
    printf("  n               - Novo jogo\n");
    printf("  h ou ?          - Mostrar ajuda\n");
    printf("  s               - Sair\n\n");
}

