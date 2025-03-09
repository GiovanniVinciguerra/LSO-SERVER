#ifndef SERVICE_HEADER
#define SERVICE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "structure.h"
#include "size_define.h"

char** get_authority_credentials(char* buffer_pt);
struct User* get_credentials(char* buffer_pt);
struct User* get_user(char* buffer_pt);
char* create_user_json_object(struct User* user, int session_id);
char* create_match_json_array(struct Match* match_list);
char* create_new_game_json_object(struct Match* match);

#endif
