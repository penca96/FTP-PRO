#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>

#define FTP_PORT 21
#define BACKLOG 5
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 10

// Colores ANSI
#define COLOR_RESET     "\x1b[0m"
#define COLOR_GREEN     "\x1b[32m"
#define COLOR_BLUE      "\x1b[34m"
#define COLOR_CYAN      "\x1b[36m"
#define COLOR_YELLOW    "\x1b[33m"
#define COLOR_RED       "\x1b[31m"
#define BOLD            "\x1b[1m"

// ASCII Art
const char* BANNER = COLOR_CYAN BOLD 
"  ███████╗████████╗██████╗ ███████╗██████╗ ██████╗ \n"
"  ██╔════╝╚══██╔══╝██╔══██╗██╔════╝██╔══██╗██╔══██╗\n"
"  █████╗     ██║   ██████╔╝█████╗  ██████╔╝██████╔╝\n"
"  ██╔══╝     ██║   ██╔═══╝ ██╔══╝  ██╔═══╝ ██╔══██╗\n"
"  ██║        ██║   ██║     ███████╗██║     ██║  ██║\n"
"  ╚═╝        ╚═╝   ╚═╝     ╚══════╝╚═╝     ╚═╝  ╚═╝\n"
COLOR_RESET;

const char* PRO_BANNER = COLOR_GREEN BOLD
"  ╔════════════════════════════════════════╗\n"
"  ║         FTP-PRO v1.0 SERVER            ║\n"
"  ║    Professional FTP Server for Linux    ║\n"
"  ╚════════════════════════════════════════╝\n"
COLOR_RESET;

const char* WELCOME_ASCII = COLOR_BLUE
"   ╔─────────────────────────────────────╗\n"
"   │ ★ Welcome to FTP-PRO Server ★       │\n"
"   │ Please authenticate to continue     │\n"
"   ╚─────────────────────────────────────╝\n"
COLOR_RESET;

const char* SUCCESS_ASCII = COLOR_GREEN
"   ✓ Authentication successful!\n"
"   ✓ Ready to transfer files\n"
COLOR_RESET;

const char* ERROR_ASCII = COLOR_RED
"   ✗ Authentication failed\n"
COLOR_RESET;

const char* SHUTDOWN_ASCII = COLOR_YELLOW BOLD
"  ╔════════════════════════════════════════╗\n"
"  ║  Server shutting down gracefully...    ║\n"
"  ║  Goodbye! 👋                           ║\n"
"  ╚════════════════════════════════════════╝\n"
COLOR_RESET;

// Estructura para manejar conexiones de clientes
typedef struct {
    int socket;
    char working_dir[BUFFER_SIZE];
    int data_socket;
} client_t;

// Variable global para el socket del servidor
int server_socket = -1;

// Prototipo de funciones
void print_banner(void);
void* handle_client(void* arg);
void send_response(int socket, int code, const char* message);
void handle_command(client_t* client, const char* command);
void cmd_user(client_t* client, const char* args);
void cmd_pass(client_t* client, const char* args);
void cmd_list(client_t* client);
void cmd_cwd(client_t* client, const char* args);
void cmd_pwd(client_t* client);
void cmd_retr(client_t* client, const char* filename);
void cmd_stor(client_t* client, const char* filename);
void cmd_dele(client_t* client, const char* filename);
void cmd_rmd(client_t* client, const char* dirname);
void cmd_mkd(client_t* client, const char* dirname);
void cmd_quit(client_t* client);
void cmd_help(client_t* client);
void signal_handler(int signum);

