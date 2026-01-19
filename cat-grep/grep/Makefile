CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -D_GNU_SOURCE
TARGET = s21_grep
SOURCES = s21_grep.c
HEADERS = s21_grep.h
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean rebuild test leaks

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean all

test: $(TARGET)
	@echo "=== Basic tests ==="
	@./$(TARGET) "pattern" test1.txt 2>/dev/null || true
	@echo "=== Test with -i flag ==="
	@./$(TARGET) -i "PATTERN" test1.txt 2>/dev/null || true
	@echo "=== Test with -v flag ==="
	@./$(TARGET) -v "pattern" test1.txt 2>/dev/null || true

leaks: $(TARGET)
	leaks --atExit -- ./$(TARGET) "pattern" test1.txt 2>/dev/null || true

# Создание тестовых файлов
test_files:
	@echo "Creating test files..."
	@echo "line with pattern" > test1.txt
	@echo "another line" >> test1.txt
	@echo "LINE WITH PATTERN" >> test1.txt
	@echo "pattern at start" > test2.txt
	@echo "end pattern" >> test2.txt

# Проверка стиля кода (если есть cppcheck)
style:
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --suppress=missingIncludeSystem $(SOURCES) 2>/dev/null; \
		echo "Style check completed"; \
	else \
		echo "cppcheck not installed, skipping style check"; \
	fi

# Быстрая проверка
check: $(TARGET) test_files
	@echo "Quick check..."
	@./$(TARGET) "pattern" test1.txt test2.txt
	@echo "---"
	@./$(TARGET) -n "pattern" test1.txt

# Очистка тестовых файлов
clean_test:
	rm -f test1.txt test2.txt patterns.txt

# Полная очистка
distclean: clean clean_test

help:
	@echo "Available targets:"
	@echo "  all       - Build $(TARGET) (default)"
	@echo "  clean     - Remove object files and executable"
	@echo "  rebuild   - Clean and rebuild"
	@echo "  test      - Run basic tests"
	@echo "  test_files- Create test files"
	@echo "  check     - Quick functionality check"
	@echo "  leaks     - Run with memory leak detection"
	@echo "  style     - Check code style (requires cppcheck)"
	@echo "  clean_test- Remove test files"
	@echo "  distclean - Remove all generated files"
	@echo "  help      - Show this help"

# Установите все как цель по умолчанию
.DEFAULT_GOAL := all