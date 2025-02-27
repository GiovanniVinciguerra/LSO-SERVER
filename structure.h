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

// Struttura che rappresenta una partita iniziata/conclusa
struct Match {
    int match_id; // Identificativo auto incrementale per discriminare i vari match iniziati
    char* player_1; // Username dello sfidante
    char* player_2; // Username dello sfidato
    char status; // (0 | 1 | 2) 0 -> Parità | 1 -> Vince lo sfidante | 2 -> Vince lo sfidato
    struct Match* next;
    struct Match* prev;
};

#endif
