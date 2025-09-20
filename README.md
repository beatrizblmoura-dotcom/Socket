Calculadora Concorrente Cliente-Servidor em C
Este projeto implementa uma aplicação cliente-servidor em C utilizando sockets TCP/IPv4. O servidor é capaz de processar operações matemáticas básicas enviadas por múltiplos clientes simultaneamente, retornando os resultados em tempo real.

Principais Funcionalidades
Arquitetura Cliente-Servidor: Comunicação baseada em sockets TCP.

Servidor Concorrente: Utiliza fork() para atender múltiplos clientes ao mesmo tempo, sem que um cliente precise esperar o outro terminar.

Protocolo Híbrido: Suporte para operações em formato prefixo (ADD 10 20) e infixo (10 + 20), além de comandos de texto (HELP, VERSION).

Build Automatizado: Um Makefile completo gerencia a compilação do cliente e do servidor.

Testes Automatizados: Um script shell (run_tests.sh) valida a funcionalidade do servidor, testando múltiplos cenários de uso e casos de erro.

Tratamento Robusto de Erros: O servidor lida com entradas inválidas, divisão por zero e encerramento de conexões de forma segura, sem travar.

Como Compilar e Executar
Pré-requisitos
Um compilador C (como gcc e make).

Um ambiente compatível com scripts shell (Linux, macOS ou WSL/MSYS2 no Windows).

Compilação
Para compilar o cliente e o servidor, execute o comando make all na raiz do projeto. Para limpar os arquivos gerados, use make clean.

Execução
Inicie o Servidor: Em um terminal, execute o servidor. A porta padrão é 5050. Para iniciar na porta padrão, use ./server. Para especificar uma porta, como a 6000, use ./server 6000.

Conecte um ou mais Clientes: Em outros terminais, inicie o cliente com o comando ./client 127.0.0.1 6000, ajustando o endereço IP e a porta conforme a configuração do seu servidor.

Como Rodar os Testes Automatizados
O projeto inclui um script de teste (run_tests.sh) que automatiza a verificação do servidor.

Pré-requisitos
O arquivo expected_output.txt com os resultados esperados deve estar presente na raiz do projeto.

Um ambiente que execute scripts shell (Linux, macOS, WSL, etc.).

Execução
Após compilar o projeto com make all, torne o script de teste executável com o comando chmod +x run_tests.sh. Este passo só precisa ser feito uma vez. Em seguida, execute os testes usando ./run_tests.sh.

O script irá iniciar o servidor, executar os testes e, no final, comparar a saída gerada com o conteúdo do arquivo expected_output.txt. Uma mensagem de sucesso será exibida se os resultados forem idênticos.

Especificação do Protocolo
Formatos de Cálculo
O servidor entende os seguintes formatos para operações matemáticas:

Formato	Exemplo	Descrição
Prefixo	ADD 10 5	Operador seguido de dois operandos.
Infixo	12.5 * 2	Operandos separados por um operador.

Export to Sheets
Operadores Suportados: ADD (+), SUB (-), MUL (*), DIV (/)

Comandos Adicionais
Comando	Ação
HELP	Exibe uma lista de comandos disponíveis.
VERSION	Mostra a versão atual do servidor.
QUIT	Desconecta o cliente do servidor.

Export to Sheets
Respostas do Servidor
As respostas seguem um formato simples para sucesso ou erro:

Sucesso: OK <resultado>

Exemplo: OK 15.000000

Erro: ERR <CODIGO> <mensagem>

Exemplo: ERR EZDV divisao_por_zero

Decisões de Projeto e Limitações
Concorrência: A escolha do fork() para concorrência é um modelo clássico e robusto, criando um isolamento completo entre os processos dos clientes. A desvantagem é um consumo de memória maior em comparação com uma abordagem baseada em threads.

Protocolo: Um protocolo de texto puro foi escolhido pela simplicidade e facilidade de depuração.

Segurança: O projeto tem fins educacionais e não implementa nenhuma camada de segurança, como criptografia (SSL/TLS) ou autenticação de usuários.

Estado: O servidor não mantém estado entre as requisições. Cada cálculo é uma transação independente.
