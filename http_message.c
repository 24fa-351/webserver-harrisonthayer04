#include "http_message.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

bool is_complete_http_message(char* buffer) {
    if (strlen(buffer) < 10) {
        return false;
    }
    if (strncmp(buffer, "GET ", 4) != 0) {
        return false;
    }
    if (strncmp(buffer + strlen(buffer) - 2, "\n\n", 2) != 0) {
        return false;
    }

    return false;
}

void read_http_client_message(int client_sock, http_client_message_t** msg,
                              http_read_result_t* result) {
    *msg = malloc(sizeof(http_client_message_t));
    char buffer[1024];
    strcpy(buffer, "");

    while (!is_complete_http_message(buffer)) {
        int bytes_read = read(client_sock, buffer + strlen(buffer),
                              sizeof(buffer) - strlen(buffer));
        if (bytes_read == 0) {
            *result = CLOSED_CONNECTION;
            return;
        }
        if (bytes_read < 0) {
            *result = BAD_REQUEST;
            return;
        }
    }
    (*msg)->method = strdup(buffer + 4);
    *result = MESSAGE;
}

void http_client_message_free(http_client_message_t* msg) { free(msg); }
