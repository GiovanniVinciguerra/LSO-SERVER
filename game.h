#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "size_define.h"
#include "structure.h"

extern struct Match* matches; // Permette la gestione  delle eventuali partite attive
char rand_seed();
int save_game(struct Match* match);
struct Match* get_save_matches_by_username(char* username); // Ottiene tutte le partite giocate (terminate) da un certo utente
struct Match* get_matches_by_username(char* username);
// Funzioni utili per gestire liste (allocazione, aggiunta e deallocazione)
struct Match* copy_match_node(struct Match* node);
struct Match* create_match_node(char* player_1, char result);
struct Match* add_new_match(struct Match* match_list, struct Match* new_match, bool check);
struct Match* find_match_by_id(struct Match* match_list, int match_id);
struct Match* free_match_node(struct Match* match_list, struct Match* match);
void free_match_list(struct Match* match_list);
int match_list_len(struct Match* match_list);
void print_match_list(struct Match* match_list);
