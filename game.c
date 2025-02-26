#include "game.h"

int save_game(struct Match* match) {
    FILE* fp = fopen("partite.txt", "a+"); // Apre il file in modalità append

    if(fp == NULL) { // Controlla che il file sia stato aperto correttamente
        perror("Errore nell'apertura del file partite.txt\n");
        return -1;
    }

    // Salva la partita sul file
    fprintf(fp, "%s,%s,%c\n", match -> username_sfidante, match -> username_sfidato, match -> winner);

    // Chiude il file
    fclose(fp);

    return 0;
}

struct Match_Played* get_matches_by_username(char* username) {
    struct Match_Played* match_played = NULL;
    FILE* fp = fopen("partite.txt", "r"); // Apre il file in modalità lettura


    if(fp == NULL) { // Controlla che il file sia stato aperto correttamente
        perror("Errore nell'apertura del file partite.txt\n");
        return NULL;
    }

    char* buffer = (char*)malloc(sizeof(char) * BUFFER_MATCH_SIZE);
    char* username_sfidante = NULL;
    char* username_sfidato = NULL;
    char* winner = NULL;
    while(fscanf(fp, "%s", buffer) != EOF) {
        if(strstr(buffer, username)) { // Controlla che l'utente sia presente
            username_sfidante = strtok(buffer, ",");
            username_sfidato = strtok(NULL, ",");
            winner = strtok(NULL, ",");
            match_played = add_match_played_node(match_played, create_match_played_node(create_match_node(username_sfidante, username_sfidato, winner[0])));
        }
    }

    free(buffer);
    return match_played;
}

// Tutti metodi per la creazione e gestione di liste e strutture
struct Match* create_match_node(char* username_sfidante, char* username_sfidato, char winner) {
    struct Match* match = (struct Match*)malloc(sizeof(struct Match));

    if(!match)
        return NULL;

    match -> username_sfidante = strdup(username_sfidante);
    match -> username_sfidato = strdup(username_sfidato);
    match -> winner = winner;

    return match;
}

struct Match_Played* create_match_played_node(struct Match* match) {
    struct Match_Played* match_played = (struct Match_Played*)malloc(sizeof(struct Match_Played));

    if(!match_played)
        return NULL;

    match_played -> match = match;
    match_played -> next = NULL;

    return match_played;
}

struct Match_Played* add_match_played_node(struct Match_Played* matches_list, struct Match_Played* match_played) {
    if(!match_played)
        return matches_list;

    match_played -> next = matches_list;
    return match_played;
}

void free_match_node(struct Match* match) {
    if(!match)
        return;

    free(match -> username_sfidante);
    free(match -> username_sfidato);
    free(match);
}

void free_match_played_node(struct Match_Played* match_played) {
    if(!match_played)
        return;

    free_match_node(match_played -> match);
    free(match_played);
}

void free_match_played_list(struct Match_Played* matches_list) {
    if(!matches_list)
        return;

    struct Match_Played* tmp = matches_list;

    while(tmp != NULL) {
        matches_list = matches_list -> next;
        free_match_played_node(tmp);
        tmp = matches_list;
    }
}
