#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura para representar um processo
typedef struct Processo {
    int id;
    int tamanho;
    int *tabela_paginas;
    struct Processo *prox;
} Processo;

// Estrutura para a tabela que armazenará os processos
typedef struct {
    Processo *cabeca;
} TabelaProcessos;

int tamanho_memoria_fisica;
int tamanho_pagina;
int tamanho_max_processo;
int *memoria_fisica;
int *quadros_livres;
TabelaProcessos tabela_processos;

// Função para verificar se um número é potência de 2
int is_power_of_two(int x) {
    return (x > 0) && ((x & (x - 1)) == 0);
}

void inicializar_memoria() {
    for (int i = 0; i < tamanho_memoria_fisica / tamanho_pagina; i++) {
        quadros_livres[i] = 1; // 1 indica quadro livre
    }
    tabela_processos.cabeca = NULL;
}

void visualizar_memoria() {
    int memoria_livre = 0;
    for (int i = 0; i < tamanho_memoria_fisica / tamanho_pagina; i++) {
        if (quadros_livres[i] == 1) {
            memoria_livre++;
        }
    }
    printf("Memória livre: %.2f%%\n", (memoria_livre / (float)(tamanho_memoria_fisica / tamanho_pagina)) * 100);
    for (int i = 0; i < tamanho_memoria_fisica; i++) {
        printf("%02X ", memoria_fisica[i]);
        if ((i + 1) % tamanho_pagina == 0) {
            printf("\n");
        }
    }
}

void criar_processo(int id, int tamanho) {
    if (tamanho > tamanho_max_processo) {
        printf("Erro: Tamanho do processo excede o tamanho máximo configurado.\n");
        return;
    }

    int paginas_necessarias = (tamanho + tamanho_pagina - 1) / tamanho_pagina;
    int paginas_alocadas = 0;

    Processo *novo_processo = (Processo *)malloc(sizeof(Processo));
    novo_processo->id = id;
    novo_processo->tamanho = tamanho;
    novo_processo->tabela_paginas = (int *)malloc(paginas_necessarias * sizeof(int));
    novo_processo->prox = NULL;

    while (paginas_alocadas < paginas_necessarias) {
        int quadro = rand() % (tamanho_memoria_fisica / tamanho_pagina);
        if (quadros_livres[quadro] == 1) {
            quadros_livres[quadro] = 0;
            novo_processo->tabela_paginas[paginas_alocadas++] = quadro;
            for (int j = 0; j < tamanho_pagina; j++) {
                memoria_fisica[quadro * tamanho_pagina + j] = rand() % 256;
            }
        }
    }

    if (paginas_alocadas < paginas_necessarias) {
        printf("Erro: Memória insuficiente para alocar o processo.\n");
        free(novo_processo->tabela_paginas);
        free(novo_processo);
        return;
    }

    // Adiciona o novo processo à tabela de processos
    novo_processo->prox = tabela_processos.cabeca;
    tabela_processos.cabeca = novo_processo;

    printf("Processo %d criado com sucesso.\n", id);
}

void visualizar_tabela_paginas(int id) {
    Processo *atual = tabela_processos.cabeca;
    while (atual != NULL) {
        if (atual->id == id) {
            printf("Tamanho do processo: %d bytes\n", atual->tamanho);
            printf("Tabela de páginas:\n");
            for (int i = 0; i < (atual->tamanho + tamanho_pagina - 1) / tamanho_pagina; i++) {
                printf("Página %d -> Quadro %d\n", i, atual->tabela_paginas[i]);
            }
            return;
        }
        atual = atual->prox;
    }
    printf("Erro: Processo não encontrado.\n");
}

int main() {
    srand(time(NULL));

    printf("Digite o tamanho da memória física (em bytes): ");
    scanf("%d", &tamanho_memoria_fisica);
    if (!is_power_of_two(tamanho_memoria_fisica)) {
        printf("Erro: O tamanho da memória física deve ser uma potência de 2.\n");
        return 1;
    }

    printf("Digite o tamanho da página (em bytes): ");
    scanf("%d", &tamanho_pagina);
    if (!is_power_of_two(tamanho_pagina)) {
        printf("Erro: O tamanho da página deve ser uma potência de 2.\n");
        return 1;
    }

    printf("Digite o tamanho máximo de um processo (em bytes): ");
    scanf("%d", &tamanho_max_processo);
    if (!is_power_of_two(tamanho_max_processo)) {
        printf("Erro: O tamanho máximo de um processo deve ser uma potência de 2.\n");
        return 1;
    }

    memoria_fisica = (int *)malloc(tamanho_memoria_fisica * sizeof(int));
    quadros_livres = (int *)malloc((tamanho_memoria_fisica / tamanho_pagina) * sizeof(int));
    inicializar_memoria();

    while (1) {
        int opcao;
        printf("\n1. Visualizar memória\n2. Criar processo\n3. Visualizar tabela de páginas\n4. Sair\nEscolha uma opção: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            visualizar_memoria();
        } else if (opcao == 2) {
            int id, tamanho;
            printf("Digite o ID do processo: ");
            scanf("%d", &id);
            printf("Digite o tamanho do processo (em bytes): ");
            scanf("%d", &tamanho);
            criar_processo(id, tamanho);
        } else if (opcao == 3) {
            int id;
            printf("Digite o ID do processo: ");
            scanf("%d", &id);
            visualizar_tabela_paginas(id);
        } else if (opcao == 4) {
            break;
        } else {
            printf("Opção inválida.\n");
        }
    }

    free(memoria_fisica);
    free(quadros_livres);
    Processo *atual = tabela_processos.cabeca;
    while (atual != NULL) {
        Processo *temp = atual;
        atual = atual->prox;
        free(temp->tabela_paginas);
        free(temp);
    }

    return 0;
}
