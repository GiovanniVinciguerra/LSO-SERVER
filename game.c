#include "game.h"

int save_game(char* username_sfidante, char* username_sfidato) {
    FILE* fp = fopen("partite.txt", "a+"); // Apre il file in modalità append
    char* buffer_read = (char*)malloc(sizeof(char) * BUFFER_READ_SIZE);

    if(fp == NULL) { // Controlla che il file sia stato aperto correttamente
        perror("Errore nell'apertura del file partite.txt\n");
        free(buffer_read);
        return -1;
    }

    // Stampa il nuovo utente sul file
    fprintf(fp, "%s-%s\n", username_sfidante, username_sfidato);

    // Chiude il file
    fclose(fp);
    free(buffer_read);

    return 0;
}
