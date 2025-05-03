#ifndef CONN_CONF_HEADER

#define CONN_CONF_HEADER
#define PORT 8080 // Porta su cui il server ascolta
#define BACKLOG 5 // Numero massimo di connessioni in attesa

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include "signin.h"
#include "login.h"
#include "structure.h"
#include "size_define.h"
#include "session.h"
#include "game.h"
#include "service.h"
#include "message.h"
#include "thread_mutex.h"

extern int server_fd; // Descrittore globale della socket del server
extern volatile sig_atomic_t stop_server; // Flag terminazione del ciclo principale per gestire la terminazione da thread principale

void init_tcp_server();
void handle_client(int client_fd);
char* find_body(char* buffer);
void free_user_node(struct User* user);
void free_resources(); // Si occupa di rilasciare tutte le risorse se la sequenza exit viene captata
void* keylogger(void* arg); // Ascolta la tastiera per intercettare la sequenza exit

#endif
