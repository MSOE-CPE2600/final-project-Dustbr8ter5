/*************************************************************************** */
// Project: CPE2600 Lab 13 Final Project
// File: send.c
// Author: Vincent Vassallo
// Section: 111
// Instruction: Enter make and run on separate WSL/Ubuntu Terminal
//              run receive first and put its pid in this program's command 
//              arguments. 
//              Entering "quit" as the username will exit the program, for
//              convenience
/*************************************************************************** */
#include "send.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
//Voting Machine?

int main(int argc, char *argv[]) {
    
    char c;
    int receiver_id;
    struct voterinfo voter;

    //Getting the receivers id from the command line
    while((c = getopt(argc, argv, 'p')) != -1) {
        if(c = 'p') {
            receiver_id = atoi(optargs);
        }
        
    }

    //Making sure the receiver exists
    if(kill(receiver_id, 0) != 0) {
        printf("Receiver id was not acquired or is incorrect\n");
        printf("Connection cannot be made with receiver\n");
        printf("Please reset...\n");
        exit(1); 
    }
    int x = 0;
    while(x = 0) { 
        //Get voter username and put into struct
        char temp[20];
        int i = 0;
        printf("Please enter only your voting username:\n");
        while(i == 0) {
            if(fgets(temp, 20, stdin) == NULL){
                printf("Invalid username detected: Please reenter...\n");
            } else {
                i = 1;
            }
        }
        if(strcmp(temp, "quit") == 0) {
            exit(1);
        }
        voter.username = temp;
        printf("User: %s may now begin voting\n", temp);
        int vote;
        printf("|*************************************************|\n");
        prinft("| For voting today there is a most pressing issue |\n");
        printf("|            What is the best cookie?             |\n");
        printf("|  Vote for the number associated with the cookie |\n");
        printf("|     1 - Snickerdoodle   2 - Chocolate Chip      |\n");
        printf("|       3 - White Choc Macadamia  4 - Sugar       |\n");
        printf("|        5 - Gingerbread  6 - Peanut Butter       |\n");
        printf("|                7 - Cornbread                    |\n");
        printf("|*************************************************|\n");
        printf("Please enter the number of your vote: \n");
        if(fgets(vote, 2, stdin) == NULL) {
            printf("Invalid vote detected...\nLogging out voter");
        } else if(vote == 7) {
            printf("Cornbread is not a cookie, it was only added to the ballot due to popular demand\n");
            printf("Logging out: Please consider resigning in and voting for a cookie\n");    
        } else {
            printf("Vote cast: sending to receiver...");
            union sigval value; 
            voter.answer = vote;
            value.sival_ptr = &voter;
            sigqueue(receiver_id, SIGUSR1, value);
            printf("Vote sent: Thank you for your time!");
        }

    }



    


}