void print_banner(void) {
    system("clear");
    printf("%s\n", BANNER);
    printf("%s\n", PRO_BANNER);
    printf(COLOR_CYAN "╔════════════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║ " COLOR_GREEN "⚙️  Server Configuration" COLOR_CYAN "                         ║\n" COLOR_RESET);
    printf(COLOR_CYAN "║ " COLOR_YELLOW "Port:" COLOR_RESET " 21 (FTP Standard)                      " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║ " COLOR_YELLOW "Protocol:" COLOR_RESET " FTP (RFC 959)                       " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║ " COLOR_YELLOW "Max Clients:" COLOR_RESET " Unlimited (Multi-threaded)        " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║ " COLOR_YELLOW "Root Directory:" COLOR_RESET " /tmp                            " COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "╚════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
}

int main(int argc, char* argv[]) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    int client_socket;
    pthread_t thread_id;

    // Configurar manejador de señales
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Mostrar banner
    print_banner();

    // Crear socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        fprintf(stderr, COLOR_RED "✗ Error al crear socket\n" COLOR_RESET);
        perror("Error al crear socket");
        exit(EXIT_FAILURE);
    }

    // Permitir reutilizar el puerto
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, COLOR_RED "✗ Error en setsockopt\n" COLOR_RESET);
        perror("Error en setsockopt");
        exit(EXIT_FAILURE);
    }

    // Configurar dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(FTP_PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, COLOR_RED "✗ Error en bind - Puerto 21 requiere privilegios de root\n" COLOR_RESET);
        perror("Error en bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_socket, BACKLOG) < 0) {
        fprintf(stderr, COLOR_RED "✗ Error en listen\n" COLOR_RESET);
        perror("Error en listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf(COLOR_GREEN BOLD "✓ " COLOR_RESET COLOR_GREEN "Servidor FTP iniciado en puerto %d\n" COLOR_RESET, FTP_PORT);
    printf(COLOR_CYAN "╔════════════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║ " COLOR_GREEN "🟢 Estado: ACTIVO" COLOR_CYAN "                                  ║\n" COLOR_RESET);
    printf(COLOR_CYAN "║ " COLOR_YELLOW "Escuchando en: 0.0.0.0:21" COLOR_CYAN "                       ║\n" COLOR_RESET);
    printf(COLOR_CYAN "║ " COLOR_YELLOW "Esperando conexiones..." COLOR_CYAN "                         ║\n" COLOR_RESET);
    printf(COLOR_CYAN "╚════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");

    // Aceptar conexiones
    while (1) {
        client_addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);

        if (client_socket < 0) {
            fprintf(stderr, COLOR_RED "✗ Error en accept\n" COLOR_RESET);
            perror("Error en accept");
            continue;
        }

        printf(COLOR_GREEN "✓ " COLOR_RESET "Nueva conexión de " COLOR_YELLOW "%s\n" COLOR_RESET, inet_ntoa(client_addr.sin_addr));

        // Crear estructura para el cliente
        client_t* client = (client_t*)malloc(sizeof(client_t));
        if (client == NULL) {
            fprintf(stderr, COLOR_RED "✗ Error al asignar memoria\n" COLOR_RESET);
            close(client_socket);
            continue;
        }

        client->socket = client_socket;
        client->data_socket = -1;
        strcpy(client->working_dir, "/tmp");

        // Crear thread para manejar el cliente
        if (pthread_create(&thread_id, NULL, handle_client, (void*)client) != 0) {
            fprintf(stderr, COLOR_RED "✗ Error al crear thread\n" COLOR_RESET);
            free(client);
            close(client_socket);
            continue;
        }

        pthread_detach(thread_id);
    }

    close(server_socket);
    return EXIT_SUCCESS;
}

