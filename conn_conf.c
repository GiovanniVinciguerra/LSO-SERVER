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

    // Controlla se la richiesta è una POST per il signing
    if (strncmp(buffer, "POST /signin", 12) == 0) {
        char* body_pt = find_body(buffer);
        if(body_pt == NULL) {
            perror("Body non trovato");
            return;
        }

        struct user* new_user = get_user(body_pt);
        int save_check = save(new_user);
        // Risposta al client usando save_check
        char* response = NULL;
        if(save_check == 0)
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nUtente registrato";
        else if(save_check == 1)
            response = "HTTP/1.1 409 Conflict\r\nContent-Type: text/plain\r\n\r\nUtente già registrato";
        else
            response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nUtente non registrato";

        write(client_fd, response, strlen(response));
    } else if (strncmp(buffer, "GET /login", 10) == 0) {
        char* body_pt = find_body(buffer);
        if(body_pt == NULL) {
            perror("Body non trovato");
            return;
        }

        struct user* find_user = check_user_exist(body_pt);
        char* response = NULL;
        if(find_user != NULL) {
            response = (char*)malloc(sizeof(char) * BUFFER_SIZE);
            strcat(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n{");
            strcat(response, "\n\"nome\":\"");
            strcat(response, find_user -> name);
            strcat(response, "\",\n");
            strcat(response, "\"surname\":\"");
            strcat(response, find_user -> surname);
            strcat(response, "\",\n");
            strcat(response, "\n\"email\":\"");
            strcat(response, find_user -> email);
            strcat(response, "\"\n}");
            free(response);
        } else
            response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nUtente non trovato";

        write(client_fd, response, strlen(response));
    } else {
        // Risposta per richieste non riconosciute
        char *response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRisorsa non trovata";
        write(client_fd, response, strlen(response));
    }
}

char* find_body(char* buffer) {
    char* body_pt = NULL;
    body_pt = strstr(buffer, "{");

    return body_pt;
}
