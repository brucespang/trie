#ifndef __STRING_H
#define __STRING_H

#include <stdlib.h>
#include <stdint.h>

#define MAX_STRING_SIZE 64

struct string {
  uint8_t length;
  char value[MAX_STRING_SIZE];
};
typedef struct string string;

string* make_string();

void insert_char_at(string* s, uint8_t i, char c);
void append_char(string* s, char c);
char remove_last_char(string* s);
char* to_c_string(string* s);

#endif
