#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "contas.dat"
#define TAM_NOME 50

typedef struct {
    int    numeroConta;
    char   nome[TAM_NOME];
    double saldo;
    int    ativo; 
} Cliente;

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

void cadastrarCliente(FILE *fp) {
    Cliente c;
    int posicao;

    printf("\n=== CADASTRAR CLIENTE ===\n");
    printf("Posicao (0, 1, 2, ...): ");
    scanf("%d", &posicao);
    limparBuffer();

    
    if (posicao < 0) {
        printf("Erro: posicao invalida.\n");
        pausar();
        return;
    }

    
    Cliente existente;
    fseek(fp, (long)posicao * sizeof(Cliente), SEEK_SET);
    size_t lidos = fread(&existente, sizeof(Cliente), 1, fp);
    if (lidos == 1 && existente.ativo == 1) {
        printf("Erro: posicao %d ja possui um cliente ativo.\n", posicao);
        pausar();
        return;
    }

    printf("Numero da conta : ");
    scanf("%d", &c.numeroConta);
    limparBuffer();

    rewind(fp);
    while (fread(&existente, sizeof(Cliente), 1, fp) == 1) {
        if (existente.ativo == 1 && existente.numeroConta == c.numeroConta) {
            printf("Erro: numero de conta %d ja existe.\n", c.numeroConta);
            pausar();
            return;
        }
    }

    printf("Nome            : ");
    fgets(c.nome, TAM_NOME, stdin);
    c.nome[strcspn(c.nome, "\n")] = '\0';

    printf("Saldo inicial   : ");
    scanf("%lf", &c.saldo);
    limparBuffer();

    c.ativo = 1;

    fseek(fp, 0, SEEK_END);
    long numRegistros = ftell(fp) / (long)sizeof(Cliente);
    if ((long)posicao > numRegistros) {
        Cliente vazio;
        memset(&vazio, 0, sizeof(Cliente));
        for (long i = numRegistros; i < (long)posicao; i++) {
            fseek(fp, i * (long)sizeof(Cliente), SEEK_SET);
            fwrite(&vazio, sizeof(Cliente), 1, fp);
        }
    }

    fseek(fp, (long)posicao * sizeof(Cliente), SEEK_SET);
    fwrite(&c, sizeof(Cliente), 1, fp);
    fflush(fp);

    printf("Cliente cadastrado na posicao %d com sucesso.\n", posicao);
    pausar();
}

void consultarCliente(FILE *fp) {
    int numeroConta;
    Cliente c;
    int encontrado = 0;

    printf("\n=== CONSULTAR CLIENTE ===\n");
    printf("Numero da conta: ");
    scanf("%d", &numeroConta);
    limparBuffer();

    rewind(fp);
    while (fread(&c, sizeof(Cliente), 1, fp) == 1) {
        if (c.ativo == 1 && c.numeroConta == numeroConta) {
            printf("\nConta  : %d\n", c.numeroConta);
            printf("Nome   : %s\n", c.nome);
            printf("Saldo  : R$ %.2f\n", c.saldo);
            encontrado = 1;
            break;
        }
    }

    if (!encontrado)
        printf("Cliente com conta %d nao encontrado.\n", numeroConta);

    pausar();
}

void atualizarSaldo(FILE *fp) {
    int numeroConta;
    Cliente c;
    int encontrado = 0;
    long posicaoBytes;

    printf("\n=== ATUALIZAR SALDO ===\n");
    printf("Numero da conta: ");
    scanf("%d", &numeroConta);
    limparBuffer();

    rewind(fp);
    while (fread(&c, sizeof(Cliente), 1, fp) == 1) {
        if (c.ativo == 1 && c.numeroConta == numeroConta) {
            posicaoBytes = ftell(fp) - (long)sizeof(Cliente);
            printf("Saldo atual: R$ %.2f\n", c.saldo);
            printf("Novo saldo : ");
            scanf("%lf", &c.saldo);
            limparBuffer();

            fseek(fp, posicaoBytes, SEEK_SET);
            fwrite(&c, sizeof(Cliente), 1, fp);
            fflush(fp);

            printf("Saldo atualizado com sucesso.\n");
            encontrado = 1;
            break;
        }
    }

    if (!encontrado)
        printf("Cliente com conta %d nao encontrado.\n", numeroConta);

    pausar();
}

void encerrarConta(FILE *fp) {
    int numeroConta;
    Cliente c;
    int encontrado = 0;
    long posicaoBytes;

    printf("\n=== ENCERRAR CONTA ===\n");
    printf("Numero da conta: ");
    scanf("%d", &numeroConta);
    limparBuffer();

    rewind(fp);
    while (fread(&c, sizeof(Cliente), 1, fp) == 1) {
        if (c.ativo == 1 && c.numeroConta == numeroConta) {
            posicaoBytes = ftell(fp) - (long)sizeof(Cliente);
            c.ativo = 0;

            fseek(fp, posicaoBytes, SEEK_SET);
            fwrite(&c, sizeof(Cliente), 1, fp);
            fflush(fp);

            printf("Conta %d encerrada com sucesso.\n", numeroConta);
            encontrado = 1;
            break;
        }
    }

    if (!encontrado)
        printf("Cliente com conta %d nao encontrado.\n", numeroConta);

    pausar();
}

void listarClientes(FILE *fp) {
    Cliente c;
    int total = 0;

    printf("\n=== LISTA DE CLIENTES ===\n");
    printf("%-6s %-30s %s\n", "Conta", "Nome", "Saldo");
    printf("----------------------------------------------\n");

    rewind(fp);
    while (fread(&c, sizeof(Cliente), 1, fp) == 1) {
        if (c.ativo == 1) {
            printf("%-6d %-30s R$ %.2f\n", c.numeroConta, c.nome, c.saldo);
            total++;
        }
    }

    if (total == 0) {
        printf("Nenhum cliente ativo encontrado.\n");
    } else {
        printf("----------------------------------------------\n");
        printf("Total de clientes ativos: %d\n", total);
    }

    pausar();
}

void repetirListagem(FILE *fp) {
    printf("\n=== REPETINDO LISTAGEM (rewind) ===\n");
    printf("Ponteiro de leitura retornou ao inicio do arquivo.\n");
    listarClientes(fp);
}

/* ---------- main ---------- */

int main() {
    FILE *fp;
    int opcao;

    fp = fopen(ARQUIVO, "r+b");
    if (fp == NULL) {
        fp = fopen(ARQUIVO, "w+b");
        if (fp == NULL) {
            perror("Erro ao abrir/criar arquivo");
            return 1;
        }
    }

    do {
        printf("\n=============================\n");
        printf("  SISTEMA DE CONTAS\n");
        printf("=============================\n");
        printf("1. Cadastrar cliente\n");
        printf("2. Consultar cliente\n");
        printf("3. Atualizar saldo\n");
        printf("4. Encerrar conta\n");
        printf("5. Listar clientes\n");
        printf("6. Repetir listagem (rewind)\n");
        printf("7. Sair\n");
        printf("-----------------------------\n");
        printf("Opcao: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1: cadastrarCliente(fp); break;
            case 2: consultarCliente(fp); break;
            case 3: atualizarSaldo(fp);   break;
            case 4: encerrarConta(fp);    break;
            case 5: listarClientes(fp);   break;
            case 6: repetirListagem(fp);  break;
            case 7: printf("Encerrando sistema...\n"); break;
            default: printf("Opcao invalida.\n"); pausar();
        }

    } while (opcao != 7);

    fclose(fp);
    return 0;
}