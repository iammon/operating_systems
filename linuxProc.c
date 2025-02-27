/**

linuxProc.c
This program serves to help me understand processes

*/

#include <stdio.h> // Standard library for exit()
#include <unistd.h> // Provides fork(), getpid(), getppid()

int main() {
    int pidValue = fork(); // create a new process

    if (pidValue < 0) {
        printf("Fork failed!\n");
        return 1;
    } else if (pidValue == 0) {
        printf("Child Process: PID = %d, Parent PID = %d\n", getpid(), getppid());
    } else {
        printf("Parent Process: PID = %d, Child PID = %d\n", getpid(), pidValue);
    }

    return 0;
}