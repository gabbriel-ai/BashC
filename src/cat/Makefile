.PHONY: all clean rebuild

CC = gcc
CFLAGS = -Wall -Werror -Wextra

all: s21_cat

s21_cat: s21_print_error.o s21_cat.o
	$(CC) $(CFLAGS) s21_cat.o s21_print_error.o -o s21_cat

s21_cat.o: s21_cat.c
	$(CC) -c s21_cat.c

s21_print_error.o:
	$(CC) -c ../common/s21_print_error.c

clean:
	rm -rf *.o

rebuild: clean all
	