CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200809L

s21_grep: grep.c
	$(CC) $(CFLAGS) -o s21_grep grep.c -lpcre

clean:
	rm -f s21_grep
