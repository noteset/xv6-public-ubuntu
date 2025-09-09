#include "user.h"
#include "types.h"
int main(void)
{
    int i;
    int num_children = 5;
    for (i = 0; i < num_children; i++)
    {
        int pid = fork();
        if (pid < 0)
        {
            printf(2,"Fork failed\n");
            exit();
        }
        if (pid == 0)
        {
            printf(1,"Child process %d started with PID %d\n", i + 1, getpid());
            while (1);
            // never exits
        }
    }
    // Parent waits for all children to finish
    //
    /* for (i = 0; i < num_children; i++)
    {
        //
        int wpid = wait();
        //
        printf(1,"Parent: child PID %d finished\n", wpid);
        //
    } */
    exit();
}