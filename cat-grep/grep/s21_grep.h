//
// Created by Iskander Zakirov on 19.11.2025.
//

#ifndef SCHOOL_21_S21_GREP_H
#define SCHOOL_21_S21_GREP_H

typedef struct {
  int e;  // добавить шаблон
  int i;  // игнор регистра
  int v;  // инверсия совпадений
  int c;  // вывод количества совпадений
  int l;  // вывод имени файла, где есть совпадения
  int n;  // печатать номера строк
  int h;
  int s;
  int f;
  int o;

  char** patterns;     // шаблоны
  int patterns_count;  // кол-во шаблонов

  char** files;     // имена файлов
  int files_count;  // их кол-во
} Options;

void parse_args(int argc, char* argv[], Options* opts);
void process_file(const char* filename, Options* opts);
void free_options(Options* opts);
void load_patterns_from_file(const char* filename, Options* opts);
void print_matches_o(const char* line, Options* opts, const char* filename,
                     int line_num);

#endif  // SCHOOL_21_S21_GREP_H
