#ifndef SIGNIN_HEADER
#define SIGNIN_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "structure.h"

struct user* get_user(char* buffer);
void save(struct user* new_user);

#endif
