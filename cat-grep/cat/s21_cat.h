//
// Created by Iskander Zakirov on 15.11.2025.
//

#ifndef S21_CAT_H
#define S21_CAT_H

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
  int error;
} Flags;

static void print_with_v(unsigned char ch, Flags* flags);
void parser(int argc, char* argv[], Flags* flags);
void cat(int argc, char* argv[], Flags* flags);

#endif
// SCHOOL_21_S21_CAT_H