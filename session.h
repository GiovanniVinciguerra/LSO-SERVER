#ifndef SESSION_HEADER
#define SESSION_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game.h"
#include "structure.h"
#include "size_define.h"

extern struct Session* sessions;

// Funzioni di gestione di liste
struct Session* create_session_node(char* username, struct Match* match_play);
struct Session* add_session(struct Session* session_list, struct Session* new_session);
struct Session* find_session_by_id(struct Session* session_list, int session_id);
struct Session* remove_session_node(struct Session* session_list, int session_id);
void free_session_list(struct Session* session_list);

#endif
