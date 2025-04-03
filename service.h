#ifndef SERVICE_HEADER
#define SERVICE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "message.h"
#include "structure.h"
#include "size_define.h"

char** get_authority_credentials(char* body);
struct User* get_credentials(char* body);
struct User* get_user(char* body);
int get_match_id(char* body);
char get_step(char* body);
char* create_user_json_object(struct User* user, int session_id);
char* create_match_json_array(struct Match* match_list);
char* create_match_json_object(struct Match* match);
char* create_message_json_array();

#endif
