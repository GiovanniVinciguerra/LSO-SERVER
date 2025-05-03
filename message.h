#ifndef MESSAGE_HEADER
#define MESSAGE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "structure.h"
#include "size_define.h"

extern struct Circular_Queue crl_q;

void init();
bool isFull();
bool isEmpty();
void enqueue(char label, char* body);
void dequeue();
void free_messages();

#endif
