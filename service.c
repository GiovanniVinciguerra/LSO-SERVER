#include "service.h"

char** get_authority_credentials(char* buffer_pt) {
    char** info = (char**)malloc(sizeof(char*) * 2);
    info[0] = (char*)malloc(sizeof(char) * SESSION_ID_SIZE);
    info[1] = (char*)malloc(sizeof(char) * USERNAME_SIZE);
    int copy_check = 0, byte_copy = 0, choice = 0;

    // Termina anche se all'interno del body ci sono più di 2 campi. In particolare session_id e username saranno sempre i primi 2 campi.
    while(*buffer_pt != '}' && choice != 2) {
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
                info[1][byte_copy] = '\0';
                if(choice == 0)
                    strcpy(info[0], info[1]);
                byte_copy = 0;
                choice++;
            } else {
                info[1][byte_copy] = *buffer_pt;
                buffer_pt++;
                byte_copy++;
            }
        }
    }

    return info;
}

/*struct User* get_credentials(char* buffer_pt) {
    struct User* user_check = (struct User*)malloc(sizeof(struct User));
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
}*/

struct User* get_credentials(char* buffer_pt) {
    cJSON *json = cJSON_Parse(buffer_pt);
    struct User* user = NULL;
    char* username = NULL;
    char* password = NULL;

    if(json == NULL)
        perror("Body non trovato\n");
    else {
        username = cJSON_GetObjectItemCaseSensitive(json, "username") -> valuestring;
        password = cJSON_GetObjectItemCaseSensitive(json, "password") -> valuestring;
    }

    user = (struct User*)malloc(sizeof(struct User));
    user -> username = strdup(username);
    user -> password = strdup(password);

    cJSON_free(json);

    return user;
}

struct User* get_user(char* buffer_pt) {
    struct User* new_user = (struct User*)malloc(sizeof(struct User));
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

int get_match_id(char* buffer_pt) {
    char* match_id_string = (char*)malloc(sizeof(char) * MATCH_ID_SIZE);
    int check = 0, index = 0;

    while(*buffer_pt != '}') {
        if(*buffer_pt == '"')
            check++;
        else if(check == 11) {
            match_id_string[index] = *buffer_pt;
            index++;
        }

        buffer_pt++;
    }

    match_id_string[index] = '\0';
    int match_id = atoi(match_id_string);

    free(match_id_string);

    return match_id;
}

char get_step(char* buffer_pt) {
    char* step_string = (char*)malloc(sizeof(char) * 2);
    int check = 0;

    while(*buffer_pt != '}') {
        if(*buffer_pt == '"')
            check++;
        else if(check == 15)
            step_string[0] = *buffer_pt;

        buffer_pt++;
    }

    step_string[1] = '\0';
    char step = step_string[0];

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
