#include "cat.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  bool number_non_empty = false;
  bool display_ends = false;
  bool number_all = false;
  bool squeeze_blank = false;
  bool display_tabs = false;
  bool invert_output = false;

  bool new_line = true;

  int total_line_number = 1;
  int i = 1;

  if (argc == 1) {
    print_usage();
    return ERROR;
  }

  while (i < argc && argv[i][0] == '-') {
    process_flag(argv[i], &number_non_empty, &display_ends, &number_all,
                 &squeeze_blank, &display_tabs, &invert_output);
    i++;
  }

  if (i == argc) {
    fprintf(stderr, "Error: Missed filename\n");
    print_usage();
    return ERROR;
  }

  for (; i < argc; i++) {
    FILE *file = fopen(argv[i], "r");
    if (file == NULL) {
      fprintf(stderr, "No such file or directory\n");
      return ERROR;
    }
    cat_with_flags(file, number_non_empty, display_ends, number_all,
                   squeeze_blank, display_tabs, invert_output,
                   &total_line_number, &new_line);
    fclose(file);
  }
  return SUCCESS;
}

void process_flag(char *flag, bool *number_non_empty, bool *display_ends,
                  bool *number_all, bool *squeeze_blank, bool *display_tabs,
                  bool *invert_output) {
  // Обработка флагов командной строки
  if (flag[1] == 'b' || strcmp(flag, "--number-nonblank") == 0) {
    *number_non_empty = true;
  } else if (flag[1] == 'E') {
    *display_ends = true;
  } else if (flag[1] == 'T') {
    *display_tabs = true;
  } else if (flag[1] == 'e') {
    *display_ends = true;
    *invert_output = true;
  } else if (flag[1] == 'n' || strcmp(flag, "--number") == 0) {
    *number_all = true;
  } else if (flag[1] == 's' || strcmp(flag, "--squeeze-blank") == 0) {
    *squeeze_blank = true;
  } else if (flag[1] == 't') {
    *display_tabs = true;
    *invert_output = true;
  } else {
    fprintf(stderr, "s21_cat: unknown flag: %s\n", flag);
    print_usage();
    exit(ERROR);
  }
}

void cat_with_flags(FILE *file, bool number_non_empty, bool display_ends,
                    bool number_all, bool squeeze_blank, bool display_tabs,
                    bool invert_output, int *total_line_number,
                    bool *new_line) {
  int current_char = 0;
  static int lf_counter = 1;

  if (number_all && number_non_empty) {
    number_all = false;
  }

  while ((current_char = fgetc(file)) != EOF) {
    process_squeeze_blank(current_char, &lf_counter, squeeze_blank);
    if (current_char != '\n' || lf_counter <= 2) {
      process_numbering(&current_char, number_all, number_non_empty,
                        total_line_number, new_line);
      process_inverted_character(&current_char, invert_output);
      process_special_characters(&current_char, display_ends, display_tabs);
      print_character(current_char, new_line);
    }
  }
}

void print_usage() {
  printf("Использование: s21_cat [-bent] [файл...]\n");
  printf("  -b  Нумеровать только непустые строки\n");
  printf("  -e  Выводить символ $ в конце каждой строки\n");
  printf("  -n  Нумеровать все строки\n");
  printf("  -s  Удалять пустые строки\n");
  printf("  -t  Заменять символы TAB на ^I\n");
}

void process_squeeze_blank(int current_char, int *lf_counter,
                           bool squeeze_blank) {
  // Обработка сжатия пустых строк
  if (squeeze_blank) {
    if (current_char == '\n') {
      (*lf_counter)++;
    } else {
      *lf_counter = 0;
    }
  }
}

void process_inverted_character(int *current_char, bool invert_output) {
  // Обработка непечатных символов
  if (invert_output) {
    if (*current_char < 32 && *current_char != '\n' && *current_char != '\t') {
      printf("^");
      *current_char += 64;
    } else if (*current_char == 127) {
      printf("^");
      *current_char = 63;
    } else if (*current_char > 127) {
      printf("M-");
      *current_char -= 128;
      if (*current_char < 32) {
        printf("^");
        *current_char += 64;
      }
      if (*current_char == 127) {
        printf("^");
        *current_char = 63;
      }
    }
  }
}

void process_special_characters(int *current_char, bool display_ends,
                                bool display_tabs) {
  // Обработка конца строки
  if (display_ends && *current_char == '\n') {
    printf("$");
  }
  // Обработка отступов
  if (display_tabs && *current_char == '\t') {
    printf("^");
    *current_char += 64;
  }
}

void process_numbering(int *current_char, bool number_all,
                       bool number_non_empty, int *total_line_number,
                       bool *new_line) {
  // Обработка нумерации строк
  if (*new_line) {
    if (number_all) {
      printf("%6d\t", *total_line_number);
      (*total_line_number)++;
    } else if (number_non_empty && *current_char != '\n') {
      printf("%6d\t", *total_line_number);
      (*total_line_number)++;
    }
    *new_line = false;
  }
}

void print_character(int current_char, bool *new_line) {
  // Вывод символа
  if (current_char == '\n') {
    *new_line = true;
  }
  putchar(current_char);
}
