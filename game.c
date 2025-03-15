#include "game.h"

struct Match* matches = NULL;

int save_game(struct Match* match) {
    FILE* fp = fopen("partite.txt", "a+"); // Apre il file in modalità append

    if(fp == NULL) { // Controlla che il file sia stato aperto correttamente
        perror("Errore nell'apertura del file partite.txt\n");
        return -1;
    }

    // Salva la partita sul file
    fprintf(fp, "%s,%s,%c\n", match -> player_1, match -> player_2, match -> result);

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
    char* result = NULL;
    while(fscanf(fp, "%s", buffer) != EOF) {
        if(strstr(buffer, username)) { // Controlla che l'utente sia presente
            player_1 = strtok(buffer, ",");
            player_2 = strtok(NULL, ",");
            result = strtok(NULL, ",");
            match_played = add_new_match(match_played, create_match_node(player_1, result[0]), false);
            match_played -> player_2 = strdup(player_2);
        }
    }

    free(buffer);
    return match_played;
}

// Tutti metodi per la creazione e gestione di liste
struct Match* create_match_node(char* player_1, char result) {
    struct Match* new_match = (struct Match*)malloc(sizeof(struct Match));

    if(!new_match) { // Non ha allocato la memoria
        perror("Impossibile allocare memoria per un nuovo match\n");
        return NULL;
    }

    new_match -> result = result;
    new_match -> status = '2';
    new_match -> step = '\0';
    if(player_1)
        new_match -> player_1 = strdup(player_1);
    else {
        perror("Player_1 non valido (NULL)\n");
        free(new_match);
        return NULL;
    }
    new_match -> player_2 = NULL;
    new_match -> next = NULL;
    new_match -> prev = NULL;

    return new_match;
}

struct Match* add_new_match(struct Match* match_list, struct Match* new_match, bool check) {
    if(!new_match) // Il nuovo match è NULL
        return match_list; // Ritorna la lista non modificata

    srand(time(NULL)); // Inizializza un seme per permettere di generare numeri pseudo-casuali

    // Assegna un id solo alle partite appena create e non a quelle che erano state conservate su file
    if(check) {
        new_match -> match_id = rand();
        while(find_match_by_id(match_list, new_match -> match_id))
            new_match -> match_id = rand();
    }

    new_match -> next = match_list;
    if(match_list)
        match_list -> prev = new_match;
    return new_match;
}

struct Match* find_match_by_id(struct Match* match_list, int match_id) {
    struct Match* find = NULL;

    while(match_list != NULL) {
        if(match_list -> match_id == match_id) {
            find = match_list;
            break;
        }
        match_list = match_list -> next;
    }

    return find;
}

struct Match* free_match_node(struct Match* match_list, struct Match* match) {
    if(match) {
        if(match -> prev)
            match -> prev -> next = match -> next;
        else
            match_list = match -> next;
        if(match -> next)
            match -> next -> prev = match -> prev;
        free(match -> player_1);
        free(match -> player_2);
        free(match);
    }

    return match_list;
}

void free_match_list(struct Match* match_list) {
    struct Match* tmp = match_list;

    while(tmp != NULL) {
        match_list = tmp -> next;
        free(tmp -> player_1);
        free(tmp -> player_2);
        free(tmp);
        tmp = match_list;
    }
}

int match_list_len(struct Match* match_list) {
    int count = 0;

    while(match_list != NULL) {
        count++;
        match_list = match_list -> next;
    }

    return count;
}

void print_match_list(struct Match* match_list) {
    while(match_list) {
        printf("%d -> [%s, %s, %c, %c]\n", match_list -> match_id, match_list -> player_1, match_list -> player_2 ? match_list -> player_2 : "NULL", match_list -> status, match_list -> result);
        match_list = match_list -> next;
    }
}
