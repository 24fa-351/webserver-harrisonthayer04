#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define LISTEN_BACKLOG 50

pthread_mutex_t lock;
pthread_t* threads;

void* threadHandleConnection(void* clientFdIn) {
    int client_fd = *(int*)clientFdIn;
    free(clientFdIn);

    char buffer[4096];
    while (1) {
        int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        buffer[bytes_read] = '\0';
        if (bytes_read == 0) break;
        pthread_mutex_lock(&lock);
        printf("Received: %s from client %d\n", buffer, client_fd);
        pthread_mutex_unlock(&lock);
        write(client_fd, buffer, bytes_read);
    }
    close(client_fd);
    pthread_mutex_lock(&lock);
    printf("Connection to client %d has been closed.\n", client_fd);
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in socket_address;
    memset(&socket_address, '\0', sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port = htons(port);
    int returnval;
    returnval = bind(socket_fd, (struct sockaddr*)&socket_address,
                     sizeof(socket_address));
    returnval = listen(socket_fd, LISTEN_BACKLOG);

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);

        int* clientFdPointer = malloc(sizeof(int));
        *clientFdPointer = accept(socket_fd, (struct sockaddr*)&client_address,
                                  &client_address_len);
        pthread_t threadId;
        pthread_create(&threadId, NULL, threadHandleConnection,
                       clientFdPointer);
        pthread_detach(threadId);
    }
    close(socket_fd);
    return 0;
}