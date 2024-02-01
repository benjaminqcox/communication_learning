#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

// Set the number of children processes to create
#define NUM_CHILDREN 3
#define WAIT_SECONDS 3

void handler(int num)
{
    // Print the child and parent then the signal recieved
    printf("I am child %d of parent %d, and recieved sig %d.\n", getpid(), getppid(), num);
    // Exit with code 1
    exit(EXIT_FAILURE);
}

int main()
{
    pid_t children[3];
    pid_t child;
    // Create a signal handler for the code SIGUSR1
    signal(SIGUSR1, handler);

    // Create NUM_CHILDREN number of children processes
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        child = fork();
        if (child == 0)
        {
            // Print the child id and the parent id
            printf("I am child %d of parent %d.\n", getpid(), getppid());
            if (i == 0)
            {
                // The first child should exit successfully after creation
                printf("I am child %d and am exiting now with code 0\n", getpid());
                exit(EXIT_SUCCESS);
            }
            else
            {
                // Every other child should loop infinitely
                while(1);
            }
        }
        else
        {
            // This is the parent, so add the child to the children array to keep track
            children[i] = child;
        }
    }

    // The parent should be the only process to reach here
    // Sleep the parent for WAIT_SECONDS seconds
    sleep(WAIT_SECONDS);
    // Kill children 1 and 2 (the children stuck in an infinite loop)
    kill(children[1], SIGUSR1);
    kill(children[2], SIGUSR1);

    // Wait for all children to finish
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        wait(&children[i]);
    }
    return 0;
}
