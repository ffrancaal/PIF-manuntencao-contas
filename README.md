# Sistema de Manutenção de Contas

Programa em C que gerencia contas de clientes usando um **arquivo binário de registros de tamanho fixo**.

## Funcionalidades

| Opção | Descrição |
|-------|-----------|
| 1 | Cadastrar cliente em uma posição específica |
| 2 | Consultar cliente pelo número da conta |
| 3 | Atualizar saldo de um cliente |
| 4 | Encerrar conta (marcação lógica como inativo) |
| 5 | Listar todos os clientes ativos |
| 6 | Repetir listagem usando `rewind()` |
| 7 | Encerrar o programa |

## Estrutura do registro

```c
typedef struct {
    int    numeroConta;   //  4 bytes  (offset  0)
    char   nome[50];      // 50 bytes  (offset  4)
                          //  2 bytes de padding (offset 54-55)
    double saldo;         //  8 bytes  (offset 56)
    int    ativo;         //  4 bytes  (offset 64)
                          //  4 bytes de padding final (offset 68-71)
} Cliente;               // total: 72 bytes por registro (sizeof reporta o valor real)
```

> O compilador insere padding para alinhar o `double` em múltiplo de 8 bytes e para que o tamanho total da struct também seja múltiplo de 8. Por isso o tamanho real é 72, não 66. O código usa `sizeof(Cliente)` em todas as operações, então isso é tratado automaticamente.

O arquivo `contas.dat` é criado automaticamente na primeira execução.

## Funções de I/O utilizadas

- `fseek()` — posiciona o ponteiro em um registro específico pelo índice
- `fread()` — lê um registro do arquivo binário
- `fwrite()` — escreve/atualiza um registro no arquivo binário
- `rewind()` — retorna o ponteiro ao início do arquivo para releitura
- `ftell()` — obtém a posição atual do ponteiro (usado para reescrever o registro correto)

## Como compilar e executar

```bash
gcc main.c -o programa
./programa
```

> Requer GCC instalado. Testado via compilador online por dificuldade de compilar no Windows (https://www.onlinegdb.com/online_c_compiler).

## Validações implementadas

- **Posição inválida:** impede cadastro em posição negativa
- **Posição ocupada:** verifica se já existe cliente ativo antes de cadastrar
- **Número de conta duplicado:** percorre o arquivo e rejeita contas já existentes
- **Preenchimento de lacunas:** ao cadastrar em posição maior que o tamanho atual do arquivo, os registros intermediários são preenchidos com registros zerados (`ativo = 0`), mantendo a integridade do acesso direto por `fseek()`

## Exclusão lógica

Contas encerradas (opção 4) **não são removidas fisicamente** do arquivo. O campo `ativo` é marcado como `0`, liberando a posição para futuro reuso via cadastro.

## Demonstração do rewind() (opção 6)

A opção 6 demonstra o comportamento de `rewind()` de forma explícita:

1. O ponteiro é avançado para o meio do arquivo via `fseek()` + `fread()`, simulando uma leitura parcial já realizada.
2. `ftell()` exibe a posição atual (byte > 0).
3. `rewind()` é chamado, retornando o ponteiro ao byte 0.
4. `ftell()` confirma a posição zero.
5. O arquivo é relido integralmente a partir do início.

Sem o `rewind()`, a leitura subsequente partiria do meio do arquivo e perderia os registros anteriores.