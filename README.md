# FTP-PRO: Servidor FTP para Linux

Un servidor FTP (File Transfer Protocol) completo desarrollado en C, compilable en Linux.

## Características

- ✅ Servidor FTP escuchando en puerto 21
- ✅ Soporte para múltiples clientes simultáneos (threading)
- ✅ Autenticación básica (USER/PASS)
- ✅ Comandos FTP estándar:
  - **LIST/ls** - Listar archivos en directorio actual
  - **CWD** - Cambiar directorio de trabajo
  - **PWD** - Mostrar directorio actual
  - **RETR** - Descargar archivo
  - **STOR** - Cargar archivo
  - **DELE** - Eliminar archivo
  - **MKD** - Crear directorio
  - **RMD** - Eliminar directorio
  - **TYPE** - Establecer tipo de transferencia
  - **SYST** - Información del sistema
  - **QUIT** - Cerrar conexión
  - **NOOP** - Operación nula

## Requisitos

- Linux (Ubuntu, Debian, Fedora, etc.)
- GCC (compilador C)
- GNU Make
- Permisos de sudo para ejecutar en puerto 21

## Instalación y Compilación

### Opción 1: Compilar y ejecutar

```bash
# Clonar o descargar el repositorio
git clone https://github.com/penca96/FTP-PRO.git
cd FTP-PRO

# Compilar
make clean
make

# Ejecutar (requiere sudo para puerto 21)
sudo ./bin/ftp_server
```

### Opción 2: Instalar como comando del sistema

```bash
make clean
make
sudo make install

# Ejecutar desde cualquier lugar
sudo ftp_server
```

## Uso del Servidor

Una vez que el servidor está ejecutándose:

```bash
# Conectarse desde otro terminal o máquina
ftp localhost

# O usando lftp (más amigable)
lftp localhost
```

### Comandos FTP

```
ftp> user anonymous
331 User name ok, need password
ftp> pass password
230 User logged in, proceed
ftp> ls
... (lista de archivos)
ftp> pwd
257 "/tmp" is current directory
ftp> cd /tmp
250 Directory changed
ftp> get archivo.txt
ftp> put archivo.txt
ftp> quit
```

## Estructura del Proyecto

```
FTP-PRO/
├── src/
│   └── ftp_server.c      # Código fuente del servidor FTP
├── Makefile              # Configuración de compilación
└── README.md             # Este archivo
```

## Detalles Técnicos

### Compilación

```bash
gcc -Wall -Wextra -pthread -std=c99 -o ftp_server src/ftp_server.c -lpthread
```

### Parámetros de compilación

- `-Wall -Wextra`: Mostrar todos los warnings
- `-pthread`: Soporte para threads POSIX
- `-std=c99`: Estándar C99
- `-lpthread`: Enlazar librería de threads

### Directorio por defecto

El servidor utiliza `/tmp` como directorio raíz para las operaciones FTP.

### Límites

- **Puerto**: 21 (FTP estándar)
- **Buffer**: 4096 bytes
- **Máximo de conexiones**: 10 (configurable)

## Cambios en el Código

Para personalizar el servidor, edita `src/ftp_server.c`:

```c
#define FTP_PORT 21          // Puerto del servidor
#define BUFFER_SIZE 4096     // Tamaño del buffer
#define BACKLOG 5            // Cola de conexiones
```

Para cambiar el directorio raíz:

```c
strcpy(client->working_dir, "/path/to/directory");  // Línea 119
```

## Desinstalación

```bash
sudo make uninstall
make clean
```

## Notas de Seguridad

⚠️ **Este servidor es de propósito educativo/demostrativo**

Para un servidor FTP de producción, considere:
- Usar librerías establecidas como vsftpd
- Implementar autenticación real
- Agregar control de acceso y permisos
- Usar conexiones cifradas (SFTP/FTPS)
- Validar y sanitizar todas las entradas

## Licencia

MIT License - Ver LICENSE para más detalles

## Autor

penca96

## Soporte

Para reportar problemas o sugerencias, crea un issue en el repositorio.