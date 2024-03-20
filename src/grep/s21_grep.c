#define _GNU_SOURCE
#include "s21_grep.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/s21_print_error.h"

int main(int argc, char *argv[]) {
  Options options = {0};
  init_standart_struct(&options);
  if (read_options(argc, argv, &options)) {
    if (options.patterns[0] == NULL && !options.e && !options.f) {
      options.patterns[0] = malloc(sizeof(char) * (strlen(argv[optind])) + 1);
      if (options.patterns[0] == NULL) {
        print_error_memory();
      } else {
        memcpy(options.patterns[0], argv[optind], strlen(argv[optind]));
        options.patterns[0][strlen(argv[optind])] = '\0';
        optind++;
      }
    }
    if (options.o && options.v && !options.c) {
      if (options.l) {
        while (argv[optind] != NULL) {
          printf("%s\n", argv[optind]);
          optind++;
        }
      }
      return 0;
    }
    if (compile_regex(&options)) {
      if (argv[optind] == NULL) {
        print_invalid_file(NULL);
      } else {
        options.amount_files = argc - optind;
        while (argv[optind] != NULL) {
          print_file(&options, argv[optind]);
          optind++;
        }
      }
    }
    free_memory(&options);
  }
  return 0;
}

void init_standart_struct(Options *options) {
  options->i = 0;
  options->v = 0;
  options->c = 0;
  options->l = 0;
  options->n = 0;
  options->h = 0;
  options->s = 0;
  options->o = 0;
  options->e = 0;
  options->f = 0;
  options->amount_files = 0;
  options->amount_patterns = 0;
}

bool read_options(int argc, char *argv[], Options *options) {
  bool is_valid = true;
  char current_option = 0;
  while ((current_option = getopt(argc, argv, SHORT_OPTIONS)) != -1 &&
         current_option != '?') {
    if (set_option(current_option, options) == false) {
      print_invalid_option();
      is_valid = false;
    }
  }
  if (current_option == '?') {
    is_valid = false;
  }
  return is_valid;
}

bool set_option(const char option, Options *options) {
  static int size = 0;
  bool is_valid_option = true;
  switch (option) {
    case 'i':
      options->i = true;
      break;
    case 'v':
      options->v = true;
      break;
    case 'c':
      options->c = true;
      break;
    case 'l':
      options->l = true;
      break;
    case 'n':
      options->n = true;
      break;
    case 'h':
      options->h = true;
      break;
    case 's':
      options->s = true;
      break;
    case 'o':
      options->o = true;
      break;
    case 'e':
      options->e = true;
      options->patterns[size] = malloc(sizeof(char) * (strlen(optarg)) + 1);
      if (options->patterns[size] == NULL) {
        print_error_memory();
      } else {
        memcpy(options->patterns[size], optarg, strlen(optarg));
        options->patterns[size][strlen(optarg)] = '\0';
        size++;
      }
      break;
    case 'f':
      options->f = true;
      get_patterns_from_file(options->patterns, &size);
      break;
    default:
      is_valid_option = false;
      break;
  }
  return is_valid_option;
}

void get_patterns_from_file(char *patterns[256], int *current_size) {
  FILE *file = fopen(optarg, "r");

  if (file == NULL) {
    print_invalid_file(optarg);
  } else {
    char *line = NULL;
    size_t size_line = 0;
    while ((getline(&line, &size_line, file)) != -1) {
      if (line[0] != '\n') {
        if (line[strlen(line) - 1] == '\n') {
          line[strlen(line) - 1] = '\0';
        }
        patterns[*current_size] = malloc(sizeof(char) * (size_line + 1));
        if (patterns[*current_size] == NULL) {
          print_error_memory();
        } else {
          memcpy(patterns[*current_size], line, size_line);
          *current_size = *current_size + 1;
        }
      }
    }
    free(line);
    fclose(file);
  }
}

bool compile_regex(Options *options) {
  bool is_success = true;
  int index = 0;
  int cflags = 0;
  if (options->i) {
    cflags = REG_ICASE | REG_EXTENDED;
  } else {
    cflags = REG_EXTENDED;
  }
  while (options->patterns[index] != NULL) {
    options->amount_patterns = options->amount_patterns + 1;
    if (regcomp(&options->comp_patterns[index], options->patterns[index],
                cflags) != 0) {
      print_compile_error();
      is_success = false;
    }
    index++;
  }
  return is_success;
}

