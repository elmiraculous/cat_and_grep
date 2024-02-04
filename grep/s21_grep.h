#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char pattern[1024];
  size_t len;
  int reg;
  bool e;  // Шаблон
  int i;   // Игнорирует различия регистра.
  bool v;  // Инвертирует смысл поиска соответствий.
  bool c;  // Выводит только количество совпадающих строк.
  bool l;  // Выводит только совпадающие файлы.
  bool n;  // Предваряет каждую строку вывода номером строки из файла ввода.
  bool h;  // Выводит совпадающие строки, не предваряя их именами файлов.
  bool s;  // Подавляет сообщения об ошибках о несуществующих или нечитаемых
           // файлах.
  bool f;  // Получает регулярные выражения из файла.
  bool o;  // Печатает только совпадающие (непустые) части совпавшей строки.

} Flags;

void pattern_add(Flags *flags, char *pattern);
void add_reg_from_file(Flags *flags, char *filepath);
Flags ReadFlags(int argc, char *argv[]);
void output_line(char *line, int n);
void pattern_add(Flags *flags, char *pattern);
void add_reg_from_file(Flags *flags, char *filepath);
void print_match(regex_t *re, char *line);
void openFile(Flags flags, char *path, regex_t *reg);
void output(Flags flags, int argc, char **argv);

#endif