/*************************************************************************** */
// Project: CPE2600 Lab 13 Final Project
// File: receive.c
// Author: Vincent Vassallo
// Section: 111
// Instruction: enter make and run on separate WSL/Ubuntu Terminal
/*************************************************************************** */
#include "receive.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#define NUM_THREADS 4
int SN;   
int CC;
int WCM;
int SU;
int GB;
int PB;
char *usernames[100];
void handler(int signo, siginfo_t *info, void *context) {
    int good = 0;
    //Get vote from sender
    voterinfo *voter = (voterinfo *) (info->si_value.sival_ptr)
    for (int j = 0; j < 100; j++) {
        if(strcmp(usernames[j], voter->username) == 0) {
            break;
        }
    }
    //Add the username to the array
    for (int j = 0; j < 100; j++) {
        if(strcmp(usernames[j], "") == 0) {
            usernames[j] == voter->username;
            good = 1;
            break;
        }
    }
    if(good == 1) {
        //Add the vote to the total depending on what it is
        switch (voter->answer) {
            case 1 :
                SN++;
                break;
            case 2 :
                CC++;
                break;
            case 3 :
                WCM++;
                break;
            case 4 :
                SU++;
                break;
            case 5 :
                GB++;
                break;
            case 6 :
                PB++;
                break;
        }
    }

    
}

void *getvotes(void *arg) {
    //Create struct to hold data
    struct sigaction sa;
    //Set the struct to the handler
    sa.sa_sigaction = handler;
    //Set the flag to save the data
    sa.sa_flags = SA_SIGINFO;
    //Register for signal
    sigaction(SIGUSR1, &sa, NULL);
    printf("Awaiting Signal");
    pause();
    return NULL;
}
int main(int argc, char *argv[]) { 
    int votes = 0;
    char ans;
    usernames = {""};
    pthread_t threads[NUM_THREADS];
    while(1) {
        //Get receiver operator input
        printf("Begin Receiving? 'y' = yes 'n' = quit\n");
        scanf("%c", &ans);
        if(ans != 'y') {
            //Remove any still running threads
            for(int i = 0; i < NUM_THREADS, i++) {
                pthread_join(threads[i], NULL);
            }
            //Print vote results
            printf("Vote Results:\n");
            printf("Snickerdoodle: %d votes\n", SN);
            printf("Chocolate Chip: %d votes\n", CC);
            printf("White Chocolate Macadamia: %d votes\n", WCM);
            printf("Sugar: %d votes\n", SU);
            printf("Gingerbread: %d votes\n", GB);
            printf("Peanut Butter: %d votes\n", PB);
            break;
        }
        //Create threads
        for(int i = 0; i < NUM_THREADS; i++) {
            if(pthread_create(&threads[i], NULL, getvotes, NULL) != 0) {
                perror("pthread_create failure");
                exit(1);
            }

        }
        //Wait for threads
        for(int i = 0; i < NUM_THREADS, i++) {
            pthread_join(threads[i], NULL);
        }
    }
    
    
}