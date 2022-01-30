#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>

bool hasprefix(const char *s, const char *prefix) {
	return strncmp(s, prefix, strlen(prefix)) == 0;
}

void removenewline(char *s) {
	size_t len = strlen(s);
	if (*(s + len - 1) == '\n') {
		*(s + len - 1) = '\0';
	}
}

void parseargs(char *string, char **args) {
	assert(args != NULL);

	size_t i = 0;
	char *saveptr = NULL;
	char *buf = strtok_r(string, " ", &saveptr);

	while (buf != NULL) {
		args[i] = malloc(strlen(buf));
		strcpy(args[i], buf);
		++i;

		buf = strtok_r(NULL, " ", &saveptr);
	}
}

void freeargs(char **args) {
	assert(args != NULL);
	for (size_t i = 0; args[i] != NULL; ++i) {
		free(args[i]);
	}
}

void callcommand(char **args) {
	assert(args != NULL);

	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		if (execvp(args[0], &args[1]) == -1) {
			perror(NULL);
			exit(1);
		}
	}

	if (waitpid(pid, NULL, 0) == -1) {
		perror("waitpid");
		exit(1);
	}
}

const char *cd = "cd ";
const char *pwd = "pwd";
const char *exitcommand = "exit";

int main(int argc, char *argv[]) {
	while (true) {
		char buf[PATH_MAX] = {0};
		if (read(STDIN_FILENO, buf, sizeof(buf)) <= 0) {
			return 0;
		}
		removenewline(buf);

		if (hasprefix(buf, cd)) {
			char *path = buf + strlen(cd);
			if (chdir(path)) {
				perror("chdir");
				return 1;
			}
		} else if (strcmp(buf, pwd) == 0) {
			char cwd[PATH_MAX];
			getcwd(cwd, sizeof(cwd));
			printf("%s\n", cwd);
		} else if (strcmp(buf, exitcommand) == 0) {
			return 0;
		} else {
			char *args[100] = {0}; // TODO avoid overflow by allocating
			parseargs(buf, args);
			callcommand(args);
			freeargs(args);
		}

		fflush(stdout);
	}
}