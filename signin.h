#ifndef SIGNIN_HEADER
#define SIGNIN_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structure.h"
#include "size_define.h"

struct User* get_user(char* buffer_pt);
int save(struct User* new_user);

#endif
