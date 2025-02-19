#include <stdio.h>
#include <stdlib.h>
#include "conn_conf.h"

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

        // Gestisci la connessione con il client
        handle_client(client_fd);

        close(client_fd); // Chiudi la connessione con il client
    }

    close(server_fd); // Chiudi la socket del server
    return 0;
}
