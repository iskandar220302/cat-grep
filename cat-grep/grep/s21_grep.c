//
// Created by Iskander Zakirov on 19.11.2025.
//

#include "s21_grep.h"

#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _GNU_SOURCE
char* s21_strdup(const char* s) {
  if (!s) return NULL;
  size_t len = strlen(s) + 1;
  char* new = malloc(len);
  if (new) {
    memcpy(new, s, len);
  }
  return new;
}
#define strdup s21_strdup
#endif

int main(int argc, char* argv[]) {
  Options opts;
  memset(&opts, 0, sizeof(Options));

  opts.patterns = calloc(100, sizeof(char*));
  opts.files = calloc(100, sizeof(char*));
  if (!opts.patterns || !opts.files) {
    fprintf(stderr, "Memory allocation failed\n");
    free_options(&opts);
    return 1;
  }

  parse_args(argc, argv, &opts);

  if (opts.patterns_count == 0) {
    fprintf(stderr, "No pattern provided\n");
    free_options(&opts);
    return 1;
  }

  if (opts.files_count == 0) {
    process_file(NULL, &opts);
  } else {
    for (int i = 0; i < opts.files_count; i++) {
      process_file(opts.files[i], &opts);
    }
  }

  free_options(&opts);
  return 0;
}

void parse_args(int argc, char* argv[], Options* opts) {
  int has_pattern = 0;

  for (int i = 1; i < argc; i++) {
    char* arg = argv[i];

    if (arg[0] == '-' && arg[1] != '\0') {
      if (strcmp(arg, "-e") == 0) {
        if (i + 1 >= argc) {
          fprintf(stderr, "grep: option requires an argument -- e\n");
          free_options(opts);
          exit(1);
        }

        char* dup = strdup(argv[++i]);
        if (!dup) {
          fprintf(stderr, "Memory allocation failed\n");
          free_options(opts);
          exit(1);
        }

        opts->patterns[opts->patterns_count++] = dup;
        opts->e = 1;
        has_pattern = 1;
        continue;
      }

      if (strcmp(arg, "-f") == 0) {
        if (i + 1 >= argc) {
          fprintf(stderr, "grep: option requires an argument -- f\n");
          free_options(opts);
          exit(1);
        }

        opts->f = 1;
        load_patterns_from_file(argv[++i], opts);
        has_pattern = 1;
        continue;
      }

      for (int j = 1; arg[j]; j++) {
        switch (arg[j]) {
          case 'i':
            opts->i = 1;
            break;
          case 'v':
            opts->v = 1;
            break;
          case 'c':
            opts->c = 1;
            break;
          case 'l':
            opts->l = 1;
            break;
          case 'n':
            opts->n = 1;
            break;
          case 'h':
            opts->h = 1;
            break;
          case 's':
            opts->s = 1;
            break;
          case 'o':
            opts->o = 1;
            break;

          default:
            fprintf(stderr, "grep: invalid option -- %c\n", arg[j]);
            free_options(opts);
            exit(1);
        }
      }
    }

    else {
      if (!has_pattern) {
        char* dup = strdup(arg);
        if (!dup) {
          fprintf(stderr, "Memory allocation failed\n");
          free_options(opts);
          exit(1);
        }

        opts->patterns[opts->patterns_count++] = dup;
        has_pattern = 1;
      } else {
        char* dup = strdup(arg);
        if (!dup) {
          fprintf(stderr, "Memory allocation failed\n");
          free_options(opts);
          exit(1);
        }

        opts->files[opts->files_count++] = dup;
      }
    }
  }
}

void process_file(const char* filename, Options* opts) {
  FILE* fp = stdin;
  if (filename != NULL) {
    fp = fopen(filename, "r");
    if (!fp) {
      if (!opts->s) perror(filename);
      return;
    }
  }

  char line[4096];
  int line_num = 0;
  int match_count = 0;
  int has_match = 0;

  while (fgets(line, sizeof(line), fp)) {
    line_num++;

    int matched_any = 0;
    for (int p = 0; p < opts->patterns_count; p++) {
      regex_t regex;
      int flags = REG_EXTENDED;
      if (opts->i) flags |= REG_ICASE;
      if (regcomp(&regex, opts->patterns[p], flags) != 0) continue;
      if (regexec(&regex, line, 0, NULL, 0) == 0) {
        matched_any = 1;
        regfree(&regex);
        break;
      }
      regfree(&regex);
    }

    int matched = opts->v ? !matched_any : matched_any;

    if (matched) {
      has_match = 1;
      match_count++;
    }

    if (!opts->c && !opts->l) {
      if (!matched) continue;

      if (opts->o && !opts->v) {
        print_matches_o(line, opts, filename, line_num);
        continue;
      }

      if (filename != NULL && opts->files_count > 1 && !opts->h)
        printf("%s:", filename);
      if (opts->n) printf("%d:", line_num);
      printf("%s", line);
      if (line[strlen(line) - 1] != '\n') printf("\n");
    }
  }

  if (opts->c) {
    if (filename != NULL && opts->files_count > 1 && !opts->h)
      printf("%s:", filename);
    printf("%d\n", match_count);
  }

  if (opts->l && has_match) {
    printf("%s\n", filename);
  }

  if (filename != NULL) fclose(fp);
}

void free_options(Options* opts) {
  for (int i = 0; i < opts->patterns_count; i++) {
    free(opts->patterns[i]);
  }

  for (int i = 0; i < opts->files_count; i++) {
    free(opts->files[i]);
  }

  free(opts->patterns);
  free(opts->files);
}

void load_patterns_from_file(const char* filename, Options* opts) {
  FILE* fp = fopen(filename, "r");
  if (!fp) {
    if (!opts->s) {
      perror(filename);
    }
    return;
  }

  char buffer[4096];

  while (fgets(buffer, sizeof(buffer), fp)) {
    size_t len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }

    if (buffer[0] == '\0') {
      continue;
    }

    char* dup = strdup(buffer);
    if (!dup) {
      fprintf(stderr, "Memory allocation failed\n");
      fclose(fp);
      free_options(opts);
      exit(1);
    }
    opts->patterns[opts->patterns_count++] = dup;
  }
}
void print_matches_o(const char* line, Options* opts, const char* filename,
                     int line_num) {
  regex_t regexes[100];
  int regex_count = opts->patterns_count;
  int flags = REG_EXTENDED;

  if (opts->i) flags |= REG_ICASE;

  for (int i = 0; i < regex_count; i++) {
    if (regcomp(&regexes[i], opts->patterns[i], flags) != 0) {
      regex_count = i;
      break;
    }
  }

  const char* ptr = line;

  while (*ptr) {
    int best_start = -1;
    int best_end = -1;

    for (int i = 0; i < regex_count; i++) {
      regmatch_t match;

      if (regexec(&regexes[i], ptr, 1, &match, 0) == 0) {
        int start = match.rm_so;
        int end = match.rm_eo;

        if (best_start == -1 || start < best_start) {
          best_start = start;
          best_end = end;
        }
      }
    }

    if (best_start == -1) break;

    if (opts->files_count > 1 && !opts->h) printf("%s:", filename);

    if (opts->n) printf("%d:", line_num);

    for (int i = best_start; i < best_end; i++) putchar(ptr[i]);

    putchar('\n');

    if (best_end == 0) break;

    ptr += best_end;
  }

  for (int i = 0; i < regex_count; i++) {
    regfree(&regexes[i]);
  }
}
