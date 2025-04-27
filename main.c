#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "conn_conf.h"

// Funzione wrapper per la gestione del thread
void* client_thread(void* arg) {
    int client_fd = *(int*)arg; // Cast a int* e deferenziazione del puntatore arg
    free(arg); // Libera memoria allocata
    handle_client(client_fd); // Gestisce la connessione con il client
    close(client_fd); // Chiudi la connessione con il client
    return NULL;
}

int main() {
    int server_fd = init_tcp_server(PORT);

    // Gestione delle connessioni in ingresso
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    int client_fd;
    while (1) {
        // Accetta una connessione in ingresso
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len)) < 0) {
            perror("Accept failed");
            continue; // Continua a gestire altre connessioni
        }

        printf("Connessione accettata da %s:%d\n",
        inet_ntoa(client_address.sin_addr),
        ntohs(client_address.sin_port));

        // Copia il descrittore della variabile locale client_fd in modo da non modificare il contesto dei thread già in esecuzione
        int* t_client_fd = (int*)malloc(sizeof(int));
        if(!t_client_fd) {
            perror("Copia contesto (client_fd) fallita");
            close(client_fd);
            continue;
        }
        *t_client_fd = client_fd;

        // Creiamo il nuovo thread
        pthread_t tid;
        if(pthread_create(&tid, NULL, client_thread, t_client_fd) != 0) {
            perror("Creazione thread fallita");
            close(client_fd);
            free(t_client_fd);
            continue;
        }

        pthread_detach(tid); // Il thread si libera da solo alla fine
    }

    close(server_fd); // Chiudi la socket del server
    return 0;
}
