// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed: Htaw Mon Date: 03/03/2025

// 3460:426 Lab 1 - Basic C shell rev. 9/10/2020

/* Basic shell */

/*
 * This is a very minimal shell. It finds an executable in the
 * PATH, then loads it and executes it (using execv). Since
 * it uses "." (dot) as a separator, it cannot handle file
 * names like "minishell.h"
 *
 * The focus on this exercise is to use fork, PATH variables,
 * and execv. 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>          // PATH_MAX
#include <stdlib.h>          // strsep()
#include <sys/types.h>       // pid_t
#include <sys/wait.h>        // wait()



#define MAX_ARGS		64
#define MAX_ARG_LEN		16
#define MAX_LINE_LEN	80
#define WHITESPACE		" ,\t\n"

struct command_t {
    char *name;
    int argc;
    char *argv[MAX_ARGS];
};

 /* Function prototypes */
 int parseCommand(char *, struct command_t *);
void printPrompt();
void readCommand(char *);

int main(int argc, char *argv[]) {
    int pid;
    int status;
    char cmdLine[MAX_LINE_LEN];
    struct command_t command;

    while (1) {
        printPrompt();
       /* Read the command line and parse it */
        readCommand(cmdLine);
        parseCommand(cmdLine, &command);
        command.argv[command.argc] = NULL;

        /*
            TODO: if the command is one of the shortcuts you're testing for
            either execute it directly or build a new command structure to
            execute next
        */

        /* Create a child process to execute the command */
        if ((pid = fork()) == 0) {
            /* Child executing command */
            execvp(command.name, command.argv);
        }
        /* Wait for the child to terminate */
        wait(&status); /* EDIT THIS LINE */
    }

    /* Shell termination */
    printf("\n\n shell: Terminating successfully\n");
    return 0;
}

/* End basic shell */

/* Parse Command function */

/* Determine command name and construct the parameter list.
  * This function will build argv[] and set the argc value.
  * argc is the number of "tokens" or words on the command line
  * argv[] is an array of strings (pointers to char *). The last
  * element in argv[] must be NULL. As we scan the command line
  * from the left, the first token goes in argv[0], the second in
  * argv[1], and so on. Each time we add a token to argv[],
  * we increment argc.
  */

// parse user command
int parseCommand(char *cLine, struct command_t *cmd) {
    // argument counter
    int argc = 0;

    // pointer to traverse command line
    char *clPtr = cLine;

    char *token;

    while ((token = strsep(&clPtr, WHITESPACE)) != NULL) {
        // skip empty token
        if (*token == '\0') {
            continue;
        }
        // store token in argv
        cmd->argv[argc] = token;
        argc++;
    }

    // make sure argv is NULL terminated
    cmd->argv[argc] = NULL;

    // store argument count and command
    cmd->argc = argc;
    
    // handle empty input
    cmd->name = (argc > 0 ? cmd->argv[0] : NULL);

    return 1;
}

/* End parseCommand function */

/* Print prompt and read command functions - Nutt pp. 79-80 */

void printPrompt() {
    /* Build the prompt string to have the machine name,
     * current directory, or other desired information
     */
    
    // buffer to store current working directory
    char promptString[PATH_MAX];
    
    // get current directory
    if (getcwd(promptString, PATH_MAX) != NULL) {
        //print prompt with current directory 
        printf("linux (hlm21 %s |> ", promptString);
    } else {
        // in case getcwd() fails
        printf("linux (hlm21) |>");
    }
}

 void readCommand(char *buffer) {
    /* This code uses any set of I/O functions, such as those in
     * the stdio library to read the entire command line into
     * the buffer. This implementation is greatly simplified,
     * but it does the job.
     */
    
    // read line of input from user
    if (fgets(buffer, 80, stdin) == NULL) {
        printf("\nError reading input\n");
        return;
    }

    // remove newline if exists
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
}

 /* End printPrompt and readCommand */