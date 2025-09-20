# Makefile para a Calculadora Cliente-Servidor

# Compilador
CC = gcc
# Flags de compilação: -I para diretórios de include, -g para debug, -Wall para avisos
CFLAGS = -Iinclude -g -Wall
# Flags de linkagem (se necessário)
LDFLAGS =

# Diretórios
SRCDIR = src
INCDIR = include
BUILDDIR = build

# Arquivos fonte
CLIENT_SRC = $(SRCDIR)/client.c
SERVER_SRC = $(SRCDIR)/server.c

# Arquivos objeto
CLIENT_OBJ = $(BUILDDIR)/client.o
SERVER_OBJ = $(BUILDDIR)/server.o

# Alvos (executáveis)
CLIENT_EXE = client
SERVER_EXE = server

# Target principal: compila tudo
all: $(CLIENT_EXE) $(SERVER_EXE)

# Target para compilar o servidor
$(SERVER_EXE): $(SERVER_OBJ)
	@echo "Linkando o executável do servidor..."
	@$(CC) $(LDFLAGS) $^ -o $@

# Target para compilar o cliente
$(CLIENT_EXE): $(CLIENT_OBJ)
	@echo "Linkando o executável do cliente..."
	@$(CC) $(LDFLAGS) $^ -o $@

# Regra para compilar arquivos .o a partir de .c
# Garante que os objetos sejam criados no diretório de build
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	@echo "Compilando $< -> $@..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Target para limpar os arquivos gerados (objetos e executáveis)
clean:
	@echo "Limpando arquivos de build..."
	@rm -f $(CLIENT_EXE) $(SERVER_EXE) $(BUILDDIR)/*.o
	@rmdir $(BUILDDIR) 2>/dev/null || true

# Targets "phony" para evitar conflitos com nomes de arquivos
.PHONY: all clean server client

# Mapeia os nomes "server" e "client" para seus executáveis
server: $(SERVER_EXE)
client: $(CLIENT_EXE)
