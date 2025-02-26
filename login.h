#ifndef LOGIN_HEADER
#define LOGIN_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"
#include "size_define.h"

struct User* check_user_exist(char* buffer);
struct User* get_credentials(char* buffer_pt);
int find_user(struct User* user_to_find);

#endif
