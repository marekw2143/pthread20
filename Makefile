CC = gcc

main: main.c
	$(CC) $< -o $@ -w -pthread -g
