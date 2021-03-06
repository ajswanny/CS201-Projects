//
//  shmem.aswanso2.c
//  CS201Assignment4
//
//  Created by Alexander Swanson on 3/8/19.
//  Copyright © 2019 aswanso2. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#define BUFFER_SIZE 32


int main(int argc, char *argv[]) {

    char* words[] = {"now", "is", "the", "time", "for", "all", "good", "men"};
    char *ptr;
    int numWords = 8;
    int pid;
    int memId;

    memId = shmget(memId, BUFFER_SIZE, IPC_CREAT | 0666);
    if (memId < 0) { printf("shmget() failed\n"); return(8); }

    pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        return(8);
    }

    if (pid > 0) {

        // This is the parent.
        printf("I am the parent, and my pid is %d\n", getpid());

        ptr = (char *) shmat(memId, 0, 0);
        if (ptr == NULL) {
            printf("shmat() failed\n");
            return(8);
        }

        int i = 0;
        while (i <numWords) {
            strcpy(ptr, words[i]);
            printf("(P) writing '%s' to shared memory\n", ptr);
            usleep(500000);
            printf("(P) read '%s' from the shared memory\n", ptr);
            i++;
        }

        shmdt(ptr);
        shmctl(memId, IPC_RMID, NULL);
        return (0);

    } else {

        // This is the child.
        pid = getpid();
        printf("I am the child, and my pid is %d\n", pid);

        ptr = (char *) shmat(memId, 0, 0);
        if (ptr == NULL) {
            printf("shmat() in child failed\n");
            return(8);
        }

        int j = 0;
        while (j < numWords) {
            j++;

            printf("(C) read this from shared memory: '%s'\n", ptr);

            // Conversion
            char *p = ptr;
            while (*p) {
                *p = toupper(*p);
                p++;
            }

            printf("(C) write this to the shared memory: '%s'\n", ptr);

            usleep(1000000);


        }

        shmdt(ptr);
        shmctl(memId, IPC_RMID, NULL);
        return (0);

    }

    /**
     * Everything in the program seems to work correctly. The interaction between the parent and the child is correct,
     * for the most part. The only anomaly is that the child does not seem to be able to read three words from the list,
     * despite the fact that the parent can read and write them. Although I am unsure, my best guess about the reason
     * for this anomaly is that the operating system is shuffling contents of memory locations as a result of other
     * running programs. I do not believe I would change any of the structure of my program... However, I would conduct
     * more research in order to learn what I could do to force the Operating System to allow me constant access to
     * memory locations I instantiate data in -- if this is indeed the problem. Furthermore, I have found that on a
     * Linux system (as opposed to a Mac OS system), after the parent reads and writes the last word in the list, four
     * more instances of the child's program sequence run. These are very likely the instructions that were intended to
     * run as the program was executing but were surely postponed by the operating system.
     */

}
