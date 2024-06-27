#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura para representar um processo
typedef struct Processo
{
    int id;
    int tamanho;
    int *tabela_paginas;
    struct Processo *prox;
} Processo;

// Estrutura para a tabela que armazenará os processos
typedef struct
{
    Processo *cabeca;
} TabelaProcessos;

int tamanho_memoria_fisica;
int tamanho_pagina;
int tamanho_max_processo;
int *memoria_fisica;
int *quadros_livres;
TabelaProcessos tabela_processos;

// Função para verificar se um número é potência de 2
int multiplo_de_dois(int x)
{
    return (x > 0) && ((x & (x - 1)) == 0);
}

void inicializar_memoria()
{
    for (int i = 0; i < tamanho_memoria_fisica / tamanho_pagina; i++)
    {
        quadros_livres[i] = 1; // 1 indica quadro livre
    }
    tabela_processos.cabeca = NULL;
}

void visualizar_memoria()
{
    int memoria_livre = 0;
    for (int i = 0; i < tamanho_memoria_fisica / tamanho_pagina; i++)
    {
        if (quadros_livres[i] == 1)
        {
            memoria_livre++;
        }
    }
    printf("Memória livre: %.2f%%\n", (memoria_livre / (float)(tamanho_memoria_fisica / tamanho_pagina)) * 100);
    for (int i = 0; i < tamanho_memoria_fisica; i++)
    {
        printf("%02X ", memoria_fisica[i]);
        if ((i + 1) % tamanho_pagina == 0)
        {
            printf("\n");
        }
    }
}

int criar_processo(int id, int tamanho)
{
    if (tamanho > tamanho_max_processo)
    {
        printf("Erro: Tamanho do processo excede o tamanho máximo configurado.\n");
        return 0;
    }

    int paginas_necessarias = (tamanho + tamanho_pagina - 1) / tamanho_pagina;
    int paginas_alocadas = 0;
    int quadros_disponiveis = 0;

    for (int i = 0; i < tamanho_memoria_fisica / tamanho_pagina; i++)
    {
        if (quadros_livres[i] == 1)
        {
            quadros_disponiveis++;
        }
    }

    if (quadros_disponiveis < paginas_necessarias)
    {
        printf("Erro: Memória insuficiente para alocar o processo.\n");
        return 0;
    }

    Processo *novo_processo = (Processo *)malloc(sizeof(Processo));
    novo_processo->id = id;
    novo_processo->tamanho = tamanho;
    novo_processo->tabela_paginas = (int *)malloc(paginas_necessarias * sizeof(int));
    novo_processo->prox = NULL;

    while (paginas_alocadas < paginas_necessarias)
    {
        int quadro = rand() % (tamanho_memoria_fisica / tamanho_pagina);
        if (quadros_livres[quadro] == 1)
        {
            quadros_livres[quadro] = 0;
            novo_processo->tabela_paginas[paginas_alocadas++] = quadro;
            int bytes_a_escrever = (tamanho > tamanho_pagina) ? tamanho_pagina : tamanho;
            for (int j = 0; j < bytes_a_escrever; j++)
            {
                memoria_fisica[quadro * tamanho_pagina + j] = rand() % 256;
            }
            tamanho -= bytes_a_escrever;
        }
    }

    // Adiciona o novo processo à tabela de processos
    novo_processo->prox = tabela_processos.cabeca;
    tabela_processos.cabeca = novo_processo;

    printf("Processo %d criado com sucesso.\n", id);
    return 1;
}

void visualizar_tabela_paginas(int id)
{
    Processo *atual = tabela_processos.cabeca;
    while (atual != NULL)
    {
        if (atual->id == id)
        {
            printf("Tamanho do processo: %d bytes\n", atual->tamanho);
            printf("Tabela de páginas:\n");
            for (int i = 0; i < (atual->tamanho + tamanho_pagina - 1) / tamanho_pagina; i++)
            {
                printf("Página %d -> Quadro %d\n", i, atual->tabela_paginas[i]);
            }
            return;
        }
        atual = atual->prox;
    }
    printf("Erro: Processo não encontrado.\n");
}

void limpar_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int main()
{
    srand(time(NULL));

    printf("Digite o tamanho da memória física (em bytes): ");
    scanf("%d", &tamanho_memoria_fisica);
    limpar_buffer();
    if (!multiplo_de_dois(tamanho_memoria_fisica))
    {
        printf("Erro: O tamanho da memória física deve ser uma potência de 2.\n");
        return 1;
    }

    printf("Digite o tamanho da página (em bytes): ");
    scanf("%d", &tamanho_pagina);
    limpar_buffer();
    if (!multiplo_de_dois(tamanho_pagina))
    {
        printf("Erro: O tamanho da página deve ser uma potência de 2.\n");
        return 1;
    }

    printf("Digite o tamanho máximo de um processo (em bytes): ");
    scanf("%d", &tamanho_max_processo);
    limpar_buffer();
    if (!multiplo_de_dois(tamanho_max_processo))
    {
        printf("Erro: O tamanho máximo de um processo deve ser uma potência de 2.\n");
        return 1;
    }

    memoria_fisica = (int *)malloc(tamanho_memoria_fisica * sizeof(int));
    quadros_livres = (int *)malloc((tamanho_memoria_fisica / tamanho_pagina) * sizeof(int));
    inicializar_memoria();

    while (1)
    {
        int opcao;
        printf("\n1. Visualizar memória\n2. Criar processo\n3. Visualizar tabela de páginas\n4. Sair\nEscolha uma opção: ");
        scanf("%d", &opcao);
        limpar_buffer();

        if (opcao == 1)
        {
            visualizar_memoria();
        }
        else if (opcao == 2)
        {
            int id, tamanho;
            printf("Digite o ID do processo: ");
            scanf("%d", &id);
            limpar_buffer();
            printf("Digite o tamanho do processo (em bytes): ");
            scanf("%d", &tamanho);
            limpar_buffer();
            if (!criar_processo(id, tamanho))
            {
                printf("\nErro ao criar o processo. Por favor, tente novamente.\n\n");
            }
        }
        else if (opcao == 3)
        {
            int id;
            printf("Digite o ID do processo: ");
            scanf("%d", &id);
            limpar_buffer();
            visualizar_tabela_paginas(id);
        }
        else if (opcao == 4)
        {
            break;
        }
        else
        {
            printf("Opção inválida.\n");
        }
    }

    free(memoria_fisica);
    free(quadros_livres);
    Processo *atual = tabela_processos.cabeca;
    while (atual != NULL)
    {
        Processo *temp = atual;
        atual = atual->prox;
        free(temp->tabela_paginas);
        free(temp);
    }

    return 0;
}
