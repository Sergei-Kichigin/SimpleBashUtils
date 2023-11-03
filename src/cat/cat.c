#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage();
void cat_with_flags(FILE *file, bool number_non_empty, bool display_ends,
										bool number_all, bool squeeze_blank, bool display_tabs,
										bool invert_output, int *total_line_number);
bool isNonPrintable(char c);

int main(int argc, char *argv[]) {
	bool number_non_empty = false;  // -b
	bool display_ends = false;      // -e
	bool number_all = false;        // -n
	bool squeeze_blank = false;     // -s
	bool display_tabs = false;      // -t
	bool invert_output = false;     // -v

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
			printf("s21_cat: Неизвестный флаг: %s\n", flag);
			print_usage();
			return 1;
		}

		i++;
	}

	if (i == argc) {
		cat_with_flags(stdin, number_non_empty, display_ends, number_all,
									 squeeze_blank, display_tabs, invert_output,
									 &total_line_number);
	} else {
		for (; i < argc; i++) {
			FILE *file = fopen(argv[i], "r");
			if (file == NULL) {
				perror("my_cat");
				return 1;
			}

			cat_with_flags(file, number_non_empty, display_ends, number_all,
										 squeeze_blank, display_tabs, invert_output,
										 &total_line_number);

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
										bool number_all, bool squeeze_blank, bool display_tabs,
										bool invert_output, int *total_line_number) {
	unsigned char line[1024];

	bool previous_line_was_blank = false;

	if (number_non_empty && number_all) {
		number_all = 0;
	}

	while (fgets((char *)line, sizeof(line), file) != NULL) {
		int line_len = strlen((char *)line);
		if (squeeze_blank) {
			if (line[0] == '\n') {
				if (previous_line_was_blank) {
					continue;
				} else {
					previous_line_was_blank = true;
				}
			} else {
				previous_line_was_blank = false;
			}
		}

		if (number_non_empty) {
			if (line[0] != '\n' && line[0] != '\r') {
				printf("%6d\t", *total_line_number);
				(*total_line_number)++;
			}
		}

		if (number_all) {
			printf("%6d\t", *total_line_number);
			(*total_line_number)++;
		}
		for (int i = 0; i < line_len; i++) {
			if (display_ends && (line[i] == '\n' || (line[i] == '\r' && line[i + 1] != '\n'))){
				putchar('$');
				putchar(line[i]);
			}
			else if (display_tabs && line[i] == '\t') {
				putchar('^');
				putchar('I');
			} else if (invert_output && isNonPrintable(line[i])) {
				putchar('^');
				putchar((line[i] + 64) % 128);
			} else {
				putchar(line[i]);
			}
		}
	}
}

bool isNonPrintable(char c) {
	return ((c >= 0 && c < 9) || (c > 10 && c < 32) || c == 127) && (c != '\r');
}
