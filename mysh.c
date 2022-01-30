// mysh: A shell that can run commands.

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pwd.h>

// Returns true if s has given prefix.
bool hasprefix(const char *s, const char *prefix) {
	return strncmp(s, prefix, strlen(prefix)) == 0;
}

// Removes trailing newline if s ends with newline.
void removenewline(char *s) {
	size_t len = strlen(s);
	if (*(s + len - 1) == '\n') {
		*(s + len - 1) = '\0';
	}
}

// Splits s by space character and returns the resulting array of strings. The s argument gets
// modified.
char **parseargs(char *s) {
	char **args = calloc(sizeof(char *), 1);
	if (args == NULL) {
		exit(EXIT_FAILURE);
	}

	size_t i = 0;
	char *saveptr = NULL;
	char *buf = strtok_r(s, " ", &saveptr);

	while (buf != NULL) {
		args = realloc(args, sizeof(char *) * (i + 2));
		if (args == NULL) {
			exit(EXIT_FAILURE);
		}

		args[i] = calloc(sizeof(char), strlen(buf) + 1);
		if (args[i] == NULL) {
			exit(EXIT_FAILURE);
		}

		strcpy(args[i], buf);
		args[i + 1] = NULL;
		++i;

		buf = strtok_r(NULL, " ", &saveptr);
	}
	return args;
}

// Frees the array of strings.
void freeargs(char **args) {
	for (size_t i = 0; args[i] != NULL; ++i) {
		free(args[i]);
	}
	free(args);
}

// Calls program for given args, where args[0] is the file name.
void callprogram(char *const *args) {
	assert(args != NULL);
	assert(args[0] != NULL);

	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror(NULL);
			exit(EXIT_FAILURE);
		}
	}

	if (waitpid(pid, NULL, 0) == -1) {
		perror("waitpid");
		exit(EXIT_FAILURE);
	}
}

// Replaces home directory with '~' and writes replaced directory to out.
void translatepath(const char path[static PATH_MAX], char out[static PATH_MAX]) {
	char *homedir = getpwuid(getuid())->pw_dir;
	if (hasprefix(path, homedir)) {
		out[0] = '~';
		strcpy(out + 1, path + strlen(homedir));
	} else {
		strcpy(out, path);
	}
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
	const char *cd = "cd ";
	char *line = NULL;
	size_t linesize = 0;

	while (true) {
		char cwd[PATH_MAX] = {0};
		getcwd(cwd, sizeof(cwd));

		char directorywithreplacement[PATH_MAX] = {0};
		translatepath(cwd, directorywithreplacement);

		printf("%s$mysh> ", directorywithreplacement);
		fflush(stdout);

		if (getline(&line, &linesize, stdin) == -1) {
			perror("read");
			break;
		}
		removenewline(line);

		if (line[0] == '\0') {
			// Empty command
		} else if (hasprefix(line, cd)) {
			// cd
			char *path = line + strlen(cd);
			if (chdir(path)) {
				perror(NULL);
			}
		} else if (hasprefix(line, "pwd ")) {
			// pwd
			printf("%s\n", cwd);
		} else if (strcmp(line, "exit") == 0) {
			// exit
			break;
		} else {
			// Call program
			char **args = parseargs(line);
			callprogram(args);
			freeargs(args);
		}
		fflush(stderr);
	}

	free(line);
}