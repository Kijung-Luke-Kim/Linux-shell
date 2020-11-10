#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void init_shell() {
    printf("Kim Kijung [ID: 201604031]\n");
    printf("Simple Shell Construction for Assignment.\n\n");
}

void printPrompt() {
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	printf("[%s@%s]$ ", cwd, getenv("USER"));
}

char *getLineString(){
    char *line = NULL;
    ssize_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

char **lineParser(char *line) {
    int bufsize = 64, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " \t\r\n\a");
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;

    return tokens;
}

int processExecuter(char **args) {
    pid_t pid, wpid;
    int status;

    // Built-in Commands
    if (args[0] == NULL) {
        return 1; // Empty Command
    }
    if (strcmp(args[0], "exit") == 0) {
        return 0; // Exit Command
    }
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "Lacks file or directory argument to \"cd\"\n");
        }
        else {
            if (chdir(args[1]) != 0) {
                perror("No such file or directory\n");
            }
        }
        return 1; // cd Command
    }

    if ((pid = fork()) == 0) { //child
        execv(args[0], args);
        fprintf(stderr, "Cannot execute %s\n", args[0]);
        exit(0);
    }
    else if (pid < 0) {
        perror("Failed to create process\n");
    }
    else { //if background
        //wait for the death of the foreground process
        while (pid != wait(0));
        //if any background did exit(), it is processed here as as side effect.
    }
    return 1;
}

int main() {
	char *line;
	char **args;
	int status;

	init_shell();

	do {
		//print prompt
		printPrompt();
		//get a line string
		line = getLineString();
		//parse the line;
		args = lineParser(line);
        status = processExecuter(args);

        free(line);
        free(args);
    } while (status);
	return EXIT_SUCCESS;
}
