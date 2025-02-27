#include "game.h"

struct Match* matches = NULL;

int save_game(struct Match* match) {
    FILE* fp = fopen("partite.txt", "a+"); // Apre il file in modalità append

    if(fp == NULL) { // Controlla che il file sia stato aperto correttamente
        perror("Errore nell'apertura del file partite.txt\n");
        return -1;
    }

    // Salva la partita sul file
    fprintf(fp, "%s,%s,%c\n", match -> player_1, match -> player_2, match -> status);

    // Chiude il file
    fclose(fp);

    return 0;
}

struct Match* get_matches_by_username(char* username) {
    struct Match* match_played = NULL;
    FILE* fp = fopen("partite.txt", "r"); // Apre il file in modalità lettura


    if(fp == NULL) { // Controlla che il file sia stato aperto correttamente
        perror("Errore nell'apertura del file partite.txt\n");
        return NULL;
    }

    char* buffer = (char*)malloc(sizeof(char) * BUFFER_MATCH_SIZE);
    char* player_1 = NULL;
    char* player_2 = NULL;
    char* status = NULL;
    while(fscanf(fp, "%s", buffer) != EOF) {
        if(strstr(buffer, username)) { // Controlla che l'utente sia presente
            player_1 = strtok(buffer, ",");
            player_2 = strtok(NULL, ",");
            status = strtok(NULL, ",");
            match_played = add_new_match(match_played, create_match_node(player_1, player_2, status[0]));
        }
    }

    free(buffer);
    return match_played;
}

// Tutti metodi per la creazione e gestione di liste
struct Match* create_match_node(char* player_1, char* player_2, char status) {
    struct Match* new_match = (struct Match*)malloc(sizeof(struct Match));

    if(!new_match) // Non ha allocato la memoria
        return NULL;

    new_match -> status = status;
    if(player_1 && player_2) {
        new_match -> player_1 = strdup(player_1);
        new_match -> player_2 = strdup(player_2);
    }
    else {
        free(new_match);
        return NULL;
    }
    new_match -> next = NULL;
    new_match -> prev = NULL;

    return new_match;
}

struct Match* add_new_match(struct Match* match_list, struct Match* new_match) {
    if(!new_match) // Il nuovo match è NULL
        return match_list; // Ritorna la lista non modificata

    srand(time(NULL)); // Inizializza un seme per permettere di generare numeri casuali

    // Assegna un id solo alle partite appena create e non a quelle che erano state conservate su file
    if(match_list == matches) {
        new_match -> match_id = rand();
        while(find_match_by_id(match_list, new_match -> match_id))
            new_match -> match_id = rand();
    }

    new_match -> next = match_list;
    match_list -> prev = new_match;
    return new_match;
}

struct Match* find_match_by_id(struct Match* match_list, int id) {
    struct Match* find = NULL;

    while(match_list != NULL) {
        if(match_list -> match_id == id) {
            find = match_list;
            break;
        }
        match_list = match_list -> next;
    }

    return find;
}

void free_match_node(struct Match* match_list, int id) {
    struct Match* find_delete = find_match_by_id(match_list, id);

    if(find_delete) {
        if(find_delete -> prev)
            find_delete -> prev -> next = find_delete -> next;
        if(find_delete -> next)
            find_delete -> next -> prev = find_delete -> prev;
        free(find_delete -> player_1);
        free(find_delete -> player_2);
        free(find_delete);
    }
}

void free_match_list(struct Match* match_list) {
    struct Match* tmp = match_list;

    while(tmp != NULL) {
        match_list = match_list -> next;
        free(tmp -> player_1);
        free(tmp -> player_2);
        free(tmp);
        tmp = match_list;
    }
}
