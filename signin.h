#ifndef SIGNIN_HEADER
#define SIGNIN_HEADER

#define TEMP_BUFFER_SIZE 256
#define BUFFER_READ_SIZE 335

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "structure.h"

struct user* get_user(char* buffer_pt);
int save(struct user* new_user);

#endif
