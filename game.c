#include "game.h"

struct Match* matches = NULL;

char rand_seed() {
    char seed = '\0';

    srand(time(NULL));
    int rand_seed = rand() % 2;

    if(rand_seed == 0)
        seed = 'O';
    else
        seed = 'X';

    return seed;
}

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

struct Match* get_save_matches_by_username(char* username) {
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

struct Match* get_matches_by_username(char* username) {
    struct Match* find_matches = NULL;
    struct Match* tmp = matches;

    while(tmp != NULL) {
        if(strcmp(tmp -> player_1, username) == 0 || (tmp -> player_2 && strcmp(tmp -> player_2, username) == 0))
            find_matches = add_new_match(find_matches, copy_match_node(tmp), false);

        tmp = tmp -> next;
    }

    return find_matches;
}

// Tutti metodi per la creazione e gestione di liste
struct Match* copy_match_node(struct Match* node) {
    struct Match* copy = NULL;

    if(node) {
        copy = (struct Match*)malloc(sizeof(struct Match));
        copy -> steps = (char**)malloc(sizeof(char*) * STEPS_SIZE);

        copy -> match_id = node -> match_id;
        copy -> player_1 = strdup(node -> player_1);
        if(node -> player_2)
            copy -> player_2 = strdup(node -> player_2);
        else
            copy -> player_2 = NULL;
        copy -> result = node -> result;
        copy -> seed_1 = node -> seed_1;
        copy -> seed_2 = node -> seed_2;
        copy -> status = node -> status;
        for(int i = 0; i < STEPS_SIZE; i++)
            if(node -> steps[i])
                copy -> steps[i] = strdup(node -> steps[i]);
            else
                copy -> steps[i] = NULL;
        copy -> prev = NULL;
        copy -> next = NULL;
    }

    return copy;
}

struct Match* create_match_node(char* player_1, char result) {
    struct Match* new_match = (struct Match*)malloc(sizeof(struct Match));

    if(!new_match) { // Non ha allocato la memoria
        perror("Impossibile allocare memoria per un nuovo match\n");
        return NULL;
    }

    new_match -> result = result;
    new_match -> status = '4';
    if(player_1)
        new_match -> player_1 = strdup(player_1);
    else {
        perror("Player_1 non valido (NULL)\n");
        free(new_match);
        return NULL;
    }

    // Alloca l'array di mosse e imposta tutti valori a NULL
    new_match -> steps = (char**)malloc(sizeof(char*) * STEPS_SIZE);
    for(int i = 0; i < STEPS_SIZE; i++)
        new_match -> steps[i] = NULL;

    new_match -> seed_1 = '\0';
    new_match -> seed_2 = '\0';

    new_match -> player_2 = NULL;
    new_match -> next = NULL;
    new_match -> prev = NULL;

    return new_match;
}

struct Match* add_new_match(struct Match* match_list, struct Match* new_match, bool check) {
    if(!new_match) // Il nuovo match è NULL
        return match_list; // Ritorna la lista non modificata

    srand(time(NULL)); // Inizializza un seme per permettere di generare numeri pseudo-casuali

    // Assegna un id solo alle partite appena create e non a quelle che erano state conservate su file oppure recuperate dalla lista di matches
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

        for(int i = 0; i < STEPS_SIZE; i++)
            free(match -> steps[i]);
        free(match -> steps);
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
        for(int i = 0; i < STEPS_SIZE; i++)
            free(tmp -> steps[i]);
        free(tmp -> steps);
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
