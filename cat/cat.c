#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  bool b;  // нумерует только непустые строки
  bool e;  // также отображает символы конца строки как $
  bool n;  // нумерует все выходные строки
  bool s;  // сжимает несколько смежных пустых строк
  bool t;  // также отображает табы как ^I
  bool v;  // печатает непечатаные символы 
} Flags;

void catSetTable(const char* table[static 256]);
void catSetEndl(const char* table[static 256]);
void catSetTab(const char* table[static 256]);
void catSetNonPrintable(const char* table[static 256]);
Flags readFlags(int argc, char* argv[]);
void cat(FILE* file, Flags flags, const char* table[static 256]);
void openFile(int argc, char* argv[], Flags flags,
              const char* table[static 256]);

int main(int argc, char* argv[]) {
  const char* table[256];
  Flags flags = readFlags(argc, argv);
  catSetTable(table);
  if (flags.e) catSetEndl(table);
  if (flags.t) catSetTab(table);
  if (flags.v) catSetNonPrintable(table);
  openFile(argc, argv, flags, table);
  return 0;
}

Flags readFlags(int argc, char* argv[]) {
  struct option longOptions[] = {{"number-nonblank", 0, NULL, 'b'},
                                 {"number", 0, NULL, 'n'},
                                 {"squeeze-blank", 0, NULL, 's'},
                                 {NULL, 0, NULL, 0}};
  int actualFlag = 0;
  Flags flags = {false, false, false, false, false, false};

  while ((actualFlag =
              getopt_long(argc, argv, "+bevEnstT", longOptions, NULL)) != -1) {
    switch (actualFlag) {
      case 'b':
        flags.b = true;
        break;
      case 'n':
        flags.n = true;
        break;
      case 's':
        flags.s = true;
        break;
      case 'e':
        flags.e = true;
        flags.v = true;
        break;
      case 'E':
        flags.e = true;
        flags.v = true;
        break;
      case 'v':
        flags.v = true;
        break;
      case 't':
        flags.t = true;
        flags.v = true;
        break;
      case 'T':
        flags.t = true;
        flags.v = true;
        break;
    }
  }
  return flags;
}

void cat(FILE* file, Flags flags, const char* table[static 256]) {
  int actualSymbol = 0;
  int lastSymbol = '\n';
  bool s = false;
  int numberLine = 0;

  while (fread(&actualSymbol, 1, 1, file) > 0) {
    if (lastSymbol == '\n') {
      if (flags.s && actualSymbol == '\n') {
        if (s) {
          continue;
        }
        s = true;
      } else {
        s = false;
      }

      if (flags.b) {
        if (actualSymbol != '\n') {
          printf("%6d\t", ++numberLine);
        }
      }

      else if (flags.n)
        printf("%6d\t", ++numberLine);
    }

    if (!actualSymbol && !flags.v) {
      printf("%c", '\0');
    } else {
      printf("%s", table[actualSymbol]);
    }
    lastSymbol = actualSymbol;
  }
}

void openFile(int argc, char* argv[], Flags flags,
              const char* table[static 256]) {
  for (char **fileName = &argv[1], **end = &argv[argc]; fileName != end;
       ++fileName) {
    if (**fileName == '-') {
      continue;
    }
    FILE* file = fopen(*fileName, "rb");
    if (errno) {
      fprintf(stderr, "%s", argv[0]);
      perror(*fileName);
      continue;
    }
    cat(file, flags, table);
    fclose(file);
  }
}

