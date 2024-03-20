#include "s21_cat.h"

#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../common/s21_print_error.h"

int main(int argc, char *argv[]) {
  Options options = {0};
  init_standart_struct(&options);
  if (read_options(argc, argv, &options)) {
    read_files(argc - optind, argv + optind, &options);
  } else {
    print_invalid_option();
  }
  return 0;
}

void init_standart_struct(Options *options) {
  options->b = 0;
  options->e = 0;
  options->n = 0;
  options->s = 0;
  options->t = 0;
  options->v = 0;
}

bool read_options(int argc, char *argv[], Options *options) {
  bool is_valid = true;
  char current_option;
  while ((current_option = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS,
                                       NULL)) != -1) {
    if (set_option(current_option, options) == false) {
      print_invalid_option();
      is_valid = false;
      break;
    }
  }
  if (options->b) {
    options->n = false;
  }
  return is_valid;
}
int set_option(const char option, Options *options) {
  bool is_valid_option = true;
  switch (option) {
    case 'b':
      options->b = true;
      break;
    case 'E':
      options->e = true;
      break;
    case 'e':
      options->e = true;
      options->v = true;
      break;
    case 'n':
      options->n = true;
      break;
    case 's':
      options->s = true;
      break;
    case 'T':
      options->t = true;
      break;
    case 't':
      options->v = true;
      options->t = true;
      break;
    case 'v':
      options->v = true;
      break;
    default:
      is_valid_option = false;
  }
  return is_valid_option;
}

void read_files(int count, char *files_names[], Options *options) {
  FILE *file = NULL;
  for (int file_number = 0; file_number < count; file_number++) {
    if ((file = fopen(files_names[file_number], "r")) != NULL) {
      if (file_number != 0) {
        char symbol;
        if (options->e) {
          printf("$");
        }
        if ((symbol = fgetc(file)) == '\n') {
          printf("%c", symbol);
        } else {
          ungetc(symbol, file);
        }
      }
      print_file(file, options);
      fclose(file);
    } else {
      print_invalid_file(files_names[file_number]);
    }
  }
}

void print_file(FILE *file, Options *options) {
  unsigned char current_symbol = fgetc(file);
  unsigned char previous_symbol = '\n';
  bool is_empty_line = false;
  while (!feof(file)) {
    if (current_symbol == '\n' && previous_symbol == '\n') {
      if (is_empty_line && options->s) {
        previous_symbol = current_symbol;
        current_symbol = fgetc(file);
        continue;
      }
      is_empty_line = true;
    } else {
      is_empty_line = false;
    }
    if (previous_symbol == '\n') {
      option_number(options, previous_symbol, current_symbol);
    }
    if (current_symbol == '\n') {
      option_show_ends(options);
    }
    if (current_symbol == '\t') {
      option_show_tabs(options, &current_symbol);
    }
    if (options->v) {
      option_print_nonprt(&current_symbol);
    }
    if (current_symbol == 127 && options->v) {
      printf("^?");
    } else {
      printf("%c", current_symbol);
    }

    previous_symbol = current_symbol;
    current_symbol = fgetc(file);
  }
}
void option_number(Options *options, unsigned char previous_symbol,
                   unsigned char current_symbol) {
  static int current_string = 1;
  if (options->b && current_symbol != '\n' && previous_symbol == '\n') {
    printf("%6d\t", current_string);
    current_string = current_string + 1;
  } else if (options->n) {
    printf("%6d\t", current_string);
    current_string = current_string + 1;
  }
}

bool option_squeeze_blank(Options *options, FILE *file,
                          unsigned char previous_symbol,
                          unsigned char current_symbol) {
  bool temp = 0;
  if (options->s && current_symbol == '\n' && previous_symbol == '\n') {
    while (current_symbol == '\n') {
      current_symbol = fgetc(file);
      if (feof(file)) {
        printf("%c", '\n');
        temp = 1;
        break;
      }
    }
    if (!temp) {
      printf("%c", current_symbol);
    }

    return 1;
  }
  return 0;
}

void option_show_ends(Options *options) {
  if (options->e) {
    printf("$");
  }
}

void option_show_tabs(Options *options, unsigned char *current_symbol) {
  if (options->t) {
    printf("^");
    *current_symbol = 'I';
  }
}

void option_print_nonprt(unsigned char *current_symbol) {
  if (*current_symbol < 32 && *current_symbol != '\n' &&
      *current_symbol != '\t') {
    printf("^");
    *current_symbol = *current_symbol + 64;
  } else if (*current_symbol > 127 && *current_symbol < 160) {
    printf("M-^");
    *current_symbol = *current_symbol - 64;
  } else if (*current_symbol >= 160) {
    printf("M-");
    *current_symbol = *current_symbol - 128;
  }
}