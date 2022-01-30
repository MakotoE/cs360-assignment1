#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>

bool hasprefix(const char *s, const char *prefix) {
	return strncmp(s, prefix, strlen(prefix)) == 0;
}

void removenewline(char *s) {
	assert(s != NULL);
	size_t len = strlen(s);
	if (*(s + len - 1) == '\n') {
		*(s + len - 1) = '\0';
	}
}

char **parseargs(char *s) {
	assert(s != NULL);

	char **args = calloc(sizeof(char *), 1);
	if (args == NULL) {
		exit(1);
	}

	size_t i = 0;
	char *saveptr = NULL;
	char *buf = strtok_r(s, " ", &saveptr);

	while (buf != NULL) {
		args = realloc(args, sizeof(char *) * (i + 2));
		if (args == NULL) {
			exit(1);
		}

		args[i] = calloc(sizeof(char), strlen(buf) + 1);
		if (args[i] == NULL) {
			exit(1);
		}

		strcpy(args[i], buf);
		args[i + 1] = NULL;
		++i;

		buf = strtok_r(NULL, " ", &saveptr);
	}
	return args;
}

void freeargs(char **args) {
	assert(args != NULL);
	for (size_t i = 0; args[i] != NULL; ++i) {
		free(args[i]);
	}
	free(args);
}

void callcommand(char **args) {
	assert(args != NULL);
	assert(args[0] != NULL);

	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		if (execvp(args[0], args) == -1) {
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

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
	while (true) {
		char buf[PATH_MAX] = {0};
		if (read(STDIN_FILENO, buf, sizeof(buf)) <= 0) {
			return 0;
		}
		removenewline(buf);

		if (buf[0] == '\0') {
		} else if (hasprefix(buf, cd)) {
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
			char **args = parseargs(buf);
			callcommand(args);
			freeargs(args);
		}

		fflush(stdout);
	}
}