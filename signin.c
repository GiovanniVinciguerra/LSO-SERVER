#include "signin.h"

struct user* get_user(char* buffer_pt) {
    struct user* new_user = (struct user*)malloc(sizeof(struct user));
    char* temp_buff = (char*)malloc(sizeof(char) * TEMP_BUFFER_SIZE);
    int copy_check = 0, byte_copy = 0, choice = 0;
    printf("Sono qui\n");
    while(*buffer_pt != '}') {
        if(*buffer_pt == ':' && *(buffer_pt + 1) == '"') {
            copy_check = 1;
            buffer_pt+=2;
        } else {
            buffer_pt++;
            continue;
        }

        if(copy_check == 1) {
            if(*buffer_pt == '"' && (*(buffer_pt + 1) == ',' || *(buffer_pt + 1) == '\n')) {
                copy_check = 0;
                buffer_pt+=2;
                temp_buff[byte_copy] = '\0';
                printf("VALUE: %s\n", temp_buff);
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
                        strcpy(new_user -> name, temp_buff);
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
    int check = 0;

    return check;
}
