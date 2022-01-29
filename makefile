mysh: mysh.c
	gcc -g -Wall -o mysh mysh.c

.PHONY: all
all: mysh

.PHONY: clean
clean:
	rm -f mysh