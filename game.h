#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "size_define.h"
#include "structure.h"

int save_game(struct Match* match);
struct Match_Played* get_matches_by_username(char* username); // Ottiene tutte le partite giocate da un certo utente
// Funzioni utili per gestire liste (allocazione, aggiunta e deallocazione)
struct Match* create_match_node(char* username_sfidante, char* username_sfidato, char winner);
struct Match_Played* create_match_played_node(struct Match* match);
struct Match_Played* add_match_played_node(struct Match_Played* matches, struct Match_Played* match_played);
void free_match_node(struct Match* match);
void free_match_played_node(struct Match_Played* match_played);
void free_match_played_list(struct Match_Played* matches_list);
