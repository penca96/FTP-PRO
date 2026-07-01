CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c99
LDFLAGS = -lpthread
TARGET = ftp_server
SRCDIR = src
OBJDIR = obj
BINDIR = bin
INSTALL_PATH = /usr/local/bin

SOURCES = $(SRCDIR)/ftp_server.c
OBJECTS = $(OBJDIR)/ftp_server.o

.PHONY: all clean run install uninstall help

help:
	@echo "╔════════════════════════════════════════════════════╗"
	@echo "║         FTP-PRO Build System Help                 ║"
	@echo "╚════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Targets disponibles:"
	@echo "  make              - Compila e instala automáticamente"
	@echo "  make clean        - Elimina archivos de compilación"
	@echo "  make run          - Ejecuta el servidor (requiere sudo)"
	@echo "  make install      - Instala en $(INSTALL_PATH)"
	@echo "  make uninstall    - Desinstala del PATH"
	@echo "  make build-only   - Solo compila sin instalar"
	@echo "  make help         - Muestra esta ayuda"
	@echo ""

# Target por defecto: compilar e instalar
all: build-only install-auto

build-only: $(BINDIR)/$(TARGET)
	@echo ""
	@echo "╔════════════════════════════════════════════════════╗"
	@echo "║      Compilación completada exitosamente          ║"
	@echo "╚════════════════════════════════════════════════════╝"

$(BINDIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	@echo "🔗 Enlazando objeto..."
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✓ Ejecutable creado: $@"

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	@echo "📦 Compilando: $<"
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "✓ Objeto creado: $@"

# Instalar automáticamente después de compilar
install-auto: $(BINDIR)/$(TARGET)
	@echo ""
	@echo "📌 Instalando en PATH..."
	@if [ ! -w $(INSTALL_PATH) ]; then \
		echo "⚠️  Se requieren permisos de administrador"; \
		sudo cp $(BINDIR)/$(TARGET) $(INSTALL_PATH)/; \
		sudo chmod +x $(INSTALL_PATH)/$(TARGET); \
	else \
		cp $(BINDIR)/$(TARGET) $(INSTALL_PATH)/; \
		chmod +x $(INSTALL_PATH)/$(TARGET); \
	fi
	@echo "✓ Servidor instalado en $(INSTALL_PATH)/$(TARGET)"
	@echo ""
	@echo "╔════════════════════════════════════════════════════╗"
	@echo "║  ✓ ¡Instalación completada!                       ║"
	@echo "║                                                    ║"
	@echo "║  Ahora puedes ejecutar desde cualquier lugar:      ║"
	@echo "║  $ sudo $(TARGET)                                  ║"
	@echo "║                                                    ║"
	@echo "║  O ver la ayuda:                                   ║"
	@echo "║  $ make help                                       ║"
	@echo "╚════════════════════════════════════════════════════╝"
	@echo ""

# Instalar manualmente
install: build-only
	@echo ""
	@echo "📌 Instalando manualmente en PATH..."
	@if [ ! -w $(INSTALL_PATH) ]; then \
		echo "⚠️  Se requieren permisos de administrador"; \
		sudo cp $(BINDIR)/$(TARGET) $(INSTALL_PATH)/; \
		sudo chmod +x $(INSTALL_PATH)/$(TARGET); \
	else \
		cp $(BINDIR)/$(TARGET) $(INSTALL_PATH)/; \
		chmod +x $(INSTALL_PATH)/$(TARGET); \
	fi
	@echo "✓ Servidor instalado en $(INSTALL_PATH)/$(TARGET)"

run: build-only
	@echo ""
	@echo "🚀 Iniciando servidor FTP-PRO..."
	@echo ""
	@sudo $(INSTALL_PATH)/$(TARGET)

clean:
	@echo "🧹 Limpiando archivos de compilación..."
	@rm -rf $(OBJDIR) $(BINDIR)
	@echo "✓ Archivos eliminados"

uninstall:
	@echo "🗑️  Desinstalando servidor..."
	@if [ ! -w $(INSTALL_PATH) ]; then \
		sudo rm -f $(INSTALL_PATH)/$(TARGET); \
	else \
		rm -f $(INSTALL_PATH)/$(TARGET); \
	fi
	@echo "✓ Servidor desinstalado de $(INSTALL_PATH)"

verify-install:
	@echo "🔍 Verificando instalación..."
	@if command -v $(TARGET) &> /dev/null; then \
		echo "✓ $(TARGET) está instalado en:"; \
		which $(TARGET); \
		echo "✓ Versión:"; \
		$(TARGET) --version 2>/dev/null || echo "  (sin información de versión)"; \
	else \
		echo "✗ $(TARGET) NO está en el PATH"; \
	fi
