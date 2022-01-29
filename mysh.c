#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>

int main(int argc, char* argv[]) {
	if (chdir("")) {
		printf("%s", strerror(errno));
	}

	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
	printf("%s", cwd);
}