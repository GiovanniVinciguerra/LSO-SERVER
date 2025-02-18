#include "conn_conf.h"

int init_tcp_server(int port) {
    int server_fd;
    struct sockaddr_in address;

    // Creazione della socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Impostazione dell'indirizzo e della porta
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Accetta connessioni su tutte le interfacce
    address.sin_port = htons(port); // Convertire la porta in formato di rete

    // Binding della socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Messa in ascolto della socket
    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server in ascolto sulla porta %d\n", port);
    return server_fd; // Restituisce il file descriptor della socket
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Leggi la richiesta dal client
    bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Read failed");
        return;
    }

    // Null-terminare la stringa per la stampa
    buffer[bytes_read] = '\0';
    printf("Richiesta ricevuta:\n%s\n", buffer);

    // Controlla se la richiesta è una POST
    if (strncmp(buffer, "POST /signin", 12) == 0) {
        /* // Lunghezza body presente nella richiesta
        char* content_len = strstr(buffer, "Content-Length:");
        int body_len = 0;

        if(content_len != NULL) {
            content_len += strlen("Content-Length:"); // Sposta il puntatore dopo l'intestazione
            body_len = atoi(content_len); // Ottiene la lunghezza ????
        }

        if(body_len > 0) {
            char* body = (char*)malloc((sizeof(char) * body_len) + 1) // Allocazione del buffer per il body
            if(body == NULL) {
                perror("Allocazione memoria body fallita");
                return;
            }

            int tot_read = 0;
            while(tot_read < body_len) {
                bytes_read = read(client_fd, body + tot_read, body_len - tot_read);
            }
        } */
        char* body_pt = find_body(buffer);
        if(body_pt == NULL) {
            perror("Body non trovato");
            return;
        }

        struct user* new_user = get_user(body_pt);
        int save_check = save(new_user);
        printf("\n%s %s %s %s %s\n", new_user -> name, new_user -> surname, new_user -> username, new_user -> email, new_user -> password);
        // Risposta al client usando save_check
        char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nRicevuto: signin";
        write(client_fd, response, strlen(response));
        //free(buffer);
    } else {
        // Risposta per richieste non riconosciute
        const char *response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRisorsa non trovata";
        write(client_fd, response, strlen(response));
    }
}

char* find_body(char* buffer) {
    char* body_pt = NULL;
    body_pt = strstr(buffer, "{\n");

    return body_pt;
}
