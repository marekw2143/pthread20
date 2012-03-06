CC = g++
CFLAGS = -Wall -Wextra -g
LIBS = -pthread

main: main.cpp
	$(CC) $< -o $@ $(CFLAGS) $(LIBS)
