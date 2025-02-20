#include "signin.h"

struct user* get_user(char* buffer_pt) {
    struct user* new_user = (struct user*)malloc(sizeof(struct user));
    char* temp_buff = (char*)malloc(sizeof(char) * TEMP_BUFFER_SIZE);
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
            if(*buffer_pt == '"' && (*(buffer_pt + 1) == ',' || choice == 4)) {
                copy_check = 0;
                buffer_pt+=2;
                temp_buff[byte_copy] = '\0';
                switch(choice) {
                    case 0:
                        new_user -> name = (char*)malloc(sizeof(char) * byte_copy);
                        strcpy(new_user -> name, temp_buff);
                        break;
                    case 1:
                        new_user -> surname = (char*)malloc(sizeof(char) * byte_copy);
                        strcpy(new_user -> surname, temp_buff);
                        break;
                    case 2:
                        new_user -> username = (char*)malloc(sizeof(char) * byte_copy);
                        strcpy(new_user -> username, temp_buff);
                        break;
                    case 3:
                        new_user -> email = (char*)malloc(sizeof(char) * byte_copy);
                        strcpy(new_user -> email, temp_buff);
                        break;
                    case 4:
                        new_user -> password = (char*)malloc(sizeof(char) * byte_copy);
                        strcpy(new_user -> password, temp_buff);
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

    free(temp_buff);
    return new_user;
}

int save(struct user* new_user) {
    FILE* fp = fopen("users.txt", "a+"); // Apre il file in modalità append
    char* buffer_read = (char*)malloc(sizeof(char) * BUFFER_READ_SIZE);

    if(fp == NULL) { // Controlla che il file sia stato aperto correttamente
        perror("Errore nell'apertura del file users.txt\n");
        free(buffer_read);
        return -1;
    }

    char* find_pt = NULL;
    while(fscanf(fp, "%s", buffer_read) != EOF) {
        find_pt = strstr(buffer_read, new_user -> username);
        if(find_pt != NULL) {
            fclose(fp);
            free(buffer_read);
            return 1;
        }
    }

    // Stampa il nuovo utente sul file
    fprintf(fp, "%s,%s,%s,%s,%s\n", new_user -> name, new_user -> surname, new_user -> username, new_user -> email, new_user -> password);

    // Chiude il file
    fclose(fp);
    free(buffer_read);

    return 0;
}
