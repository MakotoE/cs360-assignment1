#include <unistd.h>
#include <errno.h>
#include <stdio.h>

char* chdirerror(int err) {
	switch (err) {
	case EACCES:
		return "access denied";
	case ENOENT:
		return "path does not exist";
	case ENOTDIR:
		return "path is not a directory";
	default:
		return "unknown error";
	}
}

int main(int argc, char* argv[]) {
	if (chdir("")) {
		printf("%s", chdirerror(errno));
	}
}