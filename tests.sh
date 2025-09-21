#!/bin/bash
# script de teste automatizado para a Calculadora Cliente/Servidor
PORT=5055
SERVER_EXE="./server"
CLIENT_EXE="./client"
EXPECTED_OUTPUT_FILE="expected_output.txt"
TEST_OUTPUT_FILE="test_output.txt"

# cores
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "Iniciando o servidor na porta $PORT em segundo plano..."
$SERVER_EXE $PORT &
SERVER_PID=$!
# pequena pausa para garantir que o servidor esteja pronto para aceitar conexões
sleep 1

# verifica se o servidor realmente iniciou
if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo -e "${RED}ERRO: O servidor não pôde ser iniciado.${NC}"
    exit 1
fi

echo "Servidor iniciado com PID $SERVER_PID. Executando testes..."

# executa o cliente com uma série de comandos via 'here document'
# a saída do cliente (stdout e stderr) é redirecionada para o arquivo de saída do teste
$CLIENT_EXE 127.0.0.1 $PORT > $TEST_OUTPUT_FILE 2>&1 <<EOF
ADD 10 2
10.5 + 2.5
SUB 5 10
-5 - -2
MUL 3.5 2
5 * -1.5
DIV 10 4
100 / 0
DIV 5 2.5
HELP
VERSION
SUB 10
isso eh lixo
QUIT
EOF

echo "Testes finalizados. Encerrando o servidor..."
kill $SERVER_PID
# Espera o servidor terminar completamente
wait $SERVER_PID 2>/dev/null

echo "Comparando a saída com o resultado esperado..."

# compara a saida real com a saida esperada
if diff -u "$EXPECTED_OUTPUT_FILE" "$TEST_OUTPUT_FILE"; then
    echo -e "${GREEN}SUCESSO: Todos os testes passaram!${NC}"
    rm $TEST_OUTPUT_FILE # Limpa o arquivo de saída
    exit 0
else
    echo -e "${RED}FALHA: A saída do teste é diferente do esperado.${NC}"
    echo "Veja a diferença acima. O arquivo de saída foi mantido como '$TEST_OUTPUT_FILE' para análise."
    exit 1
fi
tests.sh
Displaying tests.sh.

