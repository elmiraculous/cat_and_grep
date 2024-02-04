#include "s21_grep.h"

Flags ReadFlags(int argc, char *argv[]) {
  struct option longOptions[] = {{NULL, 0, NULL, 0}};
  int currentFlag = 0;
  Flags flags = {{},    0,     0,     false, false, false, false,
                 false, false, false, false, false, false};

  while ((currentFlag = getopt_long(argc, argv, "e:ivclnhsf:o", longOptions,
                                    NULL)) != -1) {
    switch (currentFlag) {
      case 'e':
        flags.e = true;
        pattern_add(&flags, optarg);
        break;
      case 'i':
        flags.i |= REG_ICASE;
        break;
      case 'v':
        flags.v = true;
        break;
      case 'c':
        flags.c = true;
        break;
      case 'l':
        flags.l = true;
        break;
      case 'n':
        flags.n = true;
        break;
      case 'h':
        flags.h = true;
        break;
      case 's':
        flags.s = true;
        break;
      case 'f':
        flags.f = true;
        add_reg_from_file(&flags, optarg);
        break;
      case 'o':
        flags.o = true;
        break;
    }
  }

  if (flags.len == 0) {
    pattern_add(&flags, argv[optind]);
    optind++;
  }

  if (argc - optind == 1) {
    flags.h = true;
  }

  return flags;
}

void output_line(char *line, int n) {
  for (int i = 0; i < n; i++) {
    putchar(line[i]);
  }
  if (line[n - 1] != '\n') {
    putchar('\n');
  }
}

void pattern_add(Flags *flags, char *pattern) {
  if (flags->len != 0) {
    strcat(flags->pattern + flags->len, "|");
    flags->len++;
  }
  flags->len += sprintf(flags->pattern + flags->len, "(%s)", pattern);
}

void add_reg_from_file(Flags *flags, char *filepath) {
  FILE *f = fopen(filepath, "r");
  if (f == NULL) {
    if (!flags->s) {
      perror(filepath);
      exit(1);
    }
  } else {
    char *line = NULL;
    size_t memlen = 0;
    int read = getline(&line, &memlen, f);
    while (read != -1 && f != NULL) {
      if (line[read - 1] == '\n') {
        line[read - 1] = '\0';
      }
      pattern_add(flags, line);
      read = getline(&line, &memlen, f);
    }
    free(line);
  }
  fclose(f);
}

void print_match(regex_t *re, char *line) {
  regmatch_t match;
  int offset = 0;
  while (1) {
    int result = regexec(re, line + offset, 1, &match, 0);
    if (result != 0) {
      break;
    }
    for (int i = match.rm_so; i < match.rm_eo; i++) {
      putchar(line[offset + i]);
    }
    putchar('\n');
    offset += match.rm_eo;
  }
}
void openFile(Flags flags, char *path, regex_t *reg) {
  FILE *f = fopen(path, "r");
  int c = 0;
  if (f == NULL) {
    if (!flags.s) {
      perror(path);
      exit(1);
    }
  } else {
    char *line = NULL;
    int line_count = 1;
    size_t memlen = 0;
    int read = getline(&line, &memlen, f);
    while (read != -1) {
      int result = regexec(reg, line, 0, NULL, 0);
      if ((result == 0 && !flags.v) || (result != 0 && flags.v)) {
        if (!flags.c && !flags.l) {
          if (!flags.h) {
            printf("%s:", path);
          }
          if (flags.n) {
            printf("%d:", line_count);
          }
          if (flags.o && flags.v) {
            output_line(line, read);
          }
          if (flags.o) {
            print_match(reg, line);
          } else {
            output_line(line, read);
          }
        }
        c++;
      }
      read = getline(&line, &memlen, f);
      line_count++;
    }
    free(line);
  }
  if (flags.l && flags.c && c != 0) {
    c = 1;
  }

  if (flags.c) {
    if (!flags.h) {
      printf("%s:", path);
    }
    printf("%d\n", c);
  }

  if (flags.l && c > 0) {
    printf("%s\n", path);
  }

  fclose(f);
}

void output(Flags flags, int argc, char **argv) {
  regex_t re;
  int error = regcomp(&re, flags.pattern, REG_EXTENDED | flags.i);
  if (error) {
    perror("Error");
  }
  for (int i = optind; i < argc; i++) {
    openFile(flags, argv[i], &re);
  }
  regfree(&re);
}

int main(int argc, char **argv) {
  Flags flags = ReadFlags(argc, argv);
  output(flags, argc, argv);
  return 0;
}
