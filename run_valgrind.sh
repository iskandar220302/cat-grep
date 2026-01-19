#!/bin/bash
set -e

echo "=============================="
echo "  Valgrind: s21_cat"
echo "=============================="

cd /project/src/cat

valgrind \
  --leak-check=full \
  --show-leak-kinds=all \
  --errors-for-leak-kinds=definite \
  --error-exitcode=1 \
  ./s21_cat -e -b file1.txt file4.txt > /dev/null

echo "✔ s21_cat: no leaks"

echo
echo "=============================="
echo "  Valgrind: s21_grep"
echo "=============================="

cd /project/src/grep

valgrind \
  --leak-check=full \
  --show-leak-kinds=all \
  --errors-for-leak-kinds=definite \
  --error-exitcode=1 \
  ./s21_grep "main" s21_grep.c > /dev/null

echo "✔ s21_grep: no leaks"
echo
echo "🎉 All Valgrind checks passed"
