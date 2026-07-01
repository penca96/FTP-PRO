CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c99
LDFLAGS = -lpthread
TARGET = ftp_server
SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES = $(SRCDIR)/ftp_server.c
OBJECTS = $(OBJDIR)/ftp_server.o

.PHONY: all clean run

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Compilación completada: $@"

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BINDIR)/$(TARGET)
	sudo ./$(BINDIR)/$(TARGET)

clean:
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Archivos de compilación eliminados"

install: $(BINDIR)/$(TARGET)
	sudo cp $(BINDIR)/$(TARGET) /usr/local/bin/
	@echo "Servidor instalado en /usr/local/bin/"

uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "Servidor desinstalado"