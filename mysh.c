#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <stdbool.h>

bool hasprefix(const char* s, const char* prefix) {
	return strncmp(s, prefix, strlen(prefix)) == 0;
}

void removenewline(char* s) {
	size_t len = strlen(s);
	if (*(s + len - 1) == '\n') {
		*(s + len - 1) = '\0';
	}
}

const char* cd = "cd ";

int main(int argc, char* argv[]) {
//	char cwd[PATH_MAX];
//	getcwd(cwd, sizeof(cwd));
//	printf("%s", cwd);

	char buf[PATH_MAX];
	read(STDIN_FILENO, buf, sizeof(buf));
	printf("%s", buf);

	if (hasprefix(buf, cd)) {
		char* path = buf + strlen(cd);
		removenewline(path);
		if (chdir(path)) {
			printf("%s", strerror(errno));
		}
	}
}