.PHONY: all clean rebuild

CC = gcc
CFLAGS = -Wall -Werror -Wextra

all: s21_grep

s21_grep: s21_print_error.o s21_grep.o
	$(CC) $(CFLAGS) s21_grep.o s21_print_error.o -o s21_grep

s21_grep.o: s21_grep.c
	$(CC) -c s21_grep.c

s21_print_error.o:
	$(CC) -c ../common/s21_print_error.c

clean:
	rm -rf *.o

rebuild: clean all
	