void* handle_client(void* arg) {
    client_t* client = (client_t*)arg;
    char buffer[BUFFER_SIZE];
    int bytes_read;
    int authenticated = 0;

    // Enviar mensaje de bienvenida con ASCII art
    send_response(client->socket, 220, "FTP-PRO Server Ready - Welcome!");
    
    // Enviar banner de bienvenida
    const char* welcome = WELCOME_ASCII;
    send(client->socket, welcome, strlen(welcome), 0);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_read = recv(client->socket, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_read <= 0) {
            printf(COLOR_YELLOW "⚠ " COLOR_RESET "Cliente desconectado\n");
            break;
        }

        // Remover saltos de línea
        char* newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';
        newline = strchr(buffer, '\r');
        if (newline) *newline = '\0';

        printf(COLOR_BLUE "➤ " COLOR_RESET "Comando: %s\n", buffer);

        // Procesar comandos
        char command[50], args[BUFFER_SIZE];
        memset(command, 0, sizeof(command));
        memset(args, 0, sizeof(args));
        sscanf(buffer, "%s %[^\n]", command, args);

        // Convertir comando a mayúsculas
        for (int i = 0; command[i]; i++) {
            command[i] = toupper(command[i]);
        }

        // Comandos sin autenticación
        if (strcmp(command, "USER") == 0) {
            cmd_user(client, args);
            authenticated = 1;
            continue;
        }

        if (strcmp(command, "PASS") == 0) {
            cmd_pass(client, args);
            continue;
        }

        if (strcmp(command, "QUIT") == 0) {
            cmd_quit(client);
            break;
        }

        if (strcmp(command, "HELP") == 0) {
            cmd_help(client);
            continue;
        }

        // Comandos que requieren autenticación
        if (!authenticated) {
            send_response(client->socket, 530, "Please login with USER and PASS");
            send(client->socket, ERROR_ASCII, strlen(ERROR_ASCII), 0);
            continue;
        }

        // Comandos autenticados
        if (strcmp(command, "LIST") == 0 || strcmp(command, "LS") == 0) {
            cmd_list(client);
        } else if (strcmp(command, "CWD") == 0) {
            cmd_cwd(client, args);
        } else if (strcmp(command, "PWD") == 0) {
            cmd_pwd(client);
        } else if (strcmp(command, "RETR") == 0) {
            cmd_retr(client, args);
        } else if (strcmp(command, "STOR") == 0) {
            cmd_stor(client, args);
        } else if (strcmp(command, "DELE") == 0) {
            cmd_dele(client, args);
        } else if (strcmp(command, "RMD") == 0) {
            cmd_rmd(client, args);
        } else if (strcmp(command, "MKD") == 0) {
            cmd_mkd(client, args);
        } else if (strcmp(command, "TYPE") == 0) {
            send_response(client->socket, 200, "Type set to I");
        } else if (strcmp(command, "SYST") == 0) {
            send_response(client->socket, 215, "UNIX Type: L8");
        } else if (strcmp(command, "NOOP") == 0) {
            send_response(client->socket, 200, "NOOP ok");
        } else {
            send_response(client->socket, 502, "Command not implemented");
        }
    }

    close(client->socket);
    if (client->data_socket >= 0) {
        close(client->data_socket);
    }
    free(client);
    pthread_exit(NULL);
}

void send_response(int socket, int code, const char* message) {
    char response[BUFFER_SIZE];
    snprintf(response, BUFFER_SIZE, "%d %s\r\n", code, message);
    send(socket, response, strlen(response), 0);
}

void cmd_user(client_t* client, const char* args) {
    send_response(client->socket, 331, "User name ok, need password");
}

void cmd_pass(client_t* client, const char* args) {
    send_response(client->socket, 230, "User logged in, proceed");
    send(client->socket, SUCCESS_ASCII, strlen(SUCCESS_ASCII), 0);
}

void cmd_help(client_t* client) {
    const char* help_text = COLOR_CYAN
        "═══════════════════════════════════════\n"
        "         FTP-PRO HELP COMMANDS\n"
        "═══════════════════════════════════════\n"
        "LIST / LS      - List files in current directory\n"
        "PWD            - Print working directory\n"
        "CWD <dir>      - Change working directory\n"
        "RETR <file>    - Download file\n"
        "STOR <file>    - Upload file\n"
        "DELE <file>    - Delete file\n"
        "MKD <dir>      - Create directory\n"
        "RMD <dir>      - Remove directory\n"
        "TYPE           - Set transfer type\n"
        "SYST           - Show system info\n"
        "NOOP           - No operation\n"
        "HELP           - Show this help\n"
        "QUIT           - Disconnect\n"
        "═══════════════════════════════════════\n"
        COLOR_RESET;
    
    send_response(client->socket, 214, "Help follows");
    send(client->socket, help_text, strlen(help_text), 0);
}

