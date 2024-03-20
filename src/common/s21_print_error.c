#include <stdio.h>

void print_invalid_option() { printf("Error : Invalid option\n"); }

void print_error_memory() { printf("Error : Memory allocation"); }

void print_invalid_file(char *file_name) {
  printf("No such file or directory: %s", file_name);
}

void print_compile_error() { printf("Compile Error"); }
