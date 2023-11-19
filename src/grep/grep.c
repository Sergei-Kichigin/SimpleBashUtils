#include "grep.h"

#include <pcre.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  GrepOptions options = {{NULL}, 0, false, false, false, false, false, false};
  bool pattern_flag = false;
  int opt;
  while ((opt = getopt(argc, argv, "e:ivcln")) != -1) {
    switch (opt) {
      case 'e':
        pattern_flag = true;
        if (options.pattern_count < MAX_PATTERNS - 1) {
          options.patterns[options.pattern_count++] =
              strndup(optarg, MAX_LINE_LENGTH);
        } else {
          fprintf(stderr, "Error: Too many patterns\n");
          print_usage();
          cleanup_options(&options);
          return EXIT_CODE_ERROR;
        }
        break;
      case 'i':
        options.ignore_case = true;
        break;
      case 'v':
        options.invert_match = true;
        break;
      case 'c':
        options.count_lines = true;
        break;
      case 'l':
        options.print_filenames = true;
        break;
      case 'n':
        options.print_line_numbers = true;
        break;
      case '?':
        fprintf(stderr, "Error: Unknown option\n");
        print_usage();
        cleanup_options(&options);
        return EXIT_CODE_ERROR;
    }
  }

  if (!pattern_flag && optind < argc) {
    options.patterns[options.pattern_count++] = strdup(argv[optind++]);
  }

  if (argc - optind > 1) {
    options.need_filename = true;
  }

  if (options.count_lines && options.print_filenames) {
    options.count_lines = false;
  }

  for (int i = optind; i < argc; i++) {
    int process_result = process_file(&options, argv[i]);
    if (process_result != EXIT_CODE_SUCCESS) {
      cleanup_options(&options);
      return process_result;
    }
  }

  cleanup_options(&options);
  return EXIT_CODE_SUCCESS;
}

void print_usage() {
  printf("Использование: [-e pattern] [-i] [-v] [-c] [-l] [-n] filename\n");
}

void cleanup(pcre* re[]) {
  for (int pattern_index = 0; pattern_index < MAX_PATTERNS; pattern_index++) {
    if (re[pattern_index] != NULL) {
      pcre_free(re[pattern_index]);
    }
  }
}

int compile_patterns(const GrepOptions* options, pcre* re[]) {
  const char* error;
  int erroffset;
  for (int pattern_index = 0; pattern_index < options->pattern_count;
       pattern_index++) {
    re[pattern_index] = pcre_compile(options->patterns[pattern_index],
                                     options->ignore_case ? PCRE_CASELESS : 0,
                                     &error, &erroffset, NULL);
    if (re[pattern_index] == NULL) {
      fprintf(stderr, "PCRE compilation failed at offset %d: %s\n", erroffset,
              error);
      cleanup(re);
      return EXIT_CODE_ERROR;
    }
  }
  return EXIT_CODE_SUCCESS;
}

int process_file(const GrepOptions* options, const char* filename) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening file");
    return EXIT_CODE_ERROR;
  }

  char line[MAX_LINE_LENGTH];
  int line_number = 0;
  int match_count = 0;
  bool line_has_match = false;

  pcre* re[MAX_PATTERNS] = {NULL};
  int compile_result = compile_patterns(options, re);
  if (compile_result != EXIT_CODE_SUCCESS) {
    fclose(file);
    return compile_result;
  }

  while (fgets(line, sizeof(line), file) != NULL) {
    line_number++;

    for (int pattern_index = 0; pattern_index < options->pattern_count;
         pattern_index++) {
      int rc =
          pcre_exec(re[pattern_index], NULL, line, strlen(line), 0, 0, NULL, 0);
      if ((rc >= 0 && !options->invert_match) ||
          (rc < 0 && options->invert_match)) {
        match_count++;
        line_has_match = true;
        if (!options->print_filenames && !options->count_lines) {
          if (options->need_filename) printf("%s:", filename);
          if (options->print_line_numbers) printf("%d:", line_number);
          printf("%s", line);
        }
      }
    }
  }
  cleanup(re);
  fclose(file);

  if (options->count_lines) {
    if (options->need_filename) {
      printf("%s:%d\n", filename, match_count);
    } else {
      printf("%d\n", match_count);
    }
  }

  else if (options->print_filenames && line_has_match)
    printf("%s\n", filename);

  return EXIT_CODE_SUCCESS;
}

void cleanup_options(GrepOptions* options) {
    for (int i = 0; i < options->pattern_count; i++) {
        free(options->patterns[i]);
    }
}
