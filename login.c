#include "login.h"

struct User* check_user_exist(char* buffer) {
    struct User* user_check = get_credentials(buffer);
    int check_exist = find_user(user_check);

    if(check_exist == 0)
        return user_check;
    else
        free(user_check);

    return NULL;
}

int find_user(struct User* user_to_find) {
    int check_find = 1;
    FILE* fp = fopen("users.txt", "r");

    if(fp == NULL) {
        perror("Errore nell'apertura del file users.txt\n");
        return -1;
    }

    char* buffer = (char*)malloc(sizeof(char) * BUFFER_READ_SIZE);
    char *name = NULL, *surname = NULL, *email = NULL;
    while(fscanf(fp, "%s", buffer) != EOF) {
        if(strstr(buffer, user_to_find -> username) != NULL)
            if(strstr(buffer, user_to_find -> password) != NULL) {
                name = strtok(buffer, ",");
                user_to_find -> name = name;
                surname = strtok(NULL, ",");
                user_to_find -> surname = surname;
                strtok(NULL, ",");
                email = strtok(NULL, ",");
                user_to_find -> email = email;

                check_find = 0;
                break;
            }
    }

    free(buffer);
    fclose(fp);

    return check_find;
}
