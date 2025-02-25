#ifndef SESSION_H
#define SESSION_H


// Dichiarazione della funzione genera_session_id
char* genera_session_id(void);

// Struttura per rappresentare una sessione
typedef struct Session {
    char* session_id;         // L'ID della sessione (stringa allocata dinamicamente)
    struct Session* next;     // Puntatore alla sessione successiva
} Session;

// Crea un nuovo nodo di sessione dato un session_id
Session* create_session_node(const char* session_id);

// Aggiunge una nuova sessione all'inizio della lista
void add_session(Session** head, const char* session_id);

// Cerca una sessione nella lista dato il session_id
Session* find_session(Session* head, const char* session_id);

// Rimuove una sessione dalla lista in base al session_id
void remove_session(Session** head, const char* session_id);

// Libera tutta la memoria occupata dalla lista delle sessioni
void free_sessions(Session* head);

int check_session_id(char* buffer_pt, char* username_sfidante, char* username_sfidato);

extern Session* session_list;

#endif // SESSION_H

