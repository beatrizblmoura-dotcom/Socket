#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../include/proto.h"

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int port = DEFAULT_PORT;
    char *server_ip = "127.0.0.1";

    // argumentos: ./client <ip> <porta>
    if (argc >= 2) {
        server_ip = argv[1];
    }
    if (argc == 3) {
        port = atoi(argv[2]);
    }

    // criar socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erro ao criar socket");
        exit(1);
    }

    // configurar endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Endereço inválido");
        close(sockfd);
        exit(1);
    }

    // conectar
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro na conexão");
        close(sockfd);
        exit(1);
    }

    printf("Conectado ao servidor %s:%d\n", server_ip, port);
    printf("Digite operações (ex: ADD 10 2) ou QUIT para sair.\n");

    // loop principal
    while (1) {
        printf("> ");
        fflush(stdout);

        // ler entrada do usuário
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // enviar para servidor
        write(sockfd, buffer, strlen(buffer));

        // se for QUIT não espera resposta
        if (strncasecmp(buffer, "QUIT", 4) == 0) {
            printf("Encerrando cliente...\n");
            break;
        }

        // ler resposta do servidor
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = read(sockfd, buffer, BUFFER_SIZE - 1);
        if (bytes <= 0) {
            printf("Servidor desconectou.\n");
            break;
        }

        // imprimir resposta
        printf("%s", buffer);
    }

    close(sockfd);
    return 0;
}
