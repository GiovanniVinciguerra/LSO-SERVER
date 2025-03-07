#include "signin.h"

int save(struct User* new_user) {
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
