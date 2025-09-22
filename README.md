# Calculadora Concorrente Cliente-Servidor em C

**Beatriz Lima de Moura - 10416616
Giovana Simões Franco - 10417646**

Este projeto implementa uma aplicação cliente-servidor em **C** utilizando **sockets TCP/IPv4**.  
O servidor é capaz de processar operações matemáticas básicas enviadas por múltiplos clientes simultaneamente, retornando os resultados em tempo real.

---

## Principais Funcionalidades

- **Arquitetura Cliente-Servidor**: Comunicação baseada em sockets TCP.
- **Servidor Concorrente**: Utiliza `fork()` para atender múltiplos clientes ao mesmo tempo, sem que um cliente precise esperar o outro terminar.
- **Protocolo Híbrido**: Suporte para operações em formato **prefixo** (`ADD 10 20`) e **infixo** (`10 + 20`), além de comandos de texto (`HELP`, `VERSION`).
- **Build Automatizado**: Um **Makefile** completo gerencia a compilação do cliente e do servidor.
- **Testes Automatizados**: Um script shell (`tests.sh`) valida a funcionalidade do servidor, testando múltiplos cenários de uso e casos de erro.
- **Tratamento Robusto de Erros**: O servidor lida com entradas inválidas, divisão por zero e encerramento de conexões de forma segura, sem travar.

---

## Como Compilar e Executar

### Pré-requisitos
- Um compilador C (como `gcc` e `make`).
- Um ambiente compatível com scripts shell (Linux, macOS ou WSL/MSYS2 no Windows).

### Compilação
O projeto é compilado usando `make`. Na pasta raiz, execute:
```bash
# Compila o cliente e o servidor
make all
```
Para limpar os arquivos gerados, use `make clean`.

### Execução

**1. Inicie o Servidor**

Em um terminal, execute o servidor. A porta padrão é `5050`.

```bash
# Inicia na porta padrão
./server

# Ou especifique uma porta
./server 6000
```

**2. Conecte um ou mais Clientes**

Em outros terminais, inicie quantas instâncias do cliente desejar.

```bash
# Conecta ao servidor local na porta 6000
./client 127.0.0.1 6000
```

### Como Rodar os Testes Automatizados

O projeto inclui um script de teste (`tests.sh`) que automatiza a verificação do servidor, garantindo que ele se comporta como o esperado.

#### Pré-requisitos
- O arquivo `expected_output.txt` com os resultados esperados deve estar presente na raiz do projeto.
- Um ambiente que execute scripts shell (Linux, macOS, WSL, etc.).

#### Execução
**1. Garanta que o projeto esteja compilado:**
```bash
make all
```
**2. Torne o script executável:**
```bash
chmod +x tests.sh
```
**3. Execute o script de teste:**
```bash
./tests.sh
```
O script irá iniciar o servidor, executar os testes e, no final, comparar a saída gerada com o conteúdo do arquivo `expected_output.txt`. Uma mensagem de sucesso será exibida se os resultados forem idênticos.

## Especificação do Protocolo

### Formatos de Cálculo
O servidor entende os seguintes formatos para operações matemáticas:
| Formato | Exemplo  | Descrição                            |
|---------|----------|--------------------------------------|
| Prefixo | ADD 10 5 | Operador seguido de dois operandos.  |
| Infixo  | 12.5 * 2 | Operandos separados por um operador. |
**Operadores Suportados:** `ADD (+)`, `SUB (-)`, `MUL (*)`, `DIV (/)`

### Comandos Adicionais
| Comando | Ação                                     |
|---------|------------------------------------------|
| HELP    | Exibe uma lista de comandos disponíveis. |
| VERSION | Mostra a versão atual do servidor.       |
| QUIT    | Desconecta o cliente do servidor.        |

### Respostas do Servidor
As respostas seguem um formato simples para sucesso ou erro:
- **Sucesso:** `OK <resultado>`
   - Exemplo: `OK 15.000000`
- **Erro:** `ERR <CODIGO> <mensagem>`
  - Exemplo: `ERR EZDV divisao_por_zero`
 
## Decisões de Projeto e Limitações
- **Concorrência com `fork()` em vez de Threads:** Para atender múltiplos clientes, precisávamos de concorrência. Com base no que aprendemos sobre `threads` e `fork()`, fizemos a decisão de usar `fork()` porque achamos o modelo mais simples e robusto para este projeto. Cada cliente é tratado em um processo completamente separado, então um erro ou falha em uma das conexões não afeta em nada as outras. Embora `fork()` consuma um pouco mais de memória que threads, a simplicidade no gerenciamento e o isolamento total entre os clientes foram fatores decisivos.

- **Uso de `fdopen()` para I/O em Stream:** Em vez de usar as chamadas de sistema `read()` e `write()` diretamente com o descritor de arquivo do socket, nós decidimos associá-lo a um `FILE*` usando a função `fdopen()`. Fizemos isso porque o nosso protocolo é baseado em linhas. Com um `FILE*`, nós pudemos usar funções de alto nível como `fgets()` para ler uma linha inteira de uma vez e `fprintf()` para formatar e enviar a resposta. A alternativa, usando `read()`, foi nossa primeira escolha, porém exigia um controle manual de buffer para encontrar o caractere `\n`, o que levava a erros e era muito mais complexo, então mudamos nossa estratégia.

- **Tratamento de Sinais (SIGINT e SIGCHLD):** Percebemos que, ao fechar o servidor com `Ctrl+C`, o processo terminava de forma abrupta. Para resolver isso, implementamos um handler para o sinal `SIGINT`, que ativa uma flag para que o loop principal termine de forma controlada, fechando o socket principal corretamente. Além disso, ao usar `fork()`, aprendemos sobre os processos zumbis e adicionamos um handler para `SIGCHLD` para "limpar" os processos filhos que terminavam, garantindo que o servidor se mantivesse estável a longo prazo.

- **Separação de Erros Fatais e Operacionais:** Nós definimos uma estratégia clara para o tratamento de erros. Erros que impedem o servidor de sequer iniciar (como falha no `bind()` da porta) são tratados como fatais: o programa exibe uma mensagem clara com `perror()` e encerra com `exit(1)`. Por outro lado, erros durante a operação causados pelo cliente (como uma divisão por zero ou um comando inválido) são operacionais: o servidor envia uma mensagem de erro formatada de volta para o cliente, mas continua executando normalmente, pronto para atender a próxima requisição daquele ou de outros clientes. Isso tornou o servidor muito mais resiliente e segue as normas do projeto.
