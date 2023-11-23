#include "grep.h"

#include <pcre.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  GrepOptions options = {{NULL}, 0,     false, false, false,
                         false,  false, false, false, false};
  bool pattern_flag = false;
  int opt;
  while ((opt = getopt(argc, argv, "e:ivclnhs")) != -1) {
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
      case 'h':
        options.hide_filenames = true;
        break;
      case 's':
        options.silent_mode = true;
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

  pcre* combined_regex = compile_combined_pattern(&options);

  if (combined_regex == NULL) {
    cleanup_options(&options);
    return EXIT_CODE_ERROR;
  }

  for (int i = optind; i < argc; i++) {
    int process_result = process_file(&options, argv[i], combined_regex);
    if (process_result != EXIT_CODE_SUCCESS) {
      cleanup_options(&options);
      pcre_free(combined_regex);
      return process_result;
    }
  }

  pcre_free(combined_regex);
  cleanup_options(&options);
  return EXIT_CODE_SUCCESS;
}

void print_usage() {
  printf("Использование: [-e pattern] [-i] [-v] [-c] [-l] [-n] filename\n");
}

void cleanup(pcre* re) {
  if (re != NULL) {
    pcre_free(re);
  }
}

pcre* compile_combined_pattern(const GrepOptions* options) {
  char combined_pattern[MAX_LINE_LENGTH * MAX_PATTERNS];
  strcpy(combined_pattern, "(?:");

  for (int pattern_index = 0; pattern_index < options->pattern_count;
       pattern_index++) {
    strcat(combined_pattern, options->patterns[pattern_index]);
    if (pattern_index < options->pattern_count - 1) {
      strcat(combined_pattern, "|");
    }
  }

  strcat(combined_pattern, ")");

  const char* error;
  int erroffset;
  pcre* combined_regex =
      pcre_compile(combined_pattern, options->ignore_case ? PCRE_CASELESS : 0,
                   &error, &erroffset, NULL);

  if (combined_regex == NULL) {
    fprintf(stderr, "PCRE compilation failed at offset %d: %s\n", erroffset,
            error);
  }

  return combined_regex;
}

int process_file(const GrepOptions* options, const char* filename,
                 pcre* combined_regex) {
  FILE* file = fopen(filename, "r");
  if (file == NULL && !options->silent_mode) {
    perror("Error opening file");
    return EXIT_CODE_ERROR;
  }

  char line[MAX_LINE_LENGTH];
  int line_number = 0;
  int match_count = 0;
  bool line_has_match = false;

  while (fgets(line, sizeof(line), file) != NULL) {
    line_number++;

    int rc = pcre_exec(combined_regex, NULL, line, strlen(line), 0, 0, NULL, 0);
    if ((rc >= 0 && !options->invert_match) ||
        (rc < 0 && options->invert_match)) {
      match_count++;
      line_has_match = true;
      if (!options->print_filenames && !options->count_lines) {
        if (options->need_filename && !options->hide_filenames)
          printf("%s:", filename);
        if (options->print_line_numbers) printf("%d:", line_number);
        printf("%s", line);
      }
    }
  }

  fclose(file);

  if (options->count_lines) {
    if (options->need_filename && !options->hide_filenames) {
      printf("%s:%d\n", filename, match_count);
    } else {
      printf("%d\n", match_count);
    }
  } else if (options->print_filenames && line_has_match) {
    printf("%s\n", filename);
  }

  return EXIT_CODE_SUCCESS;
}

void cleanup_options(GrepOptions* options) {
  for (int i = 0; i < options->pattern_count; i++) {
    free(options->patterns[i]);
  }
}
