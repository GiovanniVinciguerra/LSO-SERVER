#include "conn_conf.h"

int server_fd = -1;
volatile sig_atomic_t stop_server = 0;

void init_tcp_server(int port) {
    struct sockaddr_in address;

    // Creazione della socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failed");
        exit(1);
    }

    // Impostazione dell'indirizzo e della porta
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Accetta connessioni su tutte le interfacce
    address.sin_port = htons(port); // Converte la porta in formato di rete

    // Permette di ottenere la porta 8080 anche se questa non è libera
    int set = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set)) < 0) {
        perror("setsockopt fallita");
        exit(1);
    }

    // Binding della socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind fallito");
        close(server_fd);
        exit(1);
    }

    // Messa in ascolto della socket
    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Server in ascolto sulla porta %d\n", port);
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Legge la richiesta dal client
    bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Read failed");
        return;
    }

    buffer[bytes_read] = '\0';
    printf("Richiesta ricevuta:\n%s\n", buffer);

    char* body_pt = find_body(buffer);
    if(body_pt == NULL) {
        perror("Body non trovato");
        return;
    }

    // Gestione delle varie richieste
    if (strncmp(buffer, "POST /signin", 12) == 0) {
        struct User* new_user = get_user(body_pt);

        // Gestiamo la concorrenza per l'ultilizzo del file user
        pthread_mutex_lock(&user_file_mutex);
            int save_check = save(new_user);
        pthread_mutex_unlock(&user_file_mutex);

        // Risposta al client usando save_check
        char* response = NULL;
        if(save_check == 0)
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente registrato";
        else if(save_check == 1)
            response = "HTTP/1.1 409 Conflict\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente già registrato";
        else
            response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non registrato";

        printf("Signin Response\n%s\n", response);
        write(client_fd, response, strlen(response));
        free_user_node(new_user);
    } else if (strncmp(buffer, "POST /login", 11) == 0) {
        // Apre il file utenti per la lettura e la riceerca un altro thread potrebbe modificarlo mentre viene letto
        pthread_mutex_lock(&user_file_mutex);
            struct User* find_user = check_user_exist(body_pt);
        pthread_mutex_unlock(&user_file_mutex);

        char* response = NULL;
        if(find_user) {
            // Mentre si cerca una sessione un altro thread potrebbe maneggiare i puntatori next cambiandoli
            pthread_mutex_lock(&sessions_mutex);
                struct Session* find_session = find_session_by_username(sessions, find_user -> username); // Controlla che l'utente non si era già loggato precedentemente
            pthread_mutex_unlock(&sessions_mutex);

            char* json_string = NULL;
            if(find_session) // Se l'utente è stato trovato salta l'inserimento nella lista e assegna lo stesso session_id
                json_string = create_user_json_object(find_user, find_session -> session_id);
            else {
                pthread_mutex_lock(&sessions_mutex);
                    sessions = add_session(sessions, create_session_node(find_user -> username)); // Aggiunge l'utente appena loggato alla sessione assegnando anche un session_id
                pthread_mutex_unlock(&sessions_mutex);

                // Allega i restanti dati dell'utente al response da mandare al client
                json_string = create_user_json_object(find_user, sessions -> session_id);
            }

            response = (char*)malloc(sizeof(char) * (RESPONSE_SIZE + strlen(json_string)));
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %zu\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\n\r\n%s", strlen(json_string), json_string);

            printf("Login Response\n%s\n", response);
            write(client_fd, response, strlen(response));
            free(json_string);
            free(response);
        } else {
            response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non trovato";
            write(client_fd, response, strlen(response));
            printf("Login Response\n%s\n", response);
        }

        free_user_node(find_user);
    } else if(strncmp(buffer, "POST /logout", 12) == 0) {
        char** auth = get_authority_credentials(body_pt);
        int session_id = atoi(auth[0]);
        char* response = NULL;

        pthread_mutex_lock(&sessions_mutex);
            bool check = check_session_exist(auth[1], session_id);
        pthread_mutex_unlock(&sessions_mutex);

        if(check) {
            pthread_mutex_lock(&sessions_mutex);
                sessions = remove_session_node(sessions, session_id);
            pthread_mutex_unlock(&sessions_mutex);

            response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente disconnesso correttamente";
        } else
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non loggato correttamente";

        printf("Logout Response\n%s\n", response);
        write(client_fd, response, strlen(response));

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else if(strncmp(buffer, "POST /new-game", 14) == 0) {
        char** auth = get_authority_credentials(body_pt);
        int session_id = atoi(auth[0]);
        char* response = NULL;

        pthread_mutex_lock(&sessions_mutex);
            bool check = check_session_exist(auth[1], session_id);
        pthread_mutex_unlock(&sessions_mutex);

        if(check) {
            struct Match* tmp = matches;

            // Durante il  matchmaking la lista matches non può assolutamente essere modificata da altri thread
            pthread_mutex_lock(&matches_mutex);
                /* Controlla se esiste un match in attesa (2) e se i player sono diversi dal giocatore che fa richiesta
                   (gestione partite multiple) altrimenti lo crea (logica del server).
                   player_1 == username_richiesta allora nessuna partita in attesa trovata (logica del client).
                   player_1 != username_richiesta allora una partita in attesa è stata trovata (logica del client). */
                while(tmp != NULL && (strcmp(tmp -> player_1, auth[1]) == 0 || tmp -> status != '2'))
                    tmp = tmp -> next;

                if(tmp) {
                    // Il player_1 potrà sapere se c'è stata una modifica per il suo match in attesa e eventualmente accettare la partita.
                    tmp -> player_2 = strdup(auth[1]);
                    tmp -> status = '3';
                } else {
                    matches = add_new_match(matches, create_match_node(auth[1], '0'), true);
                    // Aggiunta messaggio di nuova partita creata alla coda dei messaggi
                    char* message_string = (char*)malloc(sizeof(char) * MESSAGE_BODY_SIZE);
                    sprintf(message_string, "%s ha creato una nuova partita 👾.", matches -> player_1);

                    pthread_mutex_lock(&messages_mutex);
                        enqueue(matches -> status, message_string);
                    pthread_mutex_unlock(&messages_mutex);

                    free(message_string);
                }
            pthread_mutex_unlock(&matches_mutex);

            char* json_string = tmp != NULL ? create_match_json_object(tmp) : create_match_json_object(matches);

            response = (char*)malloc(sizeof(char) * (RESPONSE_SIZE + strlen(json_string)));
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %zu\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\n\r\n%s", strlen(json_string), json_string);

            printf("New-Game Response\n%s\n", response);
            write(client_fd, response, strlen(response));
            free(json_string);
            free(response);
        } else {
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non loggato correttamente";
            printf("New-Game Response\n%s\n", response);
            write(client_fd, response, strlen(response));
        }

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else if(strncmp(buffer, "POST /stat", 10) == 0) {
        char** auth = get_authority_credentials(body_pt);
        int session_id = atoi(auth[0]);
        char* response = NULL;

        pthread_mutex_lock(&sessions_mutex);
            bool check = check_session_exist(auth[1], session_id);
        pthread_mutex_unlock(&sessions_mutex);

        if(check) {
            // Accede al file delle partite salavate per le statistiche degli utenti
            pthread_mutex_lock(&match_file_mutex);
                struct Match* match_list = get_save_matches_by_username(auth[1]);
            pthread_mutex_unlock(&match_file_mutex);

            char* json_string = create_save_match_json_array(match_list);

            response = (char*)malloc(sizeof(char) * (RESPONSE_SIZE + strlen(json_string)));
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %zu\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\n\r\n%s", strlen(json_string), json_string);

            printf("Stat Response\n%s\n", response);
            write(client_fd, response, strlen(response));
            free(json_string);
            free(response);
            free_match_list(match_list);
        } else {
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non loggato correttamente";
            printf("Stat Response\n%s\n", response);
            write(client_fd, response, strlen(response));
        }

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else if(strncmp(buffer, "POST /step", 10) == 0) {
        char** auth = get_authority_credentials(body_pt);
        int session_id = atoi(auth[0]);
        char* response = NULL;

        pthread_mutex_lock(&sessions_mutex);
            bool check = check_session_exist(auth[1], session_id);
        pthread_mutex_unlock(&sessions_mutex);

        if(check) {
            int match_id = get_match_id(body_pt);

            pthread_mutex_lock(&matches_mutex);
                struct Match* match = find_match_by_id(matches, match_id);

                for(int i = 0; i < STEPS_SIZE; i++)
                    if(match -> steps[i] == NULL) {
                        match -> steps[i] = get_step(body_pt);
                        break;
                    }
            pthread_mutex_unlock(&matches_mutex);

            response = "HTTP/1.1 200 Ok\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nMossa memorizzata correttamente";
        } else
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non loggato correttamente";

        printf("Step Response\n%s\n", response);
        write(client_fd, response, strlen(response));

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else if(strncmp(buffer, "POST /update", 12) == 0) {
        char** auth = get_authority_credentials(body_pt);
        int session_id = atoi(auth[0]);
        char* response = NULL;

        pthread_mutex_lock(&sessions_mutex);
            bool check = check_session_exist(auth[1], session_id);
        pthread_mutex_unlock(&sessions_mutex);

        if(check) {
            int match_id = get_match_id(body_pt);

            pthread_mutex_lock(&matches_mutex);
                struct Match* match = find_match_by_id(matches, match_id);
            pthread_mutex_unlock(&matches_mutex);

            char* json_string = create_match_json_object(match);

            // Costruisce la response con tutte le informazioni da mandare al client
            response = (char*)malloc(sizeof(char) * (RESPONSE_SIZE + strlen(json_string)));
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %zu\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\n\r\n%s", strlen(json_string), json_string);

            pthread_mutex_lock(&matches_mutex);
                if(match -> status == '0') // Elimina il match dopo aver fatto l'update per tutti e 2 i client
                    matches = free_match_node(matches, match); // Elimina la partita dalla lista di matches
            pthread_mutex_unlock(&matches_mutex);

            printf("Update Response\n%s\n", response);
            write(client_fd, response, strlen(response));
            free(response);
        } else {
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non loggato correttamente";
            printf("Update Response\n%s\n", response);
            write(client_fd, response, strlen(response));
        }

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else if(strncmp(buffer, "POST /matches", 13) == 0) {
        char** auth = get_authority_credentials(body_pt);
        int session_id = atoi(auth[0]);
        char* response = NULL;

        pthread_mutex_lock(&sessions_mutex);
            bool check = check_session_exist(auth[1], session_id);
        pthread_mutex_unlock(&sessions_mutex);

        if(check) {
            pthread_mutex_lock(&matches_mutex);
                struct Match* user_matches = get_matches_by_username(auth[1]);
            pthread_mutex_unlock(&matches_mutex);

            char* json_string = create_match_json_array(user_matches);

            response = (char*)malloc(sizeof(char) * (RESPONSE_SIZE + strlen(json_string)));
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %zu\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\n\r\n%s", strlen(json_string), json_string);

            free_match_list(user_matches);

            printf("Matches Response\n%s\n", response);
            write(client_fd, response, strlen(response));
            free(response);
        } else {
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non loggato correttamente";
            printf("Matches Response\n%s\n", response);
            write(client_fd, response, strlen(response));
        }

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else if(strncmp(buffer, "POST /winner", 10) == 0) {
        char** auth = get_authority_credentials(body_pt);
        int session_id = atoi(auth[0]);
        char* response = NULL;

        pthread_mutex_lock(&sessions_mutex);
            bool check = check_session_exist(auth[1], session_id);
        pthread_mutex_unlock(&sessions_mutex);

        if(check) {
            int match_id = get_match_id(body_pt);

            pthread_mutex_lock(&matches_mutex);
                struct Match* match = find_match_by_id(matches, match_id);

                // Imposta lo status su terminata (0)
                match -> status = '0';
            pthread_mutex_unlock(&matches_mutex);

            // Aggiunta messaggio di partita terminata alla coda dei messaggi
            char* message_string = (char*)malloc(sizeof(char) * MESSAGE_BODY_SIZE);
            sprintf(message_string, "Si è appena conclusa la partita tra %s e %s. Con la vittoria di ", match -> player_1, match -> player_2);

            pthread_mutex_lock(&matches_mutex);
                if(strcmp(match -> player_1, auth[1]) == 0) { // Imposta il vincitore
                    match -> result = '1';
                    strcat(message_string, match -> player_1);
                } else {
                    match -> result = '2';
                    strcat(message_string, match -> player_2);
                }
            pthread_mutex_unlock(&matches_mutex);

            strcat(message_string, ". Che scontro incredibile 😮");

            pthread_mutex_lock(&messages_mutex);
                enqueue(match -> status, message_string);
            pthread_mutex_unlock(&messages_mutex);

            free(message_string);

            pthread_mutex_lock(&match_file_mutex);
                save_game(match); // Salva la partita
            pthread_mutex_unlock(&match_file_mutex);

            // Costruisce la response
            response = "HTTP/1.1 200 Ok\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nPartita salvata correttamente";
        } else
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non loggato correttamente";

        printf("Winner Response\n%s\n", response);
        write(client_fd, response, strlen(response));

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else if(strncmp(buffer, "POST /tie", 9) == 0) {
        char** auth = get_authority_credentials(body_pt);
        int session_id = atoi(auth[0]);
        char* response = NULL;

        pthread_mutex_lock(&sessions_mutex);
            bool check = check_session_exist(auth[1], session_id);
        pthread_mutex_unlock(&sessions_mutex);

        if(check) {
            int match_id = get_match_id(body_pt);

            pthread_mutex_lock(&matches_mutex);
                struct Match* match = find_match_by_id(matches, match_id);

                match -> status = '0';
                match -> result = '0';
            pthread_mutex_unlock(&matches_mutex);

            char* message_string = (char*)malloc(sizeof(char) * MESSAGE_BODY_SIZE);
            sprintf(message_string, "Si è appena conclusa la partita tra %s e %s con un pareggio. Che scontro incredibile 😮", match -> player_1, match -> player_2);

            pthread_mutex_lock(&messages_mutex);
                enqueue(match -> status, message_string);
            pthread_mutex_unlock(&messages_mutex);

            free(message_string);

            pthread_mutex_lock(&match_file_mutex);
                save_game(match);
            pthread_mutex_unlock(&match_file_mutex);

            response = "HTTP/1.1 200 Ok\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nPartita salvata correttamente";
        } else
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non loggato correttamente";

        printf("Tie Response\n%s\n", response);
        write(client_fd, response, strlen(response));

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else if(strncmp(buffer, "POST /waiting", 13) == 0) {
        char** auth = get_authority_credentials(body_pt);
        int session_id = atoi(auth[0]);
        char* response = NULL;

        pthread_mutex_lock(&sessions_mutex);
            bool check = check_session_exist(auth[1], session_id);
        pthread_mutex_unlock(&sessions_mutex);

        if(check) {
            int match_id = get_match_id(body_pt);

            pthread_mutex_lock(&matches_mutex);
                struct Match* match = find_match_by_id(matches, match_id);

                match -> status = '2';
                // Imposta il seme (O | X) per il giocatore host della partita; l'avversario sarà costretto ad accettare l'altro seme
                if(match -> seed_1 == '\0') {
                    match -> seed_1 = rand_seed();
                    if(match -> seed_1 == 'O')
                        match -> seed_2 = 'X';
                    else
                        match -> seed_2 = 'O';
                }
            pthread_mutex_unlock(&matches_mutex);

            // Aggiunta messaggio di partita in attesa alla coda dei messaggi
            char* message_string = (char*)malloc(sizeof(char) * MESSAGE_BODY_SIZE);
            sprintf(message_string, "La partita di %s è ora in attesa. Partecipate in tanti 😄", match -> player_1);

            pthread_mutex_lock(&messages_mutex);
                enqueue(match -> status, message_string);
            pthread_mutex_unlock(&messages_mutex);

            free(message_string);

            char* json_string = create_seed_json_object(match -> seed_1);

            pthread_mutex_lock(&matches_mutex);
                free(match -> player_2);
                match -> player_2 = NULL;
            pthread_mutex_unlock(&matches_mutex);

            response = (char*)malloc(sizeof(char) * (RESPONSE_SIZE + strlen(json_string)));
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %zu\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\n\r\n%s", strlen(json_string), json_string);

            printf("Waiting Response\n%s\n", response);
            write(client_fd, response, strlen(response));
            free(response);
        } else {
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non loggato correttamente";
            printf("Waiting Response\n%s\n", response);
            write(client_fd, response, strlen(response));
        }

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else if(strncmp(buffer, "POST /progress", 14) == 0) {
        char** auth = get_authority_credentials(body_pt);
        int session_id = atoi(auth[0]);
        char* response = NULL;

        pthread_mutex_lock(&sessions_mutex);
            bool check = check_session_exist(auth[1], session_id);
        pthread_mutex_unlock(&sessions_mutex);

        if(check) {
            int match_id = get_match_id(body_pt);

            pthread_mutex_lock(&matches_mutex);
                struct Match* match = find_match_by_id(matches, match_id);

                match -> status = '1';
            pthread_mutex_unlock(&matches_mutex);

            // Aggiunta messaggio di partita tornata in attesa alla coda dei messaggi
            char* message_string = (char*)malloc(sizeof(char) * MESSAGE_BODY_SIZE);
            sprintf(message_string, "La partita tra %s e %s è ora in corso. Che tensione 😱", match -> player_1, match -> player_2);

            pthread_mutex_lock(&messages_mutex);
                enqueue(match -> status, message_string);
            pthread_mutex_unlock(&messages_mutex);

            free(message_string);

            response = "HTTP/1.1 200 Ok\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nPartita messa in corso";
        } else
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non loggato correttamente";

        printf("Progress Response\n%s\n", response);
        write(client_fd, response, strlen(response));

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else if(strncmp(buffer, "POST /messages", 14) == 0) {
        char** auth = get_authority_credentials(body_pt);
        int session_id = atoi(auth[0]);
        char* response = NULL;

        pthread_mutex_lock(&sessions_mutex);
            bool check = check_session_exist(auth[1], session_id);
        pthread_mutex_unlock(&sessions_mutex);

        if(check) {
            // Scorre la coda dei messaggi un inserimento farebbe cambiare gli indici di inizio e fine
            pthread_mutex_lock(&messages_mutex);
                char* json_string = create_message_json_array();
            pthread_mutex_unlock(&messages_mutex);

            response = (char*)malloc(sizeof(char) * (RESPONSE_SIZE + strlen(json_string)));
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %zu\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\n\r\n%s", strlen(json_string), json_string);

            printf("Messages Response\n%s\n", response);
            write(client_fd, response, strlen(response));
            free(json_string);
            free(response);
        } else {
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nUtente non loggato correttamente";
            printf("Messages Response\n%s\n", response);
            write(client_fd, response, strlen(response));
        }

        free(auth[0]);
        free(auth[1]);
        free(auth);
    } else {
        // Risposta per richieste non riconosciute
        char* response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nAccess-Control-Allow-Origin: *\r\n\r\nRisorsa non trovata";
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

void free_resources() {
    printf("Deallocazione delle sessioni attive...\n");
    free_session_list(sessions);
    sessions = NULL;
    printf("Sessioni deallocate con successo.\n");

    printf("Deallocazione delle partite attive...\n");
    free_match_list(matches);
    matches = NULL;
    printf("Partite deallocate con successo.\n");

    printf("Deallocazioni dei messaggi di sistema...\n");
    free_messages();
    printf("Messaggi di sistema deallocati con successo.\n");

    printf("Tutte le risorse liberate.\nServer spento.\n");
}

void* keylogger(void* arg) {
    char buffer[256]; // Impostata lunghezza maggiore per proteggere da scritture più lunghe che potrebbero andare a scrivere posizioni in memoria non lecite

    while(!stop_server) {
        if(fgets(buffer, sizeof(buffer), stdin)) {
            if(strcmp(buffer, "exit\n") == 0) {
                stop_server = 1;
                printf("Comando 'exit' ricevuto. Spegnimento in corso...\n");

                printf("Chiusura socket server in corso...\n");
                if(server_fd >= 0) {
                    shutdown(server_fd, SHUT_RDWR);
                    close(server_fd); // Chiude la socket del server
                }
                printf("Socket chiusa.\n");
            }
        }
    }

    return NULL;
}
