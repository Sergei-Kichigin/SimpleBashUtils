#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage();
void cat_with_flags(FILE *file, bool number_non_empty, bool display_ends,
                    bool number_all, bool squeeze_blank, bool display_tabs);

int main(int argc, char *argv[]) {
  bool number_non_empty = false;  // -b
  bool display_ends = false;      // -e
  bool number_all = false;        // -n
  bool squeeze_blank = false;     // -s
  bool display_tabs = false;      // -t

  int i = 1;

  while (i < argc && argv[i][0] == '-') {
    char *flag = argv[i];
    if (flag[1] == 'b') {
      number_non_empty = true;
    } else if (flag[1] == 'e') {
      display_ends = true;
    } else if (flag[1] == 'n') {
      number_all = true;
    } else if (flag[1] == 's') {
      squeeze_blank = true;
    } else if (flag[1] == 't') {
      display_tabs = true;
    } else {
      printf("s21_cat: Неизвестный флаг: %s\n", flag);
      print_usage();
      return 1;
    }

    i++;
  }
  printf("status of flags %d %d %d %d %d\n", number_non_empty, display_ends,
         number_all, squeeze_blank, display_tabs);

  if (i == argc) {
    cat_with_flags(stdin, number_non_empty, display_ends, number_all,
                   squeeze_blank, display_tabs);
  } else {
    for (; i < argc; i++) {
      FILE *file = fopen(argv[i], "r");
      if (file == NULL) {
        perror("my_cat");
        return 1;
      }

      cat_with_flags(file, number_non_empty, display_ends, number_all,
                     squeeze_blank, display_tabs);

      fclose(file);
    }
  }

  return 0;
}

void print_usage() {
  printf("Использование: s21_cat [-bent] [файл...]\n");
  printf("  -b  Нумеровать только непустые строки\n");
  printf("  -e  Выводить символ $ в конце каждой строки\n");
  printf("  -n  Нумеровать все строки\n");
  printf("  -s  Удалять пустые строки\n");
  printf("  -t  Заменять символы TAB на ^I\n");
}

void cat_with_flags(FILE *file, bool number_non_empty, bool display_ends,
                    bool number_all, bool squeeze_blank, bool display_tabs) {
  char line[1024];
  int line_number = 1;

  printf("b - %d \ne - %d \nn - %d \ns - %d \nt - %d\n\n\n\n", number_non_empty,
         display_ends, number_all, squeeze_blank, display_tabs);

  while (fgets(line, sizeof(line), file) != NULL) {

    if (number_non_empty) {
        if (line[0] != '\n' && line[0] != '\r') {
            printf("%d %s", line_number, line);
            line_number++;
        } else {
            printf("%s", line);
        }
    }
    if (number_all) {
        printf("%d %s", line_number, line);
        line_number++;
    }
    if (display_ends) {
        line[strlen(line) - 1] = '$';
        printf("%s\n", line);
    }
  }
}
