#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <dirent.h>
#include <stdbool.h>

#define MAX_CMD_ARG 64

#define SHNAME "simplesh"

// Call this to exit from a fatal error
static void fatal(const char *str)
{
	perror(str);
	exit(1);
}

/*
 * makelist() will split the input string to list array
 * and return the number of items in the list array.
 */
static int makelist(char *input, const char *delimiters, char **list, int max)
{
	int tokens = 0;

	// XXX
	// Split input to list with strtok()
	// tokens++;
	char* p = strtok(input, delimiters);
	list[0] = p;
	tokens += 1;
	while (p != NULL) {
		p = strtok(NULL, delimiters);
		list[tokens] = p;
		tokens += 1;
	}
	tokens -= 1;
	if(tokens > max){
		fatal("too many arguments");
	}

	return tokens;
}

/*
 * Check /proc/self/fd for file-descriptor leaks.
 * If this function prints something, it means some file-descriptors are left opened.
 * This function runs on exit().
 */
static void check_fd(void)
{
	struct dirent *p;
	pid_t pid;
	char pidpath[32], path[PATH_MAX], buf[PATH_MAX];
	DIR *d;

	pid = getpid();
	sprintf(pidpath, "/proc/%d/fd", pid);
	d = opendir(pidpath);

	if (!d)
		return;

	while ((p = readdir(d))) {
		// Skip the names "." and ".." as we don't want to recurse on them
		if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
			continue;

		// Get real path
		sprintf(path, "%s/%s", pidpath, p->d_name);

		// Skip stdin, stdout, stderr
		if (strcmp(p->d_name, "0") == 0)
			continue;
		if (strcmp(p->d_name, "1") == 0)
			continue;
		if (strcmp(p->d_name, "2") == 0)
			continue;

		// All /proc/*/fd files are supposed to be symlinks
		if (p->d_type != DT_LNK)
			continue;

		// Initialize buf as readlink doesn't set EOF
		memset(buf, 0, PATH_MAX);
		readlink(path, buf, PATH_MAX);
		// Skip printing when this fd is check_fd() itself
		if (strcmp(buf, pidpath))
			printf("fd %s: %s\n", p->d_name, buf);
	}

	closedir(d);
}

int main(int argc, char **argv)
{
	char input[BUFSIZ]; // Input from the user
	char *input_arr[MAX_CMD_ARG]; // input split to string array
	char cwd[PATH_MAX]; // Current working directory

	int i, tokens;
	int flags; // open() flags
	bool eof;
	int child_status;

	// Ignore Ctrl+Z (stop process)
	signal(SIGTSTP, SIG_IGN);
	// Allow forked processes to write to the terminal
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	// Re-ape child processes automatically (zombie processes)
	signal(SIGCHLD, SIG_IGN);

	// Check for file-descriptor leaks on exit()
	atexit(check_fd);

	while (1) {
		// XXX
		// Set cwd here
		// using getcwd function
		if(getcwd(cwd, PATH_MAX) == NULL){
			fatal("current working directory error");
		} else {
			getcwd(cwd, PATH_MAX);
		}
		printf(SHNAME ":%s$ ", cwd);

		// Initialize input by inserting NULL(EOF) to input[0]
		input[0] = '\0';
		while (!(eof = feof(stdin)) && !fgets(input, BUFSIZ, stdin));
		if (eof) {
			// Ctrl+D EOF
			printf("\n");
			exit(0);
		}

		input[strlen(input) - 1] = '\0'; // Replace '\n' at the end with NULL(EOF)

		// Parse input
		tokens = makelist(input, " \t", input_arr, MAX_CMD_ARG);

		// XXX
		// Implement built-in cd
		if(strcmp(input_arr[0], "cd") == 0){
			if(tokens > 2){
				printf("simplesh: cd: too many arguments\n");
			}
			else if(tokens == 1){
				printf("simplesh: cd: no arguments\n");
			}
			else {
				if(chdir(input_arr[1]) == -1){
						char message[PATH_MAX + 14] = "simplesh: cd: ";
						strcat(message, input_arr[1]);
						perror(message);
				}
			}
		}

		// XXX
		// Implement built-in exit
		else if(strcmp(input_arr[0], "exit") == 0){
			if(tokens > 2){
				printf("simplesh: exit: too many arguments\n");
			}
			else {
				if(tokens == 1){
					exit(0);
				}
				else {
					exit(atoi(input_arr[1]));
				}
			}
		}
		// XXX
		// Implement built-in pwd
		else if(strcmp(input_arr[0], "pwd") == 0){
			if(tokens > 1){
				printf("simplesh: pwd: too many arguments\n");
			}
			else {
				printf("%s\n", cwd);
			}
		}
		/*
		 * Implement command execution.
		 * Implement redirection.
		 */
		// XXX
		// command execution
		else {
			int mode = 0; // no redirection
			int direct_index;
			char* before_direct[MAX_CMD_ARG];
			for(int i = 1; i < tokens; i++){
				if(strcmp(input_arr[i], ">") == 0){
					mode = 1; // truncate
					direct_index = i;
					for(int j = 0; j < i; j++){
						before_direct[j] = input_arr[j];
					}
					before_direct[i] = NULL;
				}
				else if(strcmp(input_arr[i], ">>") == 0){
					mode = 2; // append
					direct_index = i;
					for(int j = 0; j < i; j++){
						before_direct[j] = input_arr[j];
					}
					before_direct[i] = NULL;
				}
			}
			if(mode == 0){ // no redirection
				if(fork() == 0){
					if(execvp(input_arr[0], input_arr) == -1){
						fatal("Failed to exec");
					}
					exit(0);
				}
				else {
					wait(&child_status);
				}
			}
			else if(mode == 1){ // truncate
				if(fork() == 0){
					flags = open((input_arr[direct_index + 1]), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
					if(flags == -1){
						fatal("Failed to open for stdout redirection");
					}
					dup2(flags, 1);
					close(flags);
					if(execvp(before_direct[0], before_direct) == -1){
						fatal("Failed to open for stdout redirection");
					}
					exit(0);
				}
				else {
					wait(&child_status);
				}
			}
			else if(mode == 2){ // append
				if(fork() == 0){
					flags = open((input_arr[direct_index + 1]), O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
					if(flags == -1){
						fatal("Failed to open for stdout redirection");
					}
					dup2(flags, 1);
					close(flags);
					if(execvp(before_direct[0], before_direct) == -1){
						fatal("Failed to open for stdout redirection");
					}
					exit(0);
				}
				else {
					wait(&child_status);
				}
			}
		}
	}

	return 0;
}
