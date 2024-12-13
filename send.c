/*************************************************************************** */
// Project: CPE2600 Lab 13 Final Project
// File: send.c
// Author: Vincent Vassallo
// Section: 111
/*************************************************************************** */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
//Combine vote and username in shared memory
int main(int argc, char *argv[]) {

    int receiver_id = atoi(argv[1]);

    // Making sure the receiver exists
    if (kill(receiver_id, 0) != 0) {
        perror("Receiver id was not acquired or is incorrect");
        printf("Connection cannot be made with receiver\n");
        printf("Please reset...\n");
        exit(1); 
    }

    int shm_fd = shm_open("/my_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(shm_fd, 1024) == -1) {
        perror("ftruncate");
        exit(1);
    }

    void *shm_ptr = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Get voter username
    char temp[100];
    int i = 0;
    printf("Please enter only your voting username:\n");
    while (i == 0) {
        if (fgets(temp, sizeof(temp), stdin) == NULL) {
            printf("Invalid username detected: Please reenter...\n");
        } else {
            i = 1;
        }
    }
    
    // Remove newline character from fgets
    temp[strcspn(temp, "\n")] = '\0';
    char *username = temp;

    // Check for quit
    if (strcmp(username, "quit") == 0) {  
        exit(1);
    }

    
    printf("User: %s may now begin voting\n", username);
    char temp2[20];
    // Display fun voting message
    printf("|*************************************************|\n");
    printf("| For voting today there is a most pressing issue |\n");
    printf("|            What is the best cookie?             |\n");
    printf("|  Vote for the number associated with the cookie |\n");
    printf("|     1 - Snickerdoodle   2 - Chocolate Chip      |\n");
    printf("|       3 - White Choc Macadamia  4 - Sugar       |\n");
    printf("|        5 - Gingerbread  6 - Peanut Butter       |\n");
    printf("|                7 - Cornbread                    |\n");
    printf("|*************************************************|\n");
    // Get vote
    printf("Please enter the number of your vote: \n");
    if (fgets(temp2, 2, stdin) == NULL) {
        printf("Invalid vote detected...\nLogging out voter\n");
    } 
    // Test for cornbread
    if (temp2[0] == '7') {
        printf("Cornbread is not a cookie, it was only added to the ballot due to popular demand\n");
        printf("Logging out: Please consider resigning in and voting for a cookie\n");    
    } else {
        printf("Vote cast: sending to receiver...\n");
        strcat(temp2, username);
        strcpy((char *)shm_ptr, temp2); // Write to shared memory
        if (kill(receiver_id, SIGUSR1) == -1) {
            perror("sigkill\n");
            exit(1);
        }
        printf("Vote sent: Thank you for your time!\n");
        
    }
    if (munmap(shm_ptr, 1024) == -1) {
        perror("munmap");
        exit(1);
    }
    close(shm_fd);
    return 0;
}