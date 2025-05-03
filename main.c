#include <stdio.h>
#include <stdlib.h>
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
    init_tcp_server(PORT);

    pthread_t shtdwn_tid;
    if(pthread_create(&shtdwn_tid, NULL, keylogger, NULL) != 0) {
        perror("Errore nella creazione del thread per la gestione della sequenza (exit) di terminazione");
        close(server_fd);
        exit(1);
    }
    pthread_detach(shtdwn_tid); // Il thread si libera da solo

    // Gestione delle connessioni in ingresso
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    int client_fd;
    while (!stop_server) { // stop_server viene impostato a 1 quando viene premuta la sequenza exit
        // Accetta una connessione in ingresso
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len)) < 0) {
            if (stop_server) // Interrompe il ciclo quando volatile sig_atomic_t stop_server = 1 cioè quando viene richiesto lo shutdown con la sequenza 'exit'
                break;
            perror("Accept fallito");
            continue; // Continua a gestire altre connessioni
        }

        printf("Connessione accettata da %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

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

        pthread_detach(tid);
    }

    free_resources();

    return 0;
}