void catSetTable(const char* table[static 256]) {
  const char* rawTable[] = {
      "\00",  "\01",  "\02",  "\03",  "\04",  "\05",  "\06",  "\07",  "\b",
      "\t",   "\n",   "\v",   "\014", "\r",   "\016", "\017", "\020", "\021",
      "\022", "\023", "\024", "\025", "\026", "\027", "\030", "\031", "\032",
      "\033", "\034", "\035", "\036", "\037", " ",    "!",    "\"",   "#",
      "$",    "\%",   "&",    "\'",   "(",    ")",    "*",    "+",    ",",
      "-",    ".",    "/",    "0",    "1",    "2",    "3",    "4",    "5",
      "6",    "7",    "8",    "9",    ":",    ";",    "<",    "=",    ">",
      "?",    "@",    "A",    "B",    "C",    "D",    "E",    "F",    "G",
      "H",    "I",    "J",    "K",    "L",    "M",    "N",    "O",    "P",
      "Q",    "R",    "S",    "T",    "U",    "V",    "W",    "X",    "Y",
      "Z",    "[",    "\\",   "]",    "^",    "_",    "`",    "a",    "b",
      "c",    "d",    "e",    "f",    "g",    "h",    "i",    "j",    "k",
      "l",    "m",    "n",    "o",    "p",    "q",    "r",    "s",    "t",
      "u",    "v",    "w",    "x",    "y",    "z",    "{",    "|",    "}",
      "~",    "\x7f", "\x80", "\x81", "\x82", "\x83", "\x84", "\x85", "\x86",
      "\x87", "\x88", "\x89", "\x8a", "\x8b", "\x8c", "\x8d", "\x8e", "\x8f",
      "\x90", "\x91", "\x92", "\x93", "\x94", "\x95", "\x96", "\x97", "\x98",
      "\x99", "\x9a", "\x9b", "\x9c", "\x9d", "\x9e", "\x9f", "\xa0", "\xa1",
      "\xa2", "\xa3", "\xa4", "\xa5", "\xa6", "\xa7", "\xa8", "\xa9", "\xaa",
      "\xab", "\xac", "\xad", "\xae", "\xaf", "\xb0", "\xb1", "\xb2", "\xb3",
      "\xb4", "\xb5", "\xb6", "\xb7", "\xb8", "\xb9", "\xba", "\xbb", "\xbc",
      "\xbd", "\xbe", "\xbf", "\xc0", "\xc1", "\xc2", "\xc3", "\xc4", "\xc5",
      "\xc6", "\xc7", "\xc8", "\xc9", "\xca", "\xcb", "\xcc", "\xcd", "\xce",
      "\xcf", "\xd0", "\xd1", "\xd2", "\xd3", "\xd4", "\xd5", "\xd6", "\xd7",
      "\xd8", "\xd9", "\xda", "\xdb", "\xdc", "\xdd", "\xde", "\xdf", "\xe0",
      "\xe1", "\xe2", "\xe3", "\xe4", "\xe5", "\xe6", "\xe7", "\xe8", "\xe9",
      "\xea", "\xeb", "\xec", "\xed", "\xee", "\xef", "\xf0", "\xf1", "\xf2",
      "\xf3", "\xf4", "\xf5", "\xf6", "\xf7", "\xf8", "\xf9", "\xfa", "\xfb",
      "\xfc", "\xfd", "\xfe", "\xff"};
  memcpy(table, rawTable, sizeof rawTable);
}

void catSetEndl(const char* table[static 256]) { table['\n'] = "$\n"; }

void catSetTab(const char* table[static 256]) { table['\t'] = "^I"; }
void catSetNonPrintable(const char* table[static 256]) {
  const char* sample1[] = {"^@", "^A", "^B", "^C", "^D",
                           "^E", "^F", "^G", "^H"};
  const char* sample2[] = {"^K", "^L", "^M", "^N",  "^O", "^P", "^Q",
                           "^R", "^S", "^T", "^U",  "^V", "^W", "^X",
                           "^Y", "^Z", "^[", "^\\", "^]", "^^", "^_"};
  const char* sample3[] = {
      "^?",   "M-^@", "M-^A",  "M-^B", "M-^C", "M-^D", "M-^E", "M-^F", "M-^G",
      "M-^H", "M-^I", "M-^J",  "M-^K", "M-^L", "M-^M", "M-^N", "M-^O", "M-^P",
      "M-^Q", "M-^R", "M-^S",  "M-^T", "M-^U", "M-^V", "M-^W", "M-^X", "M-^Y",
      "M-^Z", "M-^[", "M-^\\", "M-^]", "M-^^", "M-^_", "M- ",  "M-!",  "M-\"",
      "M-#",  "M-$",  "M-\%",  "M-&",  "M-'",  "M-(",  "M-)",  "M-*",  "M-+",
      "M-,",  "M--",  "M-.",   "M-/",  "M-0",  "M-1",  "M-2",  "M-3",  "M-4",
      "M-5",  "M-6",  "M-7",   "M-8",  "M-9",  "M-:",  "M-;",  "M-<",  "M-=",
      "M->",  "M-?",  "M-@",   "M-A",  "M-B",  "M-C",  "M-D",  "M-E",  "M-F",
      "M-G",  "M-H",  "M-I",   "M-J",  "M-K",  "M-L",  "M-M",  "M-N",  "M-O",
      "M-P",  "M-Q",  "M-R",   "M-S",  "M-T",  "M-U",  "M-V",  "M-W",  "M-X",
      "M-Y",  "M-Z",  "M-[",   "M-\\", "M-]",  "M-^",  "M-_",  "M-`",  "M-a",
      "M-b",  "M-c",  "M-d",   "M-e",  "M-f",  "M-g",  "M-h",  "M-i",  "M-j",
      "M-k",  "M-l",  "M-m",   "M-n",  "M-o",  "M-p",  "M-q",  "M-r",  "M-s",
      "M-t",  "M-u",  "M-v",   "M-w",  "M-x",  "M-y",  "M-z",  "M-{",  "M-|",
      "M-}",  "M-~",  "M-^?"};
  memcpy(table, sample1, sizeof sample1);
  memcpy(&table[11], sample2, sizeof sample2);
  memcpy(&table[127], sample3, sizeof sample3);
}