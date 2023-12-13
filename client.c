#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char command[1024];
    char buffer[1024];

    // Crear socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Cambiar a la IP de tu servidor
    server_addr.sin_port = htons(PORT);

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar al servidor");
        exit(EXIT_FAILURE);
    }

    printf("Conectado al servidor en %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    // Enviar comandos al servidor
    while (1) {
        printf("Ingrese un comando (o 'exit' para salir): ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0'; // Eliminar el carácter de nueva línea

        if (strcmp(command, "exit") == 0) {
            break;
        }

        send(client_socket, command, strlen(command), 0);

        // Recibir y mostrar la salida del comando
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break; // Si no hay datos, salir del bucle
        }

        printf("Respuesta del servidor:\n%s\n", buffer);
    }

    // Cerrar conexión
    close(client_socket);

    return 0;
}
