#include "signin.h"

struct user* get_user(char* buffer) {
    struct user* new_user = (struct user*)malloc(sizeof(struct user));

    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "key", "value");

    // Ottengo il nome e lo imposto nella struttura user
    cJSON* name = cJSON_GetObjectItem(json, "nome");
    int len = strlen(name->string);
    new_user -> name = (char*)malloc(sizeof(char)*len);
    strcpy(new_user -> name, name -> string);



    return new_user;
}

void save(struct user* new_user) {

}
