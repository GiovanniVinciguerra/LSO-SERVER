#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "session.h"
#include "size_define.h"

Session* session_list = NULL;

//consigliato da chat gpt, l'ho capito fino ad un certo punto
char* genera_session_id() {
    unsigned char buffer[SESSION_ID_LENGTH];

    // Apro /dev/urandom per ottenere numeri casuali sicuri
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        perror("Errore nell'aprire /dev/urandom");
        return NULL;
    }

    // Leggo 32 byte da /dev/urandom
    ssize_t n = read(fd, buffer, SESSION_ID_LENGTH);
    close(fd);
    if (n != SESSION_ID_LENGTH) {
        perror("Errore nella lettura dei dati casuali");
        return NULL;
    }

    // Alloco una stringa per la rappresentazione esadecimale
    char* session_id = malloc(SESSION_ID_LENGTH * 2 + 1);
    if (!session_id) {
        perror("Errore nell'allocazione della memoria");
        return NULL;
    }

    // Converto ogni byte in 2 caratteri esadecimali
    for (int i = 0; i < SESSION_ID_LENGTH; i++) {
        sprintf(&session_id[i * 2], "%02x", buffer[i]);
    }
    session_id[SESSION_ID_LENGTH * 2] = '\0';

    return session_id;
}

// Tutti metodi per la creazione e gestione di liste
Session* create_session_node(const char* session_id) {
    Session* new_node = (Session*) malloc(sizeof(Session));
    if (!new_node) {
        return NULL;
    }
    new_node->session_id = strdup(session_id); //Questo metodo fa una copia del contenuto puntanto da session_id, quindi una volta utilizzato può essere fatta la free di session_id
    new_node->next = NULL;
    return new_node;
}

void add_session(Session** head, const char* session_id) {
    Session* new_node = create_session_node(session_id);
    if (!new_node)
        return;
    new_node->next = *head;
    *head = new_node;
}

Session* find_session(Session* head, const char* session_id) {
    while (head != NULL) {
        if (strcmp(head->session_id, session_id) == 0) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

void remove_session(Session** head, const char* session_id) {
    Session* current = *head;
    Session* prev = NULL;
    while (current != NULL) {
        if (strcmp(current->session_id, session_id) == 0) {
            if (prev == NULL) {
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->session_id);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void free_sessions(Session* head) {
    while (head != NULL) {
        Session* temp = head;
        head = head->next;
        free(temp->session_id);
        free(temp);
    }
}

//restituisco un intero per capire se l'utente è correttamente loggato, in caso positivo in conn_conf controllo che le stringhe puntate sono correttamente valorizzate
int check_session_id(char* buffer_pt, char* username_sfidante, char* username_sfidato) {
    char* session_id = (char*)malloc(sizeof(char) * SESSION_ID_LENGTH);
    username_sfidante = (char*)malloc(sizeof(char) * USERNAME_SIZE);
    username_sfidato = (char*)malloc(sizeof(char) * USERNAME_SIZE);
    char* temp_buff = (char*)malloc(sizeof(char) * USERNAME_SIZE);
    int copy_check = 0, byte_copy = 0, choice = 0;

    while(*buffer_pt != '}') {
        if(*buffer_pt == ':' && *(buffer_pt + 1) == '"') {
            copy_check = 1;
            buffer_pt+=2;
        } else if(copy_check == 0) {
            buffer_pt++;
            continue;
        }

        if(copy_check == 1) {
            if(*buffer_pt == '"' && (*(buffer_pt + 1) == ',' || choice == 2)) {
                copy_check = 0;
                buffer_pt+=2;
                temp_buff[byte_copy] = '\0';
                switch(choice) {
                    case 0:
                        strcpy(session_id, temp_buff);
                        break;
                    case 1:
                        strcpy(username_sfidante, temp_buff);
                        break;
                    case 2:
                        strcpy(username_sfidato, temp_buff);
                        break;
                }
                byte_copy = 0;
                choice++;
            } else {
                temp_buff[byte_copy] = *buffer_pt;
                buffer_pt++;
                byte_copy++;
            }
        }
    }

    Session* session =  find_session(session_list, session_id);

    if(session == NULL){
        free(temp_buff);
        return -1;
    } else {
        free(temp_buff);
        return 0;
    }
}

