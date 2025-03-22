#include "message.h"

struct Circular_Queue crl_q;

void init() {
    crl_q.msgs = (struct Message*)malloc(sizeof(struct Message) * MESSAGE_QUEUE_SIZE);
    crl_q.top = -1;
    crl_q.bottom = -1;
}

bool isFull() {
    return ((crl_q.bottom + 1) % MESSAGE_QUEUE_SIZE) == crl_q.top;
}

bool isEmpty() {
    return crl_q.top == -1;
}

void enqueue(char label, char* body) {
    if(!body) // Non aggiunge se il messaggio è vuoto
        return;

    if(!crl_q.msgs)
        init();

    if(isFull())
        dequeue();

    if(isEmpty())
        crl_q.top = 0;

    crl_q.bottom = (crl_q.bottom + 1) % MESSAGE_QUEUE_SIZE;
    crl_q.msgs[crl_q.bottom].label = label;
    crl_q.msgs[crl_q.bottom].body = strdup(body);
    time(&crl_q.msgs[crl_q.bottom].timestamp);
}

void dequeue() {
    if(isEmpty())
        return;

    free(crl_q.msgs[crl_q.top].body);

    if(crl_q.top == crl_q.bottom) { // Esiste un solo elemento
        crl_q.top = -1;
        crl_q.bottom = -1;
    } else
        crl_q.top = (crl_q.top + 1) % MESSAGE_QUEUE_SIZE;
}
