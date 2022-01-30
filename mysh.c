#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <stdbool.h>

bool hasprefix(const char *s, const char *prefix) {
	return strncmp(s, prefix, strlen(prefix)) == 0;
}

void removenewline(char *s) {
	size_t len = strlen(s);
	if (*(s + len - 1) == '\n') {
		*(s + len - 1) = '\0';
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
				printf("%s\n", strerror(errno));
			}
		}

		if (strcmp(buf, pwd) == 0) {
			char cwd[PATH_MAX];
			getcwd(cwd, sizeof(cwd));
			printf("%s\n", cwd);
		}

		if (strcmp(buf, exitcommand) == 0) {
			return 0;
		}
		fflush(stdout);
	}
}