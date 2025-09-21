#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h> // para waitpid
#include "../include/proto.h"

// variavel global para controlar o loop principal do servidor
volatile sig_atomic_t stop_server = 0;

void handle_sigint(int sig) {
    printf("\nSinal SIGINT recebido. Encerrando o servidor...\n");
    stop_server = 1;
}

void handle_sigchld(int sig) {
    // waitpid() em um loop para limpar todos os filhos que terminaram
    // WNOHANG garante que a chamada não bloqueie se não houver filhos para limpar
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void trim_newline(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

void process_request(const char *request, char *response) {
    char op_str[16];
    char op_char;
    double a, b, result;
    char extra;

    // --- tentativa 1: parse do formato PREFIXO (OP A B) ---
    int n_prefix = sscanf(request, "%15s %lf %lf %c", op_str, &a, &b, &extra);
    if (n_prefix == 3) { // Parse prefixo bem-sucedido
        if (strcasecmp(op_str, "ADD") == 0) {
            result = a + b;
            sprintf(response, "OK %.6f\n", result);
        } else if (strcasecmp(op_str, "SUB") == 0) {
            result = a - b;
            sprintf(response, "OK %.6f\n", result);
        } else if (strcasecmp(op_str, "MUL") == 0) {
            result = a * b;
            sprintf(response, "OK %.6f\n", result);
        } else if (strcasecmp(op_str, "DIV") == 0) {
            if (b == 0.0) {
                sprintf(response, "ERR EZDV divisao_por_zero\n");
            } else {
                result = a / b;
                sprintf(response, "OK %.6f\n", result);
            }
        } else {
            sprintf(response, "ERR EINV operacao_invalida\n");
        }
        return;
    }

    // --- tentativa 2: parse do formato INFIXO (A <op> B) ---
    int n_infix = sscanf(request, "%lf %c %lf %c", &a, &op_char, &b, &extra);
    if (n_infix == 3) { // parse infixo bem-sucedido
        switch (op_char) {
            case '+':
                result = a + b;
                sprintf(response, "OK %.6f\n", result);
                break;
            case '-':
                result = a - b;
                sprintf(response, "OK %.6f\n", result);
                break;
            case '*':
                result = a * b;
                sprintf(response, "OK %.6f\n", result);
                break;
            case '/':
                if (b == 0.0) {
                    sprintf(response, "ERR EZDV divisao_por_zero\n");
                } else {
                    result = a / b;
                    sprintf(response, "OK %.6f\n", result);
                }
                break;
            default:
                sprintf(response, "ERR EINV operacao_invalida\n");
                break;
        }
        return;
    }

    // --- se ambos os parses falharem, eh uma entrada invalida ---
    sprintf(response, "ERR EINV entrada_invalida\n");
}

int main(int argc, char *argv[]) {
    int server_fd;
    struct sockaddr_in server_addr;
    int port = DEFAULT_PORT;
    int opt = 1;

    signal(SIGINT, handle_sigint);
    signal(SIGCHLD, handle_sigchld);

    if (argc == 2) port = atoi(argv[1]);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    // evita problema de "address already in use" ao reiniciar o servidor
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(1);
    }

    printf("servidor rodando na porta %d...\n", port);

    while (!stop_server) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            // se o accept for interrompido por um sinal (como nosso SIGCHLD), apenas continue
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            continue;
        }

        // fork para criar um processo filho que cuidara do cliente
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            close(client_fd);
            continue; // pai continua a aceitar outros clientes
        }

        if (pid == 0) { // --- processo Filho ---
            // o filho nao precisa do socket de escuta do servidor
            close(server_fd);

            printf("filho (PID %d) cuidando do cliente: %s:%d\n",
                getpid(), inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // logica de comunicação com o cliente (seu codigo original)
            FILE *fp = fdopen(client_fd, "r+");
            if (fp == NULL) {
                perror("fdopen");
                close(client_fd);
                exit(1); // encerra o processo filho em caso de erro
            }

            char line[BUFFER_SIZE];
            char response[BUFFER_SIZE];

            while (fgets(line, sizeof(line), fp) != NULL) {
                trim_newline(line);
                if (line[0] == '\0') continue;

                if (strcasecmp(line, "QUIT") == 0) {
                    fprintf(fp, "OK bye\n");
                    fflush(fp);
                    break;
                }
                
                // --- BONUS: Comandos HELP e VERSION ---
                if (strcasecmp(line, "HELP") == 0) {
                    fprintf(fp, "OK Comandos: ADD|SUB|MUL|DIV A B | A op B | HELP | VERSION | QUIT\n");
                    fflush(fp);
                    continue; // pula para a proxima iteracao
                }
                if (strcasecmp(line, "VERSION") == 0) {
                    fprintf(fp, "OK Calculadora Server v1.1 (Concorrente)\n");
                    fflush(fp);
                    continue; // pula para a proxima iteracao
                }

                // se nao for um comando simples, processa a operação matematica
                process_request(line, response);
                fprintf(fp, "%s", response);
                fflush(fp);
            }

            fclose(fp); // isso tambem fecha client_fd
            printf("filho (PID %d) encerrou a conexao.\n", getpid());
            exit(0); // IMPORTANTE: O filho termina seu trabalho e sai

        } else { // --- processo Pai ---
            // o pai nao precisa do socket de comunicacao do cliente
            close(client_fd);
            printf("pai delegou cliente para o filho (PID %d).\n", pid);
            // o pai volta imediatamente para o início do loop e chama accept() novamente
        }
                printf("cliente desconectado.\n");
                // volta para aceitar proximo cliente (single-client-at-a-time)
    }

    printf("servidor encerrado.\n");
    close(server_fd);
    return 0;
}
