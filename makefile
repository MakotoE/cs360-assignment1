mysh: mysh.c
	gcc -o mysh -Wall mysh.c

.PHONY: all
all: mysh

.PHONY: clean
clean:
	rm -f mysh