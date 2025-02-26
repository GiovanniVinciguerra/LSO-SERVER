#ifndef STRUCTURE_HEADER
#define STRUCTURE_HEADER

struct User {
    char* name;
    char* surname;
    char* username;
    char* email;
    char* password;
};

// Struttura per rappresentare una sessione
typedef struct Session {
    char* session_id;         // L'ID della sessione (stringa allocata dinamicamente)
    struct Session* next;     // Puntatore alla sessione successiva
} Session;

// Struttura che rappresenta una partita disputata
struct Match {
    char* username_sfidante;
    char* username_sfidato;
    char winner; // (0 | 1) 0 -> Vince lo sfidante 1 -> Vince lo sfidato
};

// Struttura che rappresenta tutte le partite giocate da un dato giocatore (username)
struct Match_Played {
    struct Match* match;
    struct Match_Played* next;
};

#endif
