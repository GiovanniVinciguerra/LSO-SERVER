#ifndef THREAD_MUTEX_HEADER
#define THREAD_MUTEX_HEADER

#include <pthread.h>

extern pthread_mutex_t sessions_mutex; // Gestisce l'accesso condiviso alle sessioni
extern pthread_mutex_t matches_mutex; // Gestisce l'accesso condiviso ai match
extern pthread_mutex_t messages_mutex; // Gestisce l'accesso condiviso ai messaggi di sistema
extern pthread_mutex_t user_file_mutex; // Gestisce l'accesso condiviso al file di salvataggio degli utenti registrati alla piattaforma
extern pthread_mutex_t match_file_mutex; // Gestisce l'accesso condiviso al file che memorizza le partite terminate dei vari utenti

#endif
