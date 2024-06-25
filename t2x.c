#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAMANHO_MEMORIA_FISICA 512
#define TAMANHO_PAGINA 32
#define TAMANHO_MAXIMO_PROCESSO 64

typedef struct
{
    int quadro;
} EntradaTabelaPagina;

typedef struct
{
    int id;
    int tamanho;
    EntradaTabelaPagina *tabelaPaginas;
} Processo;

unsigned char memoriaFisica[TAMANHO_MEMORIA_FISICA];
int quadrosLivres[TAMANHO_MEMORIA_FISICA / TAMANHO_PAGINA];
int numQuadrosLivres = TAMANHO_MEMORIA_FISICA / TAMANHO_PAGINA;

Processo *processos[10]; // Array para armazenar processos
int numProcessos = 0;

void inicializarMemoriaFisica()
{
    for (int i = 0; i < TAMANHO_MEMORIA_FISICA; i++)
    {
        memoriaFisica[i] = 0;
    }
    for (int i = 0; i < TAMANHO_MEMORIA_FISICA / TAMANHO_PAGINA; i++)
    {
        quadrosLivres[i] = 1;
    }
}

void visualizarMemoria()
{
    printf("Memória Física:\n");
    for (int i = 0; i < TAMANHO_MEMORIA_FISICA; i++)
    {
        printf("%02x ", memoriaFisica[i]);
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\nPercentual de memória livre: %.2f%%\n", (float)numQuadrosLivres / (TAMANHO_MEMORIA_FISICA / TAMANHO_PAGINA) * 100);
}

Processo *criarProcesso(int id, int tamanho)
{
    if (tamanho > TAMANHO_MAXIMO_PROCESSO)
    {
        printf("Tamanho do processo excede o tamanho máximo permitido.\n");
        return NULL;
    }

    int numPaginas = (tamanho + TAMANHO_PAGINA - 1) / TAMANHO_PAGINA;

    if (numPaginas > numQuadrosLivres)
    {
        printf("Memória insuficiente para alocar o processo.\n");
        return NULL;
    }

    Processo *processo = (Processo *)malloc(sizeof(Processo));
    processo->id = id;
    processo->tamanho = tamanho;
    processo->tabelaPaginas = (EntradaTabelaPagina *)malloc(numPaginas * sizeof(EntradaTabelaPagina));

    srand(time(NULL));
    for (int i = 0; i < numPaginas; i++)
    {
        int quadro = -1;
        for (int j = 0; j < TAMANHO_MEMORIA_FISICA / TAMANHO_PAGINA; j++)
        {
            if (quadrosLivres[j])
            {
                quadro = j;
                quadrosLivres[j] = 0;
                numQuadrosLivres--;
                break;
            }
        }
        if (quadro == -1)
        {
            printf("Erro: não há quadros livres suficientes.\n");
            free(processo->tabelaPaginas);
            free(processo);
            return NULL;
        }

        processo->tabelaPaginas[i].quadro = quadro;

        for (int k = 0; k < TAMANHO_PAGINA && (i * TAMANHO_PAGINA + k) < tamanho; k++)
        {
            memoriaFisica[quadro * TAMANHO_PAGINA + k] = rand() % 256;
        }
    }

    processos[numProcessos++] = processo;
    return processo;
}

void visualizarTabelaPaginas(int id)
{
    for (int i = 0; i < numProcessos; i++)
    {
        if (processos[i]->id == id)
        {
            Processo *processo = processos[i];
            printf("Tabela de Páginas do Processo %d:\n", processo->id);
            printf("Tamanho: %d bytes\n", processo->tamanho);
            int numPaginas = (processo->tamanho + TAMANHO_PAGINA - 1) / TAMANHO_PAGINA;
            for (int j = 0; j < numPaginas; j++)
            {
                printf("Página %d -> Quadro %d\n", j, processo->tabelaPaginas[j].quadro);
            }
            return;
        }
    }
    printf("Processo com ID %d não encontrado.\n", id);
}

int main()
{
    inicializarMemoriaFisica();

    while (1)
    {
        printf("\n1. Visualizar memória\n2. Criar processo\n3. Visualizar tabela de páginas\n4. Sair\nEscolha uma opção: ");
        int opcao;
        scanf("%d", &opcao);

        if (opcao == 1)
        {
            visualizarMemoria();
        }
        else if (opcao == 2)
        {
            int id, tamanho;
            printf("Informe o ID do processo: ");
            scanf("%d", &id);
            printf("Informe o tamanho do processo (em bytes): ");
            scanf("%d", &tamanho);
            Processo *processo = criarProcesso(id, tamanho);
            if (processo != NULL)
            {
                printf("Processo criado com sucesso!\n");
            }
        }
        else if (opcao == 3)
        {
            int id;
            printf("Informe o ID do processo: ");
            scanf("%d", &id);
            visualizarTabelaPaginas(id);
        }
        else if (opcao == 4)
        {
            break;
        }
        else
        {
            printf("Opção inválida!\n");
        }
    }

    return 0;
}
