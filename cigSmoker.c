// cigSmoker.c
//
// Lab 2
// Operating Systems Spring 2025

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

// semaphores
sem_t agentSem;
sem_t tobacco, paper, match;
sem_t tobaccoSem, paperSem, matchSem;
sem_t mutex;

// ingredient flags
bool isTobacco = false, isPaper = false, isMatch = false;

// agent thread
void* agent_func(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 6; i++) {
        usleep(rand() % 200000);
        sem_wait(&agentSem);

        switch (id) {
            case 0:
                printf("Agent: placing tobacco and paper\n");
                sem_post(&tobacco);
                sem_post(&paper);
                break;
            case 1:
                printf("Agent: placing paper and match\n");
                sem_post(&paper);
                sem_post(&match);
                break;
            case 2:
                printf("Agent: placing match and tobacco\n");
                sem_post(&match);
                sem_post(&tobacco);
                break;
        }
    }
    printf("Agent %d done\n", id);
    return NULL;
}

// pusher threads
void* pusher_tobacco(void* arg) {
    for (int i = 0; i < 12; i++) {
        sem_wait(&tobacco);
        sem_wait(&mutex);
        if (isPaper) {
            isPaper = false;
            sem_post(&matchSem);
        } else if (isMatch) {
            isMatch = false;
            sem_post(&paperSem);
        } else {
            isTobacco = true;
        }
        sem_post(&mutex);
    }
    printf("Pusher (tobacco) done\n");
    return NULL;
}

void* pusher_paper(void* arg) {
    for (int i = 0; i < 12; i++) {
        sem_wait(&paper);
        sem_wait(&mutex);
        if (isTobacco) {
            isTobacco = false;
            sem_post(&matchSem);
        } else if (isMatch) {
            isMatch = false;
            sem_post(&tobaccoSem);
        } else {
            isPaper = true;
        }
        sem_post(&mutex);
    }
    printf("Pusher (paper) done\n");
    return NULL;
}

void* pusher_match(void* arg) {
    for (int i = 0; i < 12; i++) {
        sem_wait(&match);
        sem_wait(&mutex);
        if (isTobacco) {
            isTobacco = false;
            sem_post(&paperSem);
        } else if (isPaper) {
            isPaper = false;
            sem_post(&tobaccoSem);
        } else {
            isMatch = true;
        }
        sem_post(&mutex);
    }
    printf("Pusher (match) done\n");
    return NULL;
}

// smoker threads
void* smoker_tobacco(void* arg) {
    for (int i = 0; i < 3; i++) {
        sem_wait(&paperSem);  // smoker with tobacco waits for paper + match
        printf("Smoker with tobacco is making a cigarette\n");
        usleep(rand() % 50000);
        printf("Smoker with tobacco is smoking\n");
        usleep(rand() % 50000);
        sem_post(&agentSem);
    }
    printf("Smoker with tobacco done\n");
    return NULL;
}

void* smoker_paper(void* arg) {
    for (int i = 0; i < 3; i++) {
        sem_wait(&matchSem);  // smoker with paper waits for match + tobacco
        printf("Smoker with paper is making a cigarette\n");
        usleep(rand() % 50000);
        printf("Smoker with paper is smoking\n");
        usleep(rand() % 50000);
        sem_post(&agentSem);
    }
    printf("Smoker with paper done\n");
    return NULL;
}

void* smoker_match(void* arg) {
    for (int i = 0; i < 3; i++) {
        sem_wait(&tobaccoSem);  // smoker with match waits for tobacco + paper
        printf("Smoker with match is making a cigarette\n");
        usleep(rand() % 50000);
        printf("Smoker with match is smoking\n");
        usleep(rand() % 50000);
        sem_post(&agentSem);
    }
    printf("Smoker with match done\n");
    return NULL;
}

int main() {
    srand(time(NULL));

    sem_init(&agentSem, 0, 1);
    sem_init(&tobacco, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&match, 0, 0);
    sem_init(&tobaccoSem, 0, 0);
    sem_init(&paperSem, 0, 0);
    sem_init(&matchSem, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_t agents[3], pushers[3], smokers[6];
    int agent_ids[3] = {0, 1, 2};

    pthread_create(&agents[0], NULL, agent_func, &agent_ids[0]);
    pthread_create(&agents[1], NULL, agent_func, &agent_ids[1]);
    pthread_create(&agents[2], NULL, agent_func, &agent_ids[2]);

    pthread_create(&pushers[0], NULL, pusher_tobacco, NULL);
    pthread_create(&pushers[1], NULL, pusher_paper, NULL);
    pthread_create(&pushers[2], NULL, pusher_match, NULL);

    pthread_create(&smokers[0], NULL, smoker_tobacco, NULL);
    pthread_create(&smokers[1], NULL, smoker_tobacco, NULL);
    pthread_create(&smokers[2], NULL, smoker_paper, NULL);
    pthread_create(&smokers[3], NULL, smoker_paper, NULL);
    pthread_create(&smokers[4], NULL, smoker_match, NULL);
    pthread_create(&smokers[5], NULL, smoker_match, NULL);

    for (int i = 0; i < 3; i++) pthread_join(agents[i], NULL);
    for (int i = 0; i < 3; i++) pthread_join(pushers[i], NULL);
    for (int i = 0; i < 6; i++) pthread_join(smokers[i], NULL);

    return 0;
}
