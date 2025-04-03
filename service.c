#include "service.h"

char** get_authority_credentials(char* body) {
    cJSON* json = cJSON_Parse(body);

    char** info = (char**)malloc(sizeof(char*) * 2); // Contiene session_id e username estratti dal body della response
    info[0] = strdup(cJSON_GetObjectItemCaseSensitive(json, "session_id") -> valuestring);
    info[1] = strdup(cJSON_GetObjectItemCaseSensitive(json, "username") -> valuestring);

    cJSON_free(json);

    return info;
}

struct User* get_credentials(char* body) {
    cJSON* json = cJSON_Parse(body);

    struct User* user = (struct User*)malloc(sizeof(struct User));
    user -> username = strdup(cJSON_GetObjectItemCaseSensitive(json, "username") -> valuestring);
    user -> password = strdup(cJSON_GetObjectItemCaseSensitive(json, "password") -> valuestring);

    cJSON_free(json);

    return user;
}

struct User* get_user(char* body) {
    cJSON* json = cJSON_Parse(body);

    struct User* new_user = (struct User*)malloc(sizeof(struct User));
    new_user -> name = strdup(cJSON_GetObjectItemCaseSensitive(json, "name") -> valuestring);
    new_user -> surname = strdup(cJSON_GetObjectItemCaseSensitive(json, "surname") -> valuestring);
    new_user -> email = strdup(cJSON_GetObjectItemCaseSensitive(json, "mail") -> valuestring);
    new_user -> username = strdup(cJSON_GetObjectItemCaseSensitive(json, "username") -> valuestring);
    new_user -> password = strdup(cJSON_GetObjectItemCaseSensitive(json, "password") -> valuestring);

    cJSON_free(json);

    return new_user;
}

int get_match_id(char* body) {
    cJSON* json = cJSON_Parse(body);

    char* match_id_string = strdup(cJSON_GetObjectItemCaseSensitive(json, "match_id") -> valuestring);
    int match_id = atoi(match_id_string);

    cJSON_free(json);
    free(match_id_string);

    return match_id;
}

char get_step(char* body) {
    cJSON* json = cJSON_Parse(body);

    char* step_string = strdup(cJSON_GetObjectItemCaseSensitive(json, "step") -> valuestring);
    char step = step_string[0];

    cJSON_free(json);
    free(step_string);

    return step;
}

char* create_user_json_object(struct User* user, int session_id) {
    char* json_string = NULL;

    // Converte INT in STRING
    char* session_id_string = (char*)malloc(sizeof(char) * SESSION_ID_SIZE);
    sprintf(session_id_string, "%d", session_id);

    // Creazione dell'oggetto JSON
    cJSON* json_object = cJSON_CreateObject();

    // Aggiunta dati all'oggetto JSON
    cJSON_AddStringToObject(json_object, "name", user -> name);
    cJSON_AddStringToObject(json_object, "surname", user -> surname);
    cJSON_AddStringToObject(json_object, "email", user -> email);
    cJSON_AddStringToObject(json_object, "session_id", session_id_string);

    // Da oggetto JSON a Stringa
    json_string = cJSON_Print(json_object);

    // Pulizia oggetto JSON
    cJSON_Delete(json_object);
    free(session_id_string);

    return json_string;
}

char* create_match_json_array(struct Match* match_list) {
    char* json_string = NULL;

    cJSON* json_array = cJSON_CreateArray();

    char* result_string = (char*)malloc(sizeof(char) * 2);
    while(match_list != NULL) {
        cJSON* json_object = cJSON_CreateObject();

        cJSON_AddStringToObject(json_object, "player_1", match_list -> player_1);
        cJSON_AddStringToObject(json_object, "player_2", match_list -> player_2);
        sprintf(result_string, "%c", match_list -> result);
        cJSON_AddStringToObject(json_object, "result", result_string);

        cJSON_AddItemToArray(json_array, json_object);
        match_list = match_list -> next;
    }

    json_string = cJSON_Print(json_array);

    cJSON_Delete(json_array);
    free(result_string);

    return json_string;
}

char* create_match_json_object(struct Match* match) {
    char* json_string = NULL;

    if(!match) {
        perror("Match non valido (NULL)\n");
        return json_string;
    }

    char* match_id_string = (char*)malloc(sizeof(char) * MATCH_ID_SIZE);
    sprintf(match_id_string, "%d", match -> match_id);
    char* status_string = (char*)malloc(sizeof(char) * 2);
    sprintf(status_string, "%c", match -> status);
    char* step_string = (char*)malloc(sizeof(char) * 2);
    sprintf(step_string, "%c", match -> step);

    cJSON* json_object = cJSON_CreateObject();

    cJSON_AddStringToObject(json_object, "match_id", match_id_string);
    cJSON_AddStringToObject(json_object, "player_1", match -> player_1);
    cJSON_AddStringToObject(json_object, "player_2", match -> player_2);
    cJSON_AddStringToObject(json_object, "status", status_string);
    cJSON_AddStringToObject(json_object, "step", step_string);

    json_string = cJSON_Print(json_object);

    // Pulizia oggetto JSON
    cJSON_Delete(json_object);
    free(match_id_string);
    free(status_string);
    free(step_string);

    return json_string;
}

char* create_message_json_array() {
    char* json_string = NULL;

    cJSON* json_array = cJSON_CreateArray();

    int top = crl_q.top;
    char* label_string = (char*)malloc(sizeof(char) * 2);
    char* timestamp_string = (char*)malloc(sizeof(char) * 21); // Rappresentazione a 64 bit della data
    while(top != (crl_q.bottom + 1)) { // Scorre la coda circolare dal top al bottom
        cJSON* json_object = cJSON_CreateObject();

        sprintf(label_string, "%c", crl_q.msgs[top].label);
        cJSON_AddStringToObject(json_object, "label", label_string);
        cJSON_AddStringToObject(json_object, "body", crl_q.msgs[top].body);
        sprintf(timestamp_string, "%ld", crl_q.msgs[top].timestamp);
        cJSON_AddStringToObject(json_object, "timestamp", timestamp_string);


        cJSON_AddItemToArray(json_array, json_object);
        top = (top + 1) % MESSAGE_QUEUE_SIZE; // Aggiorna l'indice al successivo elemento
    }

    json_string = cJSON_Print(json_array);

    cJSON_Delete(json_array);
    free(label_string);
    free(timestamp_string);

    return json_string;
}
