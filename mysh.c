#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pwd.h>

bool hasprefix(const char *s, const char *prefix) {
	return strncmp(s, prefix, strlen(prefix)) == 0;
}

void removenewline(char *s) {
	size_t len = strlen(s);
	if (*(s + len - 1) == '\n') {
		*(s + len - 1) = '\0';
	}
}

char **parseargs(char *s) {
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
	for (size_t i = 0; args[i] != NULL; ++i) {
		free(args[i]);
	}
	free(args);
}

void callcommand(char *const *args) {
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

void translatepath(const char path[static PATH_MAX], char out[static PATH_MAX]) {
	char *homedir = getpwuid(getuid())->pw_dir;
	if (hasprefix(path, homedir)) {
		out[0] = '~';
		strcpy(out + 1, path + strlen(homedir));
	} else {
		strcpy(out, path);
	}
}

const char *cd = "cd ";
const char *pwd = "pwd";
const char *exitcommand = "exit";

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
	while (true) {
		char cwd[PATH_MAX];
		getcwd(cwd, sizeof(cwd));

		char directorywithreplacement[PATH_MAX];
		translatepath(cwd, directorywithreplacement);

		printf("%s$mysh> ", directorywithreplacement);
		fflush(stdout);

		char buf[PATH_MAX] = {0};
		if (read(STDIN_FILENO, buf, sizeof(buf)) <= 0) {
			return 0;
		}
		removenewline(buf);

		if (buf[0] == '\0') {
		} else if (hasprefix(buf, cd)) {
			char *path = buf + strlen(cd);
			if (chdir(path)) {
				perror(NULL);
			}
		} else if (strcmp(buf, pwd) == 0) {
			printf("%s\n", cwd);
		} else if (strcmp(buf, exitcommand) == 0) {
			return 0;
		} else {
			char **args = parseargs(buf);
			callcommand(args);
			freeargs(args);
		}
		fflush(stderr);
	}
}