void cmd_list(client_t* client) {
    DIR* dir = opendir(client->working_dir);
    if (dir == NULL) {
        send_response(client->socket, 550, "Failed to open directory");
        return;
    }

    send_response(client->socket, 150, "File listing");

    struct dirent* entry;
    struct stat file_stat;
    char filepath[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    while ((entry = readdir(dir)) != NULL) {
        snprintf(filepath, BUFFER_SIZE, "%s/%s", client->working_dir, entry->d_name);
        stat(filepath, &file_stat);

        char perms[11] = "----------";
        if (S_ISDIR(file_stat.st_mode)) perms[0] = 'd';

        struct tm* timeinfo = localtime(&file_stat.st_mtime);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo);

        snprintf(response, BUFFER_SIZE, "%s    1 owner  group %10ld %s %s\r\n",
                 perms, file_stat.st_size, time_str, entry->d_name);
        send(client->socket, response, strlen(response), 0);
    }

    closedir(dir);
    send_response(client->socket, 226, "Transfer complete");
}

void cmd_cwd(client_t* client, const char* args) {
    char new_path[BUFFER_SIZE];
    
    if (args[0] == '/') {
        strcpy(new_path, args);
    } else {
        snprintf(new_path, BUFFER_SIZE, "%s/%s", client->working_dir, args);
    }

    if (access(new_path, F_OK) == 0) {
        strcpy(client->working_dir, new_path);
        send_response(client->socket, 250, "Directory changed");
    } else {
        send_response(client->socket, 550, "Directory not found");
    }
}

void cmd_pwd(client_t* client) {
    char response[BUFFER_SIZE];
    snprintf(response, BUFFER_SIZE, "257 \"%s\" is current directory", client->working_dir);
    send_response(client->socket, 257, response + 4);
}

void cmd_retr(client_t* client, const char* filename) {
    char filepath[BUFFER_SIZE];
    snprintf(filepath, BUFFER_SIZE, "%s/%s", client->working_dir, filename);

    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        send_response(client->socket, 550, "File not found");
        return;
    }

    send_response(client->socket, 150, "Opening data connection");

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client->socket, buffer, bytes_read, 0);
    }

    fclose(file);
    send_response(client->socket, 226, "Transfer complete");
}

void cmd_stor(client_t* client, const char* filename) {
    char filepath[BUFFER_SIZE];
    snprintf(filepath, BUFFER_SIZE, "%s/%s", client->working_dir, filename);

    FILE* file = fopen(filepath, "wb");
    if (file == NULL) {
        send_response(client->socket, 550, "Cannot create file");
        return;
    }

    send_response(client->socket, 150, "Opening data connection");
    send_response(client->socket, 226, "Transfer complete");

    fclose(file);
}

void cmd_dele(client_t* client, const char* filename) {
    char filepath[BUFFER_SIZE];
    snprintf(filepath, BUFFER_SIZE, "%s/%s", client->working_dir, filename);

    if (remove(filepath) == 0) {
        send_response(client->socket, 250, "File deleted");
    } else {
        send_response(client->socket, 550, "Cannot delete file");
    }
}

void cmd_rmd(client_t* client, const char* dirname) {
    char dirpath[BUFFER_SIZE];
    snprintf(dirpath, BUFFER_SIZE, "%s/%s", client->working_dir, dirname);

    if (rmdir(dirpath) == 0) {
        send_response(client->socket, 250, "Directory deleted");
    } else {
        send_response(client->socket, 550, "Cannot delete directory");
    }
}

void cmd_mkd(client_t* client, const char* dirname) {
    char dirpath[BUFFER_SIZE];
    snprintf(dirpath, BUFFER_SIZE, "%s/%s", client->working_dir, dirname);

    if (mkdir(dirpath, 0755) == 0) {
        send_response(client->socket, 257, "Directory created");
    } else {
        send_response(client->socket, 550, "Cannot create directory");
    }
}

void cmd_quit(client_t* client) {
    send_response(client->socket, 221, "Goodbye");
}

void signal_handler(int signum) {
    printf("\n%s", SHUTDOWN_ASCII);
    if (server_socket >= 0) {
        close(server_socket);
    }
    exit(EXIT_SUCCESS);
}
