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
struct Session {
    int session_id; // L'ID della sessione (generato randomicamente)
    char* username;
    struct Match* match_play; // Punta al match che sta giocando
    struct Session* next; // Puntatore alla sessione successiva
    struct Session* prev; // Puntatore alla sessione precedente
};

// Struttura che rappresenta una partita iniziata/conclusa
struct Match {
    int match_id; // L'ID del match (generato randomicamente)
    char* player_1; // Username dello sfidante
    char* player_2; // Username dello sfidato
    char  status; // (0 | 1 | 2) 0 -> terminata | 1 -> in corso | 2 -> in attesa | 3 -> validazione
    char result; // (0 | 1 | 2) 0 -> Parità | 1 -> Vince player_1 | 2 -> Vince player_2
    struct Match* next;
    struct Match* prev;
};

#endif
