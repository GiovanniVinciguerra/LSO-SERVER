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

        struct User* new_user = get_user(body_pt);
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
    } else if (strncmp(buffer, "POST /login", 10) == 0) {
        char* body_pt = find_body(buffer);

        if(body_pt == NULL) {
            perror("Body non trovato");
            return;
        }

        struct User* find_user = check_user_exist(body_pt);
        char* response = NULL;
        if(find_user != NULL) {
            // Aggiunge l'utente appena loggato alla sessione assegnando anche un session_id
            sessions = add_session(sessions, create_session_node(find_user -> username, NULL));

            response = (char*)malloc(sizeof(char) * BUFFER_SIZE);
            response = strcat(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{");
            response = strcat(response, "\n\"nome\":\"");
            response = strcat(response, find_user -> name);
            response = strcat(response, "\",\n");
            response = strcat(response, "\"surname\":\"");
            response = strcat(response, find_user -> surname);
            response = strcat(response, "\",\n");
            response = strcat(response, "\n\"email\":\"");
            response = strcat(response, find_user -> email);
            response = strcat(response, "\",\n");
            char* buffer = (char*)malloc(sizeof(char) * 11);
            sprintf(buffer, "%d", sessions -> session_id);
            response = strcat(response, "\"session_id\":\"");
            response = strcat(response, buffer);
            response = strcat(response, "\"\n}");
            free(buffer);
        } else
            response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nUtente non trovato";

        write(client_fd, response, strlen(response));
        free(response);
    } else if(strncmp(buffer, "POST /new-game", 14)){
        char* body_pt = find_body(buffer);
        char* response = NULL;
        if(body_pt == NULL) {
            perror("Body non trovato");
            return;
        }

        char** info_game = get_info_game(body_pt);
        if(check_session_exist(info_game[1], atoi(info_game[0]))) {
            // TODO Inizia partita
        } else
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\n\r\nUtente non loggato correttamente";
        write(client_fd, response, strlen(response));
    } else if(strncmp(buffer, "POST /stat", 10)) {
        char* body_pt = find_body(buffer);
        char* response = NULL;
        if(body_pt == NULL) {
            perror("Body non trovato");
            return;
        }

        char** info_game = get_info_game(body_pt);
        if(check_session_exist(info_game[1], atoi(info_game[0]))) {
            struct Match* match_list = get_matches_by_username(info_game[1]);
            // TODO CJSON
        } else
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\n\r\nUtente non loggato correttamente";
    } else {
        // Risposta per richieste non riconosciute
        char* response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRisorsa non trovata";
        write(client_fd, response, strlen(response));
    }
}

char* find_body(char* buffer) {
    char* body_pt = NULL;
    body_pt = strstr(buffer, "{");

    return body_pt;
}
