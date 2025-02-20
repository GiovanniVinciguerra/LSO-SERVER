#include "login.h"

struct user* check_user_exist(char* buffer) {
    struct user* user_check = get_credentials(buffer);
    int check_exist = find_user(user_check);

    if(check_exist == 0)
        return user_check;
    else
        free(user_check);

    return NULL;
}

struct user* get_credentials(char* buffer_pt) {
    struct user* user_check = (struct user*)malloc(sizeof(struct user));
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
            if(*buffer_pt == '"' && (*(buffer_pt + 1) == ',' || choice == 1)) {
                copy_check = 0;
                buffer_pt+=2;
                temp_buff[byte_copy] = '\0';
                switch(choice) {
                    case 0:
                        user_check -> username = (char*)malloc(sizeof(char) * byte_copy);
                        strcpy(user_check -> username, temp_buff);
                        break;
                    case 1:
                        user_check -> password = (char*)malloc(sizeof(char) * byte_copy);
                        strcpy(user_check -> password, temp_buff);
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
    return user_check;
}

int find_user(struct user* user_to_find) {
    FILE* fp = fopen("users.txt", "r");
    if(fp == NULL) {
        perror("Errore nell'apertura del file users.txt\n");
        return -1;
    }

    char* buffer = (char*)malloc(sizeof(char) * BUFFER_READ_SIZE);
    char* token = NULL;
    while(fscanf(fp, "%s", buffer) != EOF) {
        if(strstr(buffer, user_to_find -> username) != NULL)
            if(strstr(buffer, user_to_find -> password) != NULL) {
                token = strtok(buffer, ",");
                user_to_find -> name = (char*)malloc(sizeof(char) * (strlen(token) + 1));
                strcpy(user_to_find -> name, token);
                token = strtok(NULL, ",");
                user_to_find -> surname = (char*)malloc(sizeof(char) * (strlen(token) + 1));
                strcpy(user_to_find -> surname, token);
                strtok(NULL, ",");
                token = strtok(NULL, ",");
                user_to_find -> email = (char*)malloc(sizeof(char) * (strlen(token) + 1));
                strcpy(user_to_find -> email, token);
                free(buffer);
                fclose(fp);
                return 0;
            }
    }

    free(buffer);
    fclose(fp);
    return 1;
}
