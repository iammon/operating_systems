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

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <strings.h>         // strsep()
#include <unistd.h>
#include <limits.h>          // PATH_MAX
#include <linux/limits.h>    // PATH_MAX
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
    (void)argc;
    (void)argv;
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

        if (command.name == NULL) { // prevent NULL execvp
            fprintf(stderr, "Error: No command entered\n");
            continue;
        }

        // implement "C file1 file2" copy command
        if (strcmp(command.name, "C") == 0) {
            // check for correct usage: must have exactly two file arguments
            if (command.argc != 3) {
                fprintf(stderr, "Usage: C file1 file2\n");
                continue;
            }
        
            // open source file (file1) for reading
            FILE *source = fopen(command.argv[1], "rb");
            if (source == NULL) {
                perror("Error opening source file");
                continue;
            }
        
            // open destination file (file2) for writing
            FILE *dest = fopen(command.argv[2], "wb");
            if (dest == NULL) {
                perror("Error creating destination file");
                fclose(source); // close the source file before returning
                continue;
            }
        
            // copy contents from source to destination
            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
                fwrite(buffer, 1, bytesRead, dest);
            }
        
            // close files
            fclose(source);
            fclose(dest);
        
            printf("File copied successfully: %s -> %s\n", command.argv[1], command.argv[2]);
            continue;
        }

        // implement "D file" delete command
        if (strcmp(command.name, "D") == 0) {
            if (command.argc != 2) {
                fprintf(stderr, "Usage: D file\n");
                continue;
            }

            if (remove(command.argv[1]) == 0) {
                printf("File deleted successfully: %s\n", command.argv[1]);
            } else {
                perror("Error deleting file");
            }
            continue;
        }

        // implement "E comment" echo command
        if (strcmp(command.name, "E") == 0) {
            if (command.argc == 1) {  // No arguments, just issue a new prompt
                continue;
            }

            // Print message, reducing multiple spaces to a single space
            for (int i = 1; i < command.argc; i++) {
                printf("%s", command.argv[i]);
                if (i < command.argc - 1) {
                    printf(" ");  // Add a space between words
                }
            }
            printf("\n");  // Newline at the end
            continue;
        }

        // implement "H" help command
        if (strcmp(command.name, "H") == 0) {
            printf("\n=== Shell Help Menu ===\n");
            printf("C file1 file2  - Copy file1 to file2 without deleting file1\n");
            printf("D file         - Delete the specified file\n");
            printf("E comment      - Echo a message to the screen\n");
            printf("H              - Display this help menu\n");
            printf("L              - List the contents of the current directory\n");
            printf("M file         - Create a new file and open a text editor\n");
            printf("P file         - Print the contents of the specified file\n");
            printf("Q              - Quit the shell\n");
            printf("W              - Clear the screen\n");
            printf("X program      - Execute the specified program\n");
            printf("\n");
            continue;
        }

        // implement "L" list directory command
        if (strcmp(command.name, "L") == 0) {
            pid_t pid = fork();

            if (pid == 0) { // Child process
                char *args[] = {"ls", "-l", NULL}; // Run "ls -l"
                execvp("ls", args);
                perror("execvp failed"); // If exec fails
                exit(1);
            }
            
            // Parent process waits for child
            wait(NULL);
            continue;
        }

        // implement "M file" create and open a text file
        if (strcmp(command.name, "M") == 0) {
            if (command.argc != 2) {
                fprintf(stderr, "Usage: M file\n");
                continue;
            }

            // Fork a new process to open the text editor
            pid_t pid = fork();

            if (pid == 0) { // Child process
                execlp("nano", "nano", command.argv[1], NULL);
                perror("execlp failed"); // If exec fails
                exit(1);
            }
            
            // Parent process waits for child
            wait(NULL);
            continue;
        }

        // implement "P file" print file content
        if (strcmp(command.name, "P") == 0) {
            if (command.argc != 2) {
                fprintf(stderr, "Usage: P file\n");
                continue;
            }

            // Fork a new process to run "more file"
            pid_t pid = fork();

            if (pid == 0) { // Child process
                execlp("more", "more", command.argv[1], NULL);
                perror("execlp failed"); // If exec fails
                exit(1);
            }
            
            // Parent process waits for child
            wait(NULL);
            continue;
        }

        // implement "Q" quit command
        if (strcmp(command.name, "Q") == 0) {
            printf("Exiting shell. Goodbye!\n");
            exit(0);
        }

        // implement "W" clear screen command
        if (strcmp(command.name, "W") == 0) {
            pid_t pid = fork();

            if (pid == 0) { // child process
                execlp("clear", "clear", NULL);
                perror("execlp failed");
                exit(1);
            }
        }

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
    if (cLine == NULL) {
        return 1;
    }

    for (int i = 0; i < MAX_ARGS; i++) {
        cmd->argv[i] = NULL;
    }

    int argc = 0;
    char *clPtr = cLine; 
    char *token;

    while (clPtr != NULL && (token = strsep(&clPtr, WHITESPACE)) != NULL) {
        if (*token == '\0') { 
            continue;
        }

        if (argc >= MAX_ARGS - 1) {
            fprintf(stderr, "Error: Too many arguments\n");
            return 1;
        }

        cmd->argv[argc] = token;
        argc++;
    }

    if (argc > 0) {
        cmd->argv[argc] = NULL;
        cmd->name = cmd->argv[0]; 
    } else {
        cmd->name = NULL;
    }

    cmd->argc = argc;
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
        printf("linux (hlm21) %s |> ", promptString);
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