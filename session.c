#include "session.h"

struct Session* sessions = NULL;

// Tutti metodi per la creazione e gestione di liste
struct Session* create_session_node(char* username) {
    struct Session* new_session = (struct Session*)malloc(sizeof(struct Session));
    if (!new_session) {
        perror("Impossibile allocare memoria per un nuova sessione\n");
        return NULL;
    }

    srand(time(NULL));

    new_session -> session_id = rand();
    while(find_session_by_id(sessions, new_session -> session_id)) // Controlla l'unicità del session id randomico
        new_session -> session_id = rand();

    if(username)
        new_session -> username = strdup(username);
    else {
        perror("Username non valido (NULL)\n");
        free(new_session);
        return NULL;
    }

    new_session -> next = NULL;
    new_session -> prev = NULL;

    return new_session;
}

struct Session* add_session(struct Session* session_list, struct Session* new_session) {
    if(!new_session)
        return session_list;

    new_session -> next = session_list;
    if(session_list)
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
    struct Session* find_delete = find_session_by_id(session_list, session_id);

    if(find_delete) {
        if(find_delete -> prev)
            find_delete -> prev -> next = find_delete -> next;
        else
            session_list = find_delete -> next;
        if(find_delete -> next)
            find_delete -> next -> prev = find_delete -> prev;
        free(find_delete -> username);
        free(find_delete);
    }

    return session_list;
}

void free_session_list(struct Session* session_list) {
    struct Session* tmp = session_list;

    while(tmp != NULL) {
        session_list = tmp -> next;
        free(tmp -> username);
        free(tmp);
        tmp = session_list;
    }
}

struct Session* check_session_exist(char* username, int session_id) {
    struct Session* find = find_session_by_id(sessions, session_id);

    if(!find || !strcmp(find -> username, username) == 0)
        return NULL;

    return find;
}

int session_list_len(struct Session* session_list) {
    int count = 0;

    while(session_list != NULL) {
        count++;
        session_list = session_list -> next;
    }

    return count;
}
