//
// Created by Iskander Zakirov on 15.11.2025.
//

#include "s21_cat.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  Flags flags;

  parser(argc, argv, &flags);

  if (flags.error) {
    fprintf(stderr, "cat: illegal option\n");
    return 1;
  }

  cat(argc, argv, &flags);

  return 0;
}

void parser(int argc, char* argv[], Flags* flags) {
  memset(flags, 0, sizeof(Flags));

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == '-') {
        if (strcmp(argv[i], "--number-nonblank") == 0) {
          flags->b = 1;
        } else if (strcmp(argv[i], "--number") == 0) {
          flags->n = 1;
        } else if (strcmp(argv[i], "--squeeze-blank") == 0) {
          flags->s = 1;
        } else {
          fprintf(stderr, "cat: unrecognized option '%s'\n", argv[i]);
          flags->error = 1;
          return;
        }
      } else {
        for (int j = 1; argv[i][j] != '\0'; j++) {
          switch (argv[i][j]) {
            case 'b':
              flags->b = 1;
              break;
            case 'e':
              flags->e = 1;
              flags->v = 1;
              break;
            case 'E':
              flags->e = 1;
              break;
            case 'n':
              flags->n = 1;
              break;
            case 's':
              flags->s = 1;
              break;
            case 't':
              flags->t = 1;
              flags->v = 1;
              break;
            case 'T':
              flags->t = 1;
              break;
            case 'v':
              flags->v = 1;
              break;
            default:
              fprintf(stderr, "cat: invalid option -- '%c'\n", argv[i][j]);
              flags->error = 1;
              return;
          }
        }
      }
    }
  }

  if (flags->b) {
    flags->n = 0;
  }
}

static void print_with_v(unsigned char ch, Flags* flags) {
  if (flags->t && ch == '\t') {
    printf("^I");
    return;
  }

  if (flags->e && ch == '\n') {
    if (flags->v) {
      putchar('$');
      putchar('\n');
    } else {
      putchar('$');
      putchar(ch);
    }
    return;
  }

  if (flags->v) {
    if (ch == 127) {
      printf("^?");
    } else if (ch >= 128 && ch <= 159) {
      printf("M-^%c", ch - 64);
    } else if (ch >= 160 && ch <= 254) {
      printf("M-%c", ch - 128);
    } else if (ch == 255) {
      printf("M-^?");
    } else if (ch < 32 && ch != 9 && ch != 10) {
      printf("^%c", ch + 64);
    } else if (ch == 9) {
      putchar('\t');
    } else if (ch == 10) {
      putchar('\n');
    } else {
      putchar(ch);
    }
  } else {
    if (flags->e && ch == '\n') {
      putchar('$');
    }

    if (flags->t && ch == '\t') {
      printf("^I");
    } else {
      putchar(ch);
    }
  }
}

void cat(int argc, char* argv[], Flags* flags) {
  int number_of_files = 1;

  while (number_of_files < argc && argv[number_of_files][0] == '-') {
    number_of_files++;
  }

  if (number_of_files >= argc) {
    int ch;
    int prev = '\n';
    int current_line = 1;
    int squeeze = 0;
    unsigned char uc;

    while ((ch = getchar()) != EOF) {
      uc = (unsigned char)ch;

      if (flags->s) {
        if (ch == '\n' && prev == '\n') {
          if (squeeze == 1) {
            prev = ch;
            continue;
          }
          squeeze++;
        } else {
          squeeze = 0;
        }
      }

      // Обработка нумерации строк (флаги -b и -n);
      // if (flags->b) {
      //     if (prev == '\n' && ch != '\n') {
      //         printf("%6d\t", current_line++);
      //     }
      //
      // } else if (flags->n && prev == '\n') {
      //     printf("%6d\t", current_line++);
      // }

      if (prev == '\n') {
        if (flags->b) {
          if (ch != '\n') {
            printf("%6d\t", current_line++);
          } else if (flags->e || flags->n) {
            printf("      \t");
          }
        } else if (flags->n) {
          printf("%6d\t", current_line++);
        }
      }

      print_with_v(uc, flags);

      prev = ch;
    }
  } else {
    while (number_of_files < argc) {
      char* path = argv[number_of_files];

      if (access(path, F_OK) != 0) {
        fprintf(stderr, "cat: %s: No such file or directory\n", path);
        number_of_files++;
        continue;
      }

      FILE* filename = fopen(path, "r");
      if (!filename) {
        fprintf(stderr, "cat: %s: Permission denied\n", path);
        number_of_files++;
        continue;
      }

      int ch;
      int prev = '\n';
      int current_line = 1;
      int squeeze = 0;
      unsigned char uc;

      while ((ch = fgetc(filename)) != EOF) {
        uc = (unsigned char)ch;

        if (flags->s) {
          if (ch == '\n' && prev == '\n') {
            if (squeeze == 1) {
              prev = ch;
              continue;
            }
            squeeze++;
          } else {
            squeeze = 0;
          }
        }

        // Обработка нумерации строк (флаги -b и -n)
        // if (flags->b) {
        //     if (prev == '\n' && ch != '\n') {
        //         printf("%6d\t", current_line++);
        //     }
        //
        // } else if (flags->n && prev == '\n') {
        //     printf("%6d\t", current_line++);
        // }

        if (prev == '\n') {
          if (flags->b) {
            if (ch != '\n') {
              printf("%6d\t", current_line++);
            } else if (flags->e || flags->n) {
              printf("      \t");
            }
          } else if (flags->n) {
            printf("%6d\t", current_line++);
          }
        }

        print_with_v(uc, flags);

        prev = ch;
      }

      fclose(filename);
      number_of_files++;
    }
  }
}