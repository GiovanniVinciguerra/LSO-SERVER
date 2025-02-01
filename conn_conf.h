#ifndef CONN_CONF_HEADER

#define CONN_CONF_HEADER
#define PORT 8080 // Porta su cui il server ascolta
#define BACKLOG 5 // Numero massimo di connessioni in attesa
#define BUFFER_SIZE 1024 // Dimensione del buffer per la richiesta

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int init_tcp_server();
void handle_client(int client_fd);

#endif
