/*************************************************************************** */
// Project: CPE2600 Lab 13 Final Project
// File: receive.c
// Author: Vincent Vassallo
// Section: 111
// Instruction: enter make and run on separate WSL/Ubuntu Terminal
/*************************************************************************** */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>

#define NUM_THREADS 4

int SN;   
int CC;
int WCM;
int SU;
int GB;
int PB;
int check = 0;
char *usernames[100];
int vote;

void handler(int signo, siginfo_t *info, void *context) {
    check = 1;
    printf("Received Signal\n");
}

void *getvotes(void *arg) {
    int good = 0;
    check = 0;
    sigset_t set;
    int sig;

    // Initialize the signal set and add SIGUSR1 to it
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    printf("Awaiting Signal...\n");
    // Wait for SIGUSR1
    sigwait(&set, &sig);

    // Call the signal handler manually
    
    handler(sig, NULL, NULL);

    int shm_fd = shm_open("/my_shm", O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    void *shm_ptr = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    //Set vote into a separate char to atoi
    char *temp = shm_ptr;
    char vote = temp[0];
    char username[20];
    for(int i = 1; i < strlen(temp); i++){
        username[i-1] = temp[i];
    }

    if (strcmp(username, "") == 0) {
        exit(1); // Ensure voter and username are not NULL
    }

    for (int j = 0; j < 100; j++) {
        if(usernames[j] != NULL && strcmp(usernames[j], username) == 0) {
            // Username previously voted: vote discarded
            if (munmap(shm_ptr, 1024) == -1) {
                perror("munmap");
                exit(1);
            }
            close(shm_fd);
            return NULL;
        }
    }
    for (int j = 0; j < 100; j++) {
        if(usernames[j] == NULL) {
            usernames[j] = strdup(username); // Allocate memory for username
            good = 1;
            break;
        }
    }
    if(good == 1) {
        check = 1;
        // Add the vote to the total depending on what it is
        switch (vote) {
            case '1' : SN++; break;
            case '2' : CC++; break;
            case '3' : WCM++; break;
            case '4' : SU++; break;
            case '5' : GB++; break;
            case '6' : PB++; break;
        }
    }
    if (munmap(shm_ptr, 1024) == -1) {
        perror("munmap");
        exit(1);
    }
    close(shm_fd);
    return NULL;
}

int main(int argc, char *argv[]) { 
    char ans;
    pthread_t threads[NUM_THREADS];
    sigset_t set;

    // Block SIGUSR1 in the main thread and all threads created by it
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    for (int i = 0; i < 100; i++) {
        usernames[i] = NULL;
    }
    printf("PID = %d\n", getpid());

    while(1) {
        printf("Begin Receiving? 'y' = yes 'n' = quit\n");
        scanf(" %c", &ans);
        if(ans != 'y') {
            printf("Vote Results:\n");
            printf("Snickerdoodle: %d votes\n", SN);
            printf("Chocolate Chip: %d votes\n", CC);
            printf("White Chocolate Macadamia: %d votes\n", WCM);
            printf("Sugar: %d votes\n", SU);
            printf("Gingerbread: %d votes\n", GB);
            printf("Peanut Butter: %d votes\n", PB);      
            break;
        }
        for(int i = 0; i < NUM_THREADS; i++) {
            if(pthread_create(&threads[i], NULL, getvotes, NULL) != 0) {
                perror("pthread_create failure");
                exit(1);
            }
        }
        for(int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }
    }  
    return 0;
}