# 🚀 FTP-PRO: Servidor FTP Profesional para Linux

<div align="center">

![FTP](https://img.shields.io/badge/Protocol-FTP-blue?style=flat-square)
![C](https://img.shields.io/badge/Language-C-brightgreen?style=flat-square)
![Linux](https://img.shields.io/badge/Platform-Linux-orange?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-purple?style=flat-square)
![Status](https://img.shields.io/badge/Status-Active-success?style=flat-square)
![Security](https://img.shields.io/badge/Security-3_Modes-red?style=flat-square)

Un **servidor FTP completo, moderno y compilable** desarrollado en C puro con **3 modos de seguridad configurables**, soporte para múltiples clientes simultáneos, interfaz ASCII Art decorativa y control de interfaz de red.

[⬇️ Instalación](#-instalación-rápida) • [🔐 Seguridad](#-modos-de-seguridad) • [📖 Documentación](#-documentación-completa) • [⚙️ Configuración](#-configuración) • [🌐 Interfaz de Red](#-control-de-interfaz-de-red)

</div>

---

## 📋 Tabla de Contenidos

- [✨ Características](#-características)
- [🔐 Modos de Seguridad](#-modos-de-seguridad)
- [📊 Comparativa de Modos](#-comparativa-de-modos-de-seguridad)
- [🌐 Control de Interfaz de Red](#-control-de-interfaz-de-red)
- [📋 Requisitos](#-requisitos)
- [⚡ Instalación Rápida](#-instalación-rápida)
- [📖 Documentación Completa](#-documentación-completa)
- [🎮 Uso del Servidor](#-uso-del-servidor)
- [⚙️ Configuración](#-configuración)
- [📁 Estructura del Proyecto](#-estructura-del-proyecto)
- [🔧 Detalles Técnicos](#-detalles-técnicos)
- [🔒 Protecciones de Seguridad](#-protecciones-de-seguridad-en-modo-secure)
- [🐛 Troubleshooting](#-troubleshooting)
- [📜 Licencia](#-licencia)

---

## ✨ Características

### Núcleo
- ✅ **Servidor FTP Completo** - RFC 959 compatible
- ✅ **Multi-cliente** - Soporte para múltiples conexiones simultáneas con threading
- ✅ **3 Modos de Seguridad** - Configurable según necesidades
- ✅ **Control de Interfaz de Red** - Especifica en qué IP escuchar
- ✅ **Interfaz Decorativa** - ASCII Art profesional y colores ANSI
- ✅ **Instalación Automática** - Se instala en PATH al compilar
- ✅ **Cross-Platform** - Funciona en cualquier distro Linux

### Comandos FTP Soportados
| Comando | Descripción |
|---------|-----------|
| `USER` | Autenticación de usuario |
| `PASS` | Contraseña |
| `LIST` / `LS` | Listar archivos del directorio actual |
| `PWD` | Mostrar directorio actual |
| `CWD` | Cambiar directorio de trabajo |
| `RETR` | Descargar archivo |
| `STOR` | Cargar archivo |
| `DELE` | Eliminar archivo |
| `MKD` | Crear directorio |
| `RMD` | Eliminar directorio |
| `TYPE` | Establecer tipo de transferencia |
| `SYST` | Información del sistema |
| `NOOP` | Operación nula (keepalive) |
| `HELP` | Mostrar comandos disponibles |
| `SECINFO` | Información de seguridad |
| `QUIT` | Desconectar |

---

## 🔐 Modos de Seguridad

### Descripción General

FTP-PRO incluye **3 modos de seguridad** que puedes activar según tus necesidades. Los modos menos seguros siguen disponibles para desarrollo y pruebas.

### 🔓 Modo 1: SIN AUTENTICACIÓN (`--no-auth`)

```bash
sudo ftp_server --no-auth
```

**Características:**
- ❌ **INSEGURO** - Acceso completamente libre
- ✅ Útil para: Desarrollo, testing, laboratorios
- ✅ No requiere contraseña
- ✅ No hay validación de usuarios

**Indicador:** 🔓 SECURITY: NONE (No authentication)

**Caso de Uso:**
```bash
# Ambiente de desarrollo local
sudo ftp_server --no-auth

# Acceso desde cliente
ftp> open localhost
ftp> ls
```

### 🔐 Modo 2: AUTENTICACIÓN BÁSICA (`--basic`) - Por Defecto

```bash
sudo ftp_server --basic
sudo ftp_server  # Usa --basic por defecto
```

**Características:**
- ⚠️ **SEGURIDAD MEDIA** - Acepta cualquier usuario/contraseña
- ✅ Requiere USER y PASS
- ✅ No valida contra el sistema
- ✅ Bueno para: Demostraciones, prototipos
- ✅ Todas las credenciales son aceptadas

**Indicador:** 🔐 SECURITY: BASIC (Any user accepted)

**Caso de Uso:**
```bash
# Modo pruebas/demostraciones
sudo ftp_server --basic

# Acceso desde cliente
ftp> open localhost
ftp> user admin
ftp> pass admin123
ftp> ls
```

### 🔒 Modo 3: AUTENTICACIÓN SEGURA (`--secure`) - Recomendado

```bash
sudo ftp_server --secure
```

**Características:**
- ✅ **MÁS SEGURO** - Autentica contra usuarios del sistema Linux
- ✅ Valida contraseñas contra /etc/shadow
- ✅ Usa hash crypt() para verificación
- ✅ Previene directory traversal (..)
- ✅ Valida nombres de archivo
- ✅ Protección contra path traversal attacks
- ✅ Ideal para: Producción, ambientes compartidos

**Indicador:** 🔒 SECURITY: SECURE (System users)

**Caso de Uso:**
```bash
# Modo producción
sudo ftp_server --secure

# Acceso desde cliente (usa usuario Linux real)
ftp> open localhost
ftp> user miusuario
ftp> pass micontraseña_real
ftp> ls
```

---

## 📊 Comparativa de Modos de Seguridad

| Característica | --no-auth | --basic | --secure |
|---|:---:|:---:|:---:|
| **Autenticación** | ❌ | ✓ | ✓ |
| **Verificación de usuario** | ❌ | ❌ | ✓ |
| **Verificación de contraseña** | ❌ | ✓* | ✓ |
| **Valida contra /etc/passwd** | ❌ | ❌ | ✓ |
| **Hash crypt() verificado** | ❌ | ❌ | ✓ |
| **Path traversal protection** | ❌ | ❌ | ✓ |
| **Filename validation** | ❌ | ❌ | ✓ |
| **Protege contra ../** | ❌ | ❌ | ✓ |
| **Acceso sin credenciales** | ✓ | ❌ | ❌ |
| **Recomendado para** | Desarrollo | Testing | Producción |
| **Nivel de Riesgo** | 🔴 Crítico | 🟡 Medio | 🟢 Bajo |

*En modo --basic se requiere USER/PASS pero cualquier valor es aceptado

---

## 🌐 Control de Interfaz de Red

### Opción `--ip` - Especificar IP de Escucha

FTP-PRO permite configurar en qué interfaz de red o dirección IP debe escuchar el servidor.

```bash
# Escuchar en localhost (más seguro, por defecto)
sudo ftp_server --ip 127.0.0.1

# Escuchar en interfaz específica
sudo ftp_server --ip 192.168.1.100

# Escuchar en todas las interfaces (NO RECOMENDADO)
sudo ftp_server --ip 0.0.0.0
```

### Características

- ✅ **Por defecto: `127.0.0.1`** - Solo acceso desde localhost
- ✅ **Validación de IP** - Rechaza IPs inválidas
- ✅ **Interfaz específica** - Perfecto para servidores multi-red
- ✅ **Mayor control de seguridad** - Limita acceso a interfaces específicas

### Ejemplos de Uso

```bash
# Servidor accesible solo localmente (recomendado)
sudo ftp_server --ip 127.0.0.1 --secure

# Servidor accesible en una interfaz interna
sudo ftp_server --ip 192.168.100.5 --secure

# Combinar con modo de seguridad
sudo ftp_server --ip 10.0.0.5 --basic --help
```

### Tabla de Direcciones IP Comunes

| Dirección IP | Alcance | Caso de Uso |
|---|---|---|
| `127.0.0.1` | Solo localhost | Desarrollo local, máximo nivel de seguridad |
| `192.168.x.x` | Red privada | Redes internas corporativas |
| `10.0.0.0/8` | Red privada | Ambientes virtuales, docker |
| `172.16.0.0/12` | Red privada | VPCs, networks privadas |
| `0.0.0.0` | Todas las interfaces | ⚠️ NO RECOMENDADO - Expone a todo |

---

## 📋 Requisitos

### Sistema Operativo
- **Linux** (Ubuntu, Debian, Fedora, CentOS, etc.)
- Kernel 2.6+

### Herramientas de Desarrollo
- **GCC** - Compilador C
- **GNU Make** - Sistema de compilación
- **Permisos sudo** - Para puerto 21 (FTP estándar)

### Dependencias Recomendadas
```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# Fedora/RHEL
sudo dnf install gcc make

# Arch
sudo pacman -S base-devel
```

---

## ⚡ Instalación Rápida

### Opción 1: Compilar y ejecutar

```bash
# Clonar repositorio
git clone https://github.com/penca96/FTP-PRO.git
cd FTP-PRO

# Compilar
make clean
make

# Ejecutar (requiere sudo para puerto 21)
sudo ./bin/ftp_server --secure --ip 127.0.0.1
```

### Opción 2: Instalar como comando del sistema

```bash
git clone https://github.com/penca96/FTP-PRO.git
cd FTP-PRO

make clean
make
sudo make install

# Ejecutar desde cualquier lugar
sudo ftp_server --secure --ip 127.0.0.1
```

---

## 📖 Documentación Completa

### Ver ayuda

```bash
sudo ftp_server --help
```

Muestra:
- Opciones de seguridad disponibles
- Ejemplos de uso
- Configuración por defecto

### Usar el servidor con todas las opciones

```bash
# Producción: Seguro, solo localhost
sudo ftp_server --secure --ip 127.0.0.1

# Desarrollo: Sin seguridad, localhost
sudo ftp_server --no-auth --ip 127.0.0.1

# Testing en red: Seguridad básica, interfaz específica
sudo ftp_server --basic --ip 192.168.1.100
```

---

## 🎮 Uso del Servidor

Una vez que el servidor está ejecutándose:

```bash
# Conectarse desde otro terminal o máquina
ftp localhost

# O usando lftp (más amigable)
lftp localhost
```

### Sesión de Ejemplo

```
$ ftp localhost
Connected to localhost (127.0.0.1).
220 Welcome to FTP-PRO Server v1.0
Name (localhost:user): admin
331 User name ok, need password
Password: admin123
230 User logged in, proceed

ftp> pwd
257 "/tmp" is current directory

ftp> ls
total 1234
drwxr-xr-x    2 owner  group        4096 Jul 01 12:34 downloads
-rw-r--r--    1 owner  group       5678 Jul 01 12:30 file.txt

ftp> cd downloads
250 Directory changed

ftp> get file.txt
local: file.txt remote: file.txt
200 PORT command successful
150 Opening data connection
226 Transfer complete
5678 bytes received in 0.05 seconds

ftp> quit
221 Goodbye
```

---

## ⚙️ Configuración

### Personalizar en código

Para cambios más profundos, edita `src/ftp_server.c`:

```c
// Puerto del servidor
#define FTP_PORT 21

// Tamaño del buffer de comunicación
#define BUFFER_SIZE 4096

// Cola de conexiones pendientes
#define BACKLOG 5

// Máximo de clientes simultáneos
#define MAX_CLIENTS 10
```

### Cambiar directorio raíz

El servidor utiliza `/tmp` como directorio raíz por defecto. Para cambiarlo:

```c
// En la función main(), línea ~250
strncpy(client->working_dir, "/var/ftp", sizeof(client->working_dir) - 1);
strncpy(client->root_dir, "/var/ftp", sizeof(client->root_dir) - 1);
```

### Recompilar después de cambios

```bash
make clean
make
sudo make install  # Si lo instalaste previamente
```

---

## 📁 Estructura del Proyecto

```
FTP-PRO/
├── src/
│   └── ftp_server.c          # Código fuente del servidor FTP
├── bin/
│   └── ftp_server            # Ejecutable compilado
├── obj/
│   └── ftp_server.o          # Objeto compilado
├── Makefile                  # Configuración de compilación
├── LICENSE                   # Licencia MIT
└── README.md                 # Este archivo
```

---

## 🔧 Detalles Técnicos

### Compilación

```bash
gcc -Wall -Wextra -pthread -std=c99 -o ftp_server src/ftp_server.c -lpthread
```

### Parámetros de compilación

| Flag | Descripción |
|------|-----------|
| `-Wall -Wextra` | Mostrar todos los warnings |
| `-pthread` | Soporte para threads POSIX |
| `-std=c99` | Estándar C99 |
| `-lpthread` | Enlazar librería de threads |

### Especificaciones del servidor

| Característica | Valor |
|---|---|
| **Puerto estándar** | 21 (FTP) |
| **IP por defecto** | 127.0.0.1 (localhost) |
| **Buffer de comunicación** | 4096 bytes |
| **Máximo de conexiones** | Ilimitado (multi-threaded) |
| **Directorio raíz** | /tmp |
| **Protocolo** | RFC 959 (FTP) |
| **Modo de transferencia** | ASCII y BINARY |

---

## 🔒 Protecciones de Seguridad en Modo SECURE

El modo `--secure` incluye múltiples capas de protección:

### 1. Validación de Autenticación
- ✅ Verifica usuario en `/etc/passwd`
- ✅ Valida contraseña con hash `crypt()`
- ✅ Rechaza acceso sin credenciales válidas

### 2. Protección contra Path Traversal
```c
validate_path()  // Previene acceso a .. o rutas fuera del root
```
- Valida que todas las rutas estén dentro del directorio raíz
- Resuelve rutas relativas con `realpath()`
- Rechaza intentos de escapar del sandbox

### 3. Validación de Nombres de Archivo
```c
is_safe_filename()  // Valida caracteres permitidos
```
- Rechaza caracteres especiales peligrosos
- Previene inyección de comandos
- Evita nombres malformados

### 4. Control de Interfaz
- Solo escucha en la IP especificada
- Previene acceso no autorizado en red

---

## 🐛 Troubleshooting

### Error: "Address already in use"

```bash
# El puerto 21 está en uso por otro proceso
# Opción 1: Esperar a que se libere
sleep 60 && sudo ftp_server

# Opción 2: Usar lsof para encontrar el proceso
sudo lsof -i :21
sudo kill -9 <PID>
```

### Error: "Permission denied"

```bash
# El puerto 21 requiere permisos de root
# Solución: Usa sudo
sudo ftp_server --secure --ip 127.0.0.1
```

### No se puede conectar desde otra máquina

```bash
# Verificar que la IP es correcta
sudo ftp_server --ip 192.168.1.100  # Tu IP real

# Verificar conectividad
ping 192.168.1.100

# Conectar especificando IP
ftp 192.168.1.100
```

### Cliente FTP se desconecta de repente

```bash
# Usa lftp que es más tolerante
lftp user@192.168.1.100

# O configure timeout más largo en cliente ftp
set net:timeout 60
```

---

## ⚡ Desinstalación

```bash
# Si instalaste con make install
sudo make uninstall

# Limpiar archivos compilados
make clean
```

---

## ⚠️ Notas de Seguridad

**Este servidor es principalmente para propósito educativo/demostrativo**, aunque incluye funcionalidades avanzadas de seguridad.

Para un servidor FTP de **PRODUCCIÓN en internet**, considera:

- 🔒 Usar librerías establecidas como **vsftpd** o **ProFTPD**
- 🔐 Implementar **FTPS (FTP sobre SSL/TLS)** o **SFTP**
- 🛡️ Agregar **firewall rules** restrictivas
- 📊 Implementar **logging y auditoría**
- 🔑 Usar **autenticación de clave pública**
- ⏱️ Configurar **timeouts y limits de conexión**
- 🚨 Monitorear **intentos de acceso fallidos**

Para ambientes locales y educativos, **FTP-PRO es perfectamente seguro** con modo `--secure`.

---

## 📊 Ejemplos de Uso Recomendados

### 🏠 Desarrollo Local
```bash
# Máxima seguridad, sin autenticación necesaria
sudo ftp_server --no-auth --ip 127.0.0.1
```

### 🧪 Testing/Demo
```bash
# Requiere credenciales, pero acepta cualquiera
sudo ftp_server --basic --ip 127.0.0.1
```

### 🔐 Producción Interna
```bash
# Autentica contra usuarios del sistema
sudo ftp_server --secure --ip 192.168.1.100
```

### 📚 Laboratorio Educativo
```bash
# Sin restricciones, para aprender FTP
sudo ftp_server --no-auth --ip 192.168.100.1
```

---

## 📜 Licencia

MIT License - Ver `LICENSE` para más detalles

```
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files...
```

---

## 👤 Autor

**penca96** - Desarrollador del proyecto

- GitHub: [@penca96](https://github.com/penca96)
- Proyecto: [FTP-PRO](https://github.com/penca96/FTP-PRO)

---

## 💬 Soporte y Contribuciones

### Reportar Problemas
Para reportar bugs o sugerencias, crea un **issue** en el repositorio:
- [Issues](https://github.com/penca96/FTP-PRO/issues)

### Contribuir
Se aceptan pull requests con mejoras:
1. Fork el repositorio
2. Crea una rama (`git checkout -b feature/mejora`)
3. Commit tus cambios (`git commit -am 'Add mejora'`)
4. Push a la rama (`git push origin feature/mejora`)
5. Abre un Pull Request

---

## 📚 Recursos Adicionales

- [RFC 959 - File Transfer Protocol](https://tools.ietf.org/html/rfc959)
- [Linux man pages - FTP](https://linux.die.net/man/1/ftp)
- [POSIX Threads](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)

---

<div align="center">

⭐ Si te fue útil, ¡dale una estrella en GitHub!

Hecho con ❤️ por penca96

</div>
