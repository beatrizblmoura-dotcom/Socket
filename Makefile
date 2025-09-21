# makefile para a Calculadora Cliente-Servidor

# compilador
CC = gcc
# flags de compilação: -I para diretorios de include, -g para debug, -Wall para avisos
CFLAGS = -Iinclude -g -Wall
# flags de linkagem (se necessario)
LDFLAGS =

# diretorios
SRCDIR = src
INCDIR = include
BUILDDIR = build

# arquivos fonte
CLIENT_SRC = $(SRCDIR)/client.c
SERVER_SRC = $(SRCDIR)/server.c

# arquivos objeto
CLIENT_OBJ = $(BUILDDIR)/client.o
SERVER_OBJ = $(BUILDDIR)/server.o

# alvos (executaveis)
CLIENT_EXE = client
SERVER_EXE = server

# target principal: compila tudo
all: $(CLIENT_EXE) $(SERVER_EXE)

# target para compilar o servidor
$(SERVER_EXE): $(SERVER_OBJ)
	@echo "Linkando o executável do servidor..."
	@$(CC) $(LDFLAGS) $^ -o $@

# target para compilar o cliente
$(CLIENT_EXE): $(CLIENT_OBJ)
	@echo "Linkando o executável do cliente..."
	@$(CC) $(LDFLAGS) $^ -o $@

# regra para compilar arquivos .o a partir de .c
# garante que os objetos sejam criados no diretorio de build
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	@echo "Compilando $< -> $@..."
	@$(CC) $(CFLAGS) -c $< -o $@

# target para limpar os arquivos gerados (objetos e executaveis)
clean:
	@echo "Limpando arquivos de build..."
	@rm -f $(CLIENT_EXE) $(SERVER_EXE) $(BUILDDIR)/*.o
	@rmdir $(BUILDDIR) 2>/dev/null || true

# targets "phony" para evitar conflitos com nomes de arquivos
.PHONY: all clean server client

# mapeia os nomes "server" e "client" para seus executaveis
server: $(SERVER_EXE)
client: $(CLIENT_EXE)



