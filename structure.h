#ifndef STRUCTURE_HEADER
#define STRUCTURE_HEADER

#include <time.h>
#include "size_define.h"

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
    struct Session* next; // Puntatore alla sessione successiva
    struct Session* prev; // Puntatore alla sessione precedente
};

// Struttura che rappresenta una partita iniziata/conclusa
struct Match {
    int match_id; // L'ID del match (generato randomicamente)
    char* player_1; // Username dello sfidante
    char seed_1; // Seme del player_1
    char seed_2; // Seme del player_2
    char* player_2; // Username dello sfidato
    /* (0 | 1 | 2 | 3 | 4) 0 -> terminata (winner termina la partita) | 1 -> in corso (giocatore accettato) | 2 -> in attesa (il client non gioca nessun'altra partita)
       | 3 -> validazione (un player fa match) | 4 -> nuova creazione (creazione nodo) */
    char  status;
    char result; // (0 | 1 | 2) 0 -> Parità | 1 -> Vince player_1 | 2 -> Vince player_2
    char** steps; // Rappresenta tutte le mosse effettuate nella partita
    struct Match* next;
    struct Match* prev;
};

struct Message {
    char label; // Contiene lo stato della partita
    char* body; // Contiene il corpo del messaggio
    time_t timestamp; // Contiene il momento in cui è stato generato il messaggio
};

struct Circular_Queue { // Gestisce i messaggi generati dalle azioni degli utenti come una coda circolare
    struct Message* msgs; // Messaggi generati MAX 2048
    int top, bottom;
};

#endif
