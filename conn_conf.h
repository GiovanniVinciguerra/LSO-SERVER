#ifndef CONN_CONF_HEADER

#define CONN_CONF_HEADER
#define PORT 8080 // Porta su cui il server ascolta
#define BACKLOG 5 // Numero massimo di connessioni in attesa

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "signin.h"
#include "login.h"
#include "structure.h"
#include "size_define.h"
#include "session.h"
#include "game.h"
#include "service.h"

int init_tcp_server();
void handle_client(int client_fd);
char* find_body(char* buffer);
void free_user_node(struct User* user);

#endif
