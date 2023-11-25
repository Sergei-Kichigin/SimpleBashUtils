// cat.h
#ifndef CAT_H
#define CAT_H
#define SUCCESS 0
#define ERROR 1

#include <stdbool.h>
#include <stdio.h>

void print_usage();
void process_flag(char *flag, bool *number_non_empty, bool *display_ends,
                  bool *number_all, bool *squeeze_blank, bool *display_tabs,
                  bool *invert_output);
void cat_with_flags(FILE *file, bool number_non_empty, bool display_ends,
                    bool number_all, bool squeeze_blank, bool display_tabs,
                    bool invert_output, int *total_line_number, bool *new_line);

void process_squeeze_blank(int current_char, int *lf_counter,
                           bool squeeze_blank);
void process_inverted_character(int *current_char, bool invert_output);
void process_special_characters(int *current_char, bool display_ends,
                                bool display_tabs);
void process_numbering(int *current_char, bool number_all,
                       bool number_non_empty, int *total_line_number,
                       bool *new_line);
void print_character(int current_char, bool *new_line);

#endif  // CAT_H