void print_file(Options *options, char *file_name) {
  int current_string = 1;
  if (options->c) {
    option_c_print(options, file_name);
  } else if (options->o) {
    option_o_print(options, file_name, &current_string);
  } else {
    option_v_print(options, file_name, &current_string);
  }
}

void option_c_print(Options *options, char *file_name) {
  FILE *file = fopen(file_name, "r");

  if (file == NULL) {
    if (!options->s) {
      print_invalid_file(file_name);
      return;
    }
  }
  int match_strings = 0;
  int index = 0;
  char *line = NULL;
  size_t len = 0;
  regex_t comp_regex = {0};
  bool is_match = false;
  while (getline(&line, &len, file) != -1) {
    is_match = false;
    index = 0;
    while (index != options->amount_patterns) {
      comp_regex = options->comp_patterns[index];
      if (regexec(&comp_regex, line, 0, NULL, 0) == 0) {
        is_match = true;
      }
      index++;
    }
    if (options->v) {
      is_match = !is_match;
      if (options->o) {
        is_match = false;
      }
    }
    if (is_match) {
      match_strings++;
    }
  }

  if (options->l) {
    if (match_strings != 0) {
      printf("%s\n", file_name);
    }
  } else {
    if (options->amount_files > 1 && !options->h) {
      printf("%s:", file_name);
    }
    printf("%d\n", match_strings);
  }

  free(line);
}

void option_o_print(Options *options, char *file_name, int *current_string) {
  FILE *file = fopen(file_name, "r");
  if (file == NULL) {
    if (!options->s) {
      print_invalid_file(file_name);
      return;
    }
  }
  bool file_name_was_print = false;
  char *line = NULL;
  size_t len = 0;
  regmatch_t pmatches = {0};
  regex_t comp_regex = {0};
  int offset = 0;
  int index = 0;
  while (getline(&line, &len, file) != -1) {
    offset = 0;
    index = 0;
    while (index != options->amount_patterns) {
      comp_regex = options->comp_patterns[index];
      while (regexec(&comp_regex, line + offset, 1, &pmatches, 0) == 0) {
        if (options->l) {
          if (!file_name_was_print) {
            printf("%s\n", file_name);
            file_name_was_print = true;
          }
          break;
        } else {
          if (options->amount_files > 1 && !options->h) {
            printf("%s:", file_name);
          }
          if (options->n) {
            printf("%d:", *current_string);
          }
          printf("%.*s\n", (int)(pmatches.rm_eo - pmatches.rm_so),
                 (line + offset + pmatches.rm_so));
        }
        offset += pmatches.rm_eo;
        if (pmatches.rm_so == pmatches.rm_eo) {
          offset += 1;
        }
        if (offset > (int)len) {
          break;
        }
        if ((line + offset)[0] == '\n') {
          break;
        }
      }
      index++;
    }

    *current_string = *current_string + 1;
  }
  fclose(file);
  free(line);
}

void option_v_print(Options *options, char *file_name, int *current_string) {
  FILE *file = fopen(file_name, "r");
  if (file == NULL) {
    if (!options->s) {
      print_invalid_file(file_name);
      return;
    }
  }
  char *line = NULL;
  size_t len = 0;
  bool is_name_was_print = false;
  bool is_match = false;
  while (getline(&line, &len, file) != -1) {
    is_match = false;
    int index = 0;
    while (index != options->amount_patterns) {
      regex_t comp_regex = options->comp_patterns[index];
      if ((regexec(&comp_regex, line, 0, NULL, 0) == 0)) {
        is_match = true;
      }
      index++;
    }
    if (options->v) {
      is_match = !is_match;
      if (options->o) {
        is_match = false;
      }
    }
    if (is_match == true) {
      if (options->l) {
        if (!is_name_was_print) {
          printf("%s\n", file_name);
          is_name_was_print = true;
        }
      } else {
        standart_print(options, file_name, line, current_string);
      }
    }

    *current_string = *current_string + 1;
  }
  free(line);
  fclose(file);
}

void standart_print(Options *options, char *file_name, char *line,
                    int *current_string) {
  bool is_printed_string = false;
  if (options->amount_files > 1 && !options->h) {
    printf("%s:", file_name);
  }
  if (options->n && !(is_printed_string)) {
    printf("%d:", *current_string);
    is_printed_string = true;
  }
  printf("%s", line);
  if (strstr(line, "\n") == NULL) {
    printf("\n");
  }
}

void free_memory(Options *options) {
  int index = 0;
  while (options->patterns[index] != NULL) {
    free(options->patterns[index]);
    regfree(&options->comp_patterns[index]);
    index++;
  }
}
