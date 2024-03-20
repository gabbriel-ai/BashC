#ifndef CAT_H
#define CAT_H

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

char SHORT_OPTIONS[] = "bEensTtv";
struct option LONG_OPTIONS[] = {
    {"number-nonblank", 0, NULL, 'b'}, {"show-ends", 0, NULL, 'E'},
    {"number", 0, NULL, 'n'},          {"squeeze-blank", 0, NULL, 's'},
    {"show-tabs", 0, NULL, 'T'},       {NULL, 0, NULL, 0}};

typedef struct Options {
  bool b;
  bool e;
  bool n;
  bool s;
  bool t;
  bool v;
} Options;

void init_standart_struct(Options *options);
int set_option(const char option, Options *options);
void print_options(Options *options);
bool read_options(int argc, char *argv[], Options *options);
void read_files(int count, char *files_names[], Options *options);
void print_file(FILE *file, Options *options);

void option_number(Options *options, unsigned char previous_symbol,
                   unsigned char current_symbol);
bool option_squeeze_blank(Options *options, FILE *file,
                          unsigned char previous_symbol,
                          unsigned char current_symbol);
void option_show_ends(Options *options);
void option_show_tabs(Options *options, unsigned char *current_symbol);
void option_print_nonprt(unsigned char *current_symbol);
#endif