#include <stdio.h>
#include <stdlib.h>
#include <sys/_pthread/_pthread_mutex_t.h>
#include <unistd.h>
#include <pthread.h>

int global_sum = 0;
pthread_mutex_t lock;


void *myThreadFun(void *vargp) {
    for (int i = 0; i < 5; i++) {
        sleep(1);
        printf("Printing from Thread \n");
    }
}

int main() {
    pthread_t thread_id;
    printf("Before Thread\n");

    pthread_create(&thread_id, NULL, myThreadFun, NULL);

    printf("Printing from main \n");

    /*for (int i = 0; i < 5; i++) {
        sleep(1);
        printf("Printing from Thread \n");
    }*/

    pthread_join(thread_id, NULL);


    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    printf("After Thread \n");
    exit(0);
}

void *increase(void *arg) {
    for (int i = 0; i < 20000000; i++) {
        pthread_mutex_lock(&lock);
        global_sum += 1;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *decrease(void *arg) {
    for (int i = 0; i < 20000000; i++) {
        pthread_mutex_lock(&lock);
        global_sum -= 1;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}