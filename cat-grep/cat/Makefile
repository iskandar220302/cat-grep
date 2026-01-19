CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
TARGET = s21_cat
SOURCES = s21_cat.c
HEADERS = s21_cat.h
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean rebuild

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean all

test: $(TARGET)
	./$(TARGET) -b -e file1.txt file2.txt

leaks: $(TARGET)
	leaks --atExit -- ./$(TARGET) -b -e file1.txt file2.txt