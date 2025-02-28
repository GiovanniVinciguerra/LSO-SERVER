#include "session.h"

struct Session* sessions = NULL;

// Tutti metodi per la creazione e gestione di liste
struct Session* create_session_node(char* username, struct Match* match_play) {
    struct Session* new_session = (struct Session*)malloc(sizeof(struct Session));
    if (!new_session) {
        perror("Impossibile allocare memoria per un nuova sessione\n");
        return NULL;
    }

    srand(time(NULL));

    new_session -> session_id = rand();
    while(find_session_by_id(sessions, new_session -> session_id))
        new_session -> session_id = rand();

    if(username)
        new_session -> username = strdup(username);
    else {
        perror("Username non valido (NULL)\n");
        free(new_session);
        return NULL;
    }

    new_session -> match_play = match_play;
    new_session -> next = NULL;
    new_session -> prev = NULL;

    return new_session;
}

struct Session* add_session(struct Session* session_list, struct Session* new_session) {
    if(!new_session)
        return session_list;

    new_session -> next = session_list;
    session_list -> prev = new_session;

    return new_session;
}

struct Session* find_session_by_id(struct Session* session_list, int session_id) {
    struct Session* find = NULL;

    while(session_list != NULL) {
        if(session_list -> session_id == session_id) {
            find = session_list;
            break;
        }
        session_list = session_list -> next;
    }

    return find;
}

struct Session* remove_session_node(struct Session* session_list, int session_id) {
    // Dai un'occhiata Elia che qui non mi convince l'eliminazione fatta in questo modo
    struct Session* find_delete = find_session_by_id(session_list, session_id);

    if(find_delete) {
        if(find_delete -> prev)
            find_delete -> prev -> next = find_delete -> next;
        if(find_delete -> next)
            find_delete -> next -> prev = find_delete -> prev;
        free(find_delete -> username);
        free_match_node(find_delete -> match_play, find_delete -> match_play -> match_id);
        free(find_delete);
    }

    return session_list;
}

void free_session_list(struct Session* session_list) {
    // TODO
}



//restituisco un intero per capire se l'utente è correttamente loggato, in caso positivo in conn_conf controllo che le stringhe puntate sono correttamente valorizzate
// Modificare
/*int check_session_id(char* buffer_pt, char* username_sfidante, char* username_sfidato) {
    char* session_id = (char*)malloc(sizeof(char) * SESSION_ID_LENGTH);
    username_sfidante = (char*)malloc(sizeof(char) * USERNAME_SIZE);
    username_sfidato = (char*)malloc(sizeof(char) * USERNAME_SIZE);
    char* temp_buff = (char*)malloc(sizeof(char) * USERNAME_SIZE);
    int copy_check = 0, byte_copy = 0, choice = 0;

    while(*buffer_pt != '}') {
        if(*buffer_pt == ':' && *(buffer_pt + 1) == '"') {
            copy_check = 1;
            buffer_pt+=2;
        } else if(copy_check == 0) {
            buffer_pt++;
            continue;
        }

        if(copy_check == 1) {
            if(*buffer_pt == '"' && (*(buffer_pt + 1) == ',' || choice == 2)) {
                copy_check = 0;
                buffer_pt+=2;
                temp_buff[byte_copy] = '\0';
                switch(choice) {
                    case 0:
                        strcpy(session_id, temp_buff);
                        break;
                    case 1:
                        strcpy(username_sfidante, temp_buff);
                        break;
                    case 2:
                        strcpy(username_sfidato, temp_buff);
                        break;
                }
                byte_copy = 0;
                choice++;
            } else {
                temp_buff[byte_copy] = *buffer_pt;
                buffer_pt++;
                byte_copy++;
            }
        }
    }

    Session* session =  find_session(session_list, session_id);

    if(session == NULL){
        free(temp_buff);
        return -1;
    } else {
        free(temp_buff);
        return 0;
    }
}*/

