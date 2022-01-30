mysh: mysh.c
	gcc -g -Wall -Wextra -o mysh mysh.c

.PHONY: all
all: mysh

.PHONY: clean
clean:
	rm -f mysh