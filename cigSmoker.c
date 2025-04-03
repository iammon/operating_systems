// cigSmoker.c
// Htaw Mon
// Lab 2
// Operating Systems Spring 2025
//
// To into an executable: gcc -o cigSmoker cigSmoker.c -lpthread
// To run: ./cigSmoker

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

// Colors
#define COLOR_AGENT   "\033[34m"
#define COLOR_PUSHER  "\033[32m"
#define COLOR_SMOKER  "\033[33m"
#define COLOR_DONE    "\033[35m"

// Semaphores
sem_t agentSem;
sem_t tobacco, paper, match;
sem_t tobaccoSem, paperSem, matchSem;
sem_t mutex;

// Ingredient flags
bool isTobacco = false, isPaper = false, isMatch = false;

// Timestamped, color-coded print
void print_color(const char* color, const char* msg) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    time_t now = ts.tv_sec;
    struct tm* t = localtime(&now);
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%H:%M:%S", t);

    printf("%s[%s.%03ld] %s\033[0m\n", color, timebuf, ts.tv_nsec / 1000000, msg);
}

// Agent thread
void* agent_func(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 6; i++) {
        usleep(rand() % 200000);
        sem_wait(&agentSem);

        switch (id) {
            case 0:
                print_color(COLOR_AGENT, "Agent: placing tobacco and paper");
                sem_post(&tobacco);
                sem_post(&paper);
                break;
            case 1:
                print_color(COLOR_AGENT, "Agent: placing paper and match");
                sem_post(&paper);
                sem_post(&match);
                break;
            case 2:
                print_color(COLOR_AGENT, "Agent: placing match and tobacco");
                sem_post(&match);
                sem_post(&tobacco);
                break;
        }
    }
    char msg[64];
    snprintf(msg, sizeof(msg), "Agent %d done", id);
    print_color(COLOR_DONE, msg);
    return NULL;
}

// Pusher threads
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
    print_color(COLOR_DONE, "Pusher (tobacco) done");
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
    print_color(COLOR_DONE, "Pusher (paper) done");
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
    print_color(COLOR_DONE, "Pusher (match) done");
    return NULL;
}

// Smoker threads
void* smoker_tobacco(void* arg) {
    for (int i = 0; i < 3; i++) {
        sem_wait(&paperSem);
        print_color(COLOR_SMOKER, "Smoker with tobacco is making a cigarette");
        usleep(rand() % 50000);
        print_color(COLOR_SMOKER, "Smoker with tobacco is smoking");
        usleep(rand() % 50000);
        sem_post(&agentSem);
    }
    print_color(COLOR_DONE, "Smoker with tobacco done");
    return NULL;
}

void* smoker_paper(void* arg) {
    for (int i = 0; i < 3; i++) {
        sem_wait(&matchSem);
        print_color(COLOR_SMOKER, "Smoker with paper is making a cigarette");
        usleep(rand() % 50000);
        print_color(COLOR_SMOKER, "Smoker with paper is smoking");
        usleep(rand() % 50000);
        sem_post(&agentSem);
    }
    print_color(COLOR_DONE, "Smoker with paper done");
    return NULL;
}

void* smoker_match(void* arg) {
    for (int i = 0; i < 3; i++) {
        sem_wait(&tobaccoSem);
        print_color(COLOR_SMOKER, "Smoker with match is making a cigarette");
        usleep(rand() % 50000);
        print_color(COLOR_SMOKER, "Smoker with match is smoking");
        usleep(rand() % 50000);
        sem_post(&agentSem);
    }
    print_color(COLOR_DONE, "Smoker with match done");
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
