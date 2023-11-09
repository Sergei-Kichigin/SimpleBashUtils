#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage();
void cat_with_flags(FILE *file, bool number_non_empty, bool display_ends,
                    bool number_all, bool squeeze_blank, bool display_tabs,
                    bool invert_output, int *total_line_number, bool *new_line);


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

  while (i < argc && argv[i][0] == '-') {
    char *flag = argv[i];
    if (flag[1] == 'b') {
      number_non_empty = true;
    } else if (flag[1] == 'E') {
      display_ends = true;
    } else if (flag[1] == 'T') {
      display_tabs = true;
    } else if (flag[1] == 'e') {
      display_ends = true;
      invert_output = true;
    } else if (flag[1] == 'n') {
      number_all = true;
    } else if (flag[1] == 's') {
      squeeze_blank = true;
    } else if (flag[1] == 't') {
      display_tabs = true;
      invert_output = true;
    } else {
      printf("s21_cat: unknown flag: %s\n", flag);
      print_usage();
      return 1;
    }
    i++;
  }

  if (i == argc) {
    cat_with_flags(stdin, number_non_empty, display_ends, number_all,
                   squeeze_blank, display_tabs, invert_output,
                   &total_line_number, &new_line);
  } else {
    for (; i < argc; i++) {
      FILE *file = fopen(argv[i], "r");
      if (file == NULL) {
        perror("problems with file");
        return 1;
      }
      cat_with_flags(file, number_non_empty, display_ends, number_all,
                     squeeze_blank, display_tabs, invert_output,
                     &total_line_number, &new_line);

      fclose(file);
    }
  }

  return 0;
}

void cat_with_flags(FILE *file, bool number_non_empty, bool display_ends,
                    bool number_all, bool squeeze_blank, bool display_tabs,
                    bool invert_output, int *total_line_number,
                    bool *new_line) {

  int current_char = fgetc(file);
  static int lf_counter = 1;

  if (number_all && number_non_empty) {
    number_all = false;
  }

  while (!feof(file)) {
    // -s
    if (squeeze_blank) {
      if (current_char == '\n') {
        lf_counter++;
      } else {
        lf_counter = 0;
      }
    }

    // -b / -n
    if (current_char != '\n' || lf_counter <= 2) {
      if (*new_line) {
        if (number_all) {
          printf("%6d\t", *total_line_number);
          (*total_line_number)++;
        } else if (number_non_empty && current_char != '\n') {
          printf("%6d\t", *total_line_number);
          (*total_line_number)++;
        }
        *new_line = false;
      }

      // -v
      if (invert_output) {
        if (current_char < 32 && current_char != '\n' && current_char != '\t') {
          printf("^");
          current_char += 64;
        }
        if (current_char == 127) {
          printf("^");
          current_char = 63;
        }
        if (current_char > 127) {
          printf("M-");
          current_char -= 128;
          if (current_char < 32) {
            printf("^");
            current_char += 64;
          }
          if (current_char == 127) {
            printf("^");
            current_char = 63;
          }
        }
      }

      // -E
      if (display_ends) {
        if (current_char == '\n') {
          printf("$");
        }
      }
      // -T
      if (display_tabs && current_char == '\t') {
        printf("^");
        current_char += 64;
      }

      if (current_char == '\n') {
        *new_line = true;
      }
      putchar(current_char);
    }

    current_char = fgetc(file);
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