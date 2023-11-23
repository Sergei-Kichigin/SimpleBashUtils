// grep.h
#ifndef GREP_H
#define GREP_H

#define MAX_PATTERNS 10
#define MAX_LINE_LENGTH 2048
#define EXIT_CODE_SUCCESS 0
#define EXIT_CODE_ERROR 1

#include <pcre.h>
#include <stdbool.h>

typedef struct {
  char* patterns[MAX_PATTERNS];
  int pattern_count;
  bool ignore_case;
  bool invert_match;
  bool count_lines;
  bool print_filenames;
  bool print_line_numbers;
  bool need_filename;
  bool hide_filenames;
  bool silent_mode;
} GrepOptions;

pcre* compile_combined_pattern(const GrepOptions* options);
void print_usage();
void cleanup(pcre* re);
int compile_patterns(const GrepOptions* options, pcre* re[]);
int process_file(const GrepOptions* options, const char* filename,
                 pcre* combined_regex);
void cleanup_options(GrepOptions* options);

#endif  // GREP_H
