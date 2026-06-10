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
    int    numeroConta;   // 4 bytes
    char   nome[50];      // 50 bytes
    double saldo;         // 8 bytes
    int    ativo;         // 4 bytes  (1 = ativo, 0 = encerrado)
} Cliente;               // total: 66 bytes por registro
```

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

> Requer GCC instalado. Testado via compilador online (https://www.onlinegdb.com/online_c_compiler) (Meu Windows está com problemas para conseguir executar, mas a compilação é facilitada em macOS/Linux).

## Validações implementadas

- **Posição inválida:** impede cadastro em posição negativa
- **Posição ocupada:** verifica se já existe cliente ativo antes de cadastrar
- **Número de conta duplicado:** percorre o arquivo e rejeita contas já existentes
- **Preenchimento de lacunas:** ao cadastrar em posição maior que o tamanho atual do arquivo, os registros intermediários são preenchidos com registros zerados (`ativo = 0`), mantendo a integridade do acesso direto por `fseek()`

## Exclusão lógica

Contas encerradas (opção 4) **não são removidas fisicamente** do arquivo. O campo `ativo` é marcado como `0`, liberando a posição para futuro reuso via cadastro.