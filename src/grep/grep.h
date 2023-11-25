// grep.h
#ifndef GREP_H
#define GREP_H

#define MAX_PATTERNS 100
#define MAX_LINE_LENGTH 2048
#define SUCCESS 0
#define ERROR 1

#include <pcre.h>
#include <stdbool.h>

typedef struct {
  char* patterns[MAX_PATTERNS];
  int pattern_count;
  bool read_patterns;
  bool ignore_case;
  bool invert_match;
  bool count_lines;
  bool print_filenames;
  bool print_line_numbers;
  bool need_filename;
  bool hide_filenames;
  bool silent_mode;
  bool read_patterns_from_file;
  char* pattern_file;
} GrepOptions;

pcre* compile_combined_pattern(const GrepOptions* options);
int read_patterns_from_file(GrepOptions* options);
void print_usage();
void cleanup(pcre* re);
int process_file(const GrepOptions* options, const char* filename,
                 pcre* combined_regex);
void cleanup_options(GrepOptions* options);

#endif  // GREP_H
