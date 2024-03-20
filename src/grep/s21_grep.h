#ifndef GREP_H
#define GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
char SHORT_OPTIONS[] = "ivclnhsoe:f:";
typedef struct Options {
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool o;
  bool e;
  bool f;
  int amount_files;
  int amount_patterns;
  char* patterns[256];
  regex_t comp_patterns[256];

} Options;

void init_standart_struct(Options* options);
bool read_options(int argc, char* argv[], Options* options);
bool set_option(const char option, Options* options);
void get_patterns_from_file(char* patterns[256], int* current_size);

bool compile_regex(Options* options);

void print_file(Options* options, char* file_name);
void option_c_print(Options* options, char* file_name);
void option_o_print(Options* options, char* file_name, int* current_string);
void option_v_print(Options* options, char* file_name, int* current_string);
void standart_print(Options* options, char* file_name, char* line,
                    int* current_string);

void free_memory(Options* options);

#endif