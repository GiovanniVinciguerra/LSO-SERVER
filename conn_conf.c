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

    // Gestione delle varie richieste
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

        printf("Signin Response\n%s\n", response);
        write(client_fd, response, strlen(response));
        free_user_node(new_user);
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
            // Allega i restanti dati dell'utente al response da mandare al client
            char* json_string = create_user_json_object(find_user, sessions -> session_id);
            int json_string_len = strlen(json_string);
            response = (char*)malloc(sizeof(char) * (json_string_len + 46));
            response[0] = '\0';
            strcat(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
            strcat(response, json_string);

            printf("Login Response\n%s\n", response);
            write(client_fd, response, strlen(response));
            free(json_string);
            free(response);
        } else {
            response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nUtente non trovato";
            write(client_fd, response, strlen(response));
            printf("Login Response\n%s\n", response);
        }

        free_user_node(find_user);
    } else if(strncmp(buffer, "POST /new-game", 14) == 0){
        char* body_pt = find_body(buffer);
        char* response = NULL;
        if(body_pt == NULL) {
            perror("Body non trovato");
            return;
        }

        char** auth = get_authority_credentials(body_pt);
        if(check_session_exist(auth[1], atoi(auth[0]))) {
            /* Controlla se esiste un match in attesa e se è diverso dal giocatore che fa richiesta (gestione partite multiple)
               altrimenti lo crea (logica del server).
               player_1 == username_richiesta allora nessuna partita in attesa trovata (logica del client).
               player_1 != username_richiesta allora una partita in attesa è stata trovata (logica del client). */
            if(matches && strcasecmp(matches -> player_1, auth[1]) != 0)
                // Il client host della partita potrà sapere se c'è stata una modifica per il suo match in attesa.
                matches -> player_2 = strdup(auth[1]);
            else
                matches = add_new_match(matches, create_match_node(auth[1], '0'), true);

            char* json_string = create_new_game_json_object(matches);
            int json_string_len = strlen(json_string);

            response = (char*)malloc(sizeof(char) * (json_string_len + 46));
            response[0] = '\0';
            strcat(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
            strcat(response, json_string);

            printf("New-Game Response\n%s\n", response);
            write(client_fd, response, strlen(response));
            free(json_string);
            free(response);
        } else {
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\n\r\nUtente non loggato correttamente";
            printf("New-Game Response\n%s\n", response);
            write(client_fd, response, strlen(response));
        }

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else if(strncmp(buffer, "POST /stat", 10) == 0) {
        char* body_pt = find_body(buffer);
        char* response = NULL;
        if(body_pt == NULL) {
            perror("Body non trovato");
            return;
        }

        char** auth = get_authority_credentials(body_pt);
        if(check_session_exist(auth[1], atoi(auth[0]))) {
            struct Match* match_list = get_matches_by_username(auth[1]);
            char* json_string = create_match_json_array(match_list);
            int json_string_len = strlen(json_string);

            response = (char*)malloc(sizeof(char) * (json_string_len + 46));
            response[0] = '\0';
            strcat(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
            strcat(response, json_string);

            printf("Stat Response\n%s\n", response);
            write(client_fd, response, strlen(response));
            free(json_string);
            free(response);
            free_match_list(match_list);
        } else {
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\n\r\nUtente non loggato correttamente";
            printf("Stat Response\n%s\n", response);
            write(client_fd, response, strlen(response));
        }

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else {
        // Risposta per richieste non riconosciute
        char* response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRisorsa non trovata";
        printf("Method Not Allowed\n%s\n", response);
        write(client_fd, response, strlen(response));
    }
}

char* find_body(char* buffer) {
    char* body_pt = NULL;
    body_pt = strstr(buffer, "{");

    return body_pt;
}

void free_user_node(struct User* user) {
    if(!user)
        return;

    free(user -> email);
    free(user -> name);
    free(user -> password);
    free(user -> surname);
    free(user -> username);
    free(user);
}
