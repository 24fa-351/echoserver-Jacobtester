#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LISTEN_BACKLOG 5

void handleConnection(int client_fd)
{
    char buffer[1024];
    int bytes_read;
    while ((bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0)
    {
        printf("Received: %s\n", buffer);
        write(client_fd, buffer, bytes_read);
        memset(buffer, 0, sizeof(buffer)); 
    }
    close(client_fd);
}


//Browser & Telnet usage
// ./tcp_echo_server -p 8080 
// Browser: http://localhost:8080
// Telnet: telnet localhost 8080
int main(int argc, char *argv[])
{
    int port = 0;
    int opt;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
        case 'p':
            port = atoi(optarg);
            break;
        default:
            printf("Incorrect usage\n");
            exit(EXIT_FAILURE);
        }
    }

    if (port == 0) {
        fprintf(stderr, "No port provided\n");
        exit(EXIT_FAILURE);
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in socket_address;
    memset(&socket_address, '\0', sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1) {
        perror("bind");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, LISTEN_BACKLOG) == -1) {
        perror("listen");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", port);

    //Server Loop
    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        int client_fd = accept(socket_fd, (struct sockaddr *)&client_address, &client_address_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        handleConnection(client_fd);
    }

    close(socket_fd);
    return 0;
} 
