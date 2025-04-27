#include "thread_mutex.h"

pthread_mutex_t sessions_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t matches_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t messages_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t user_file_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t match_file_mutex = PTHREAD_MUTEX_INITIALIZER;
