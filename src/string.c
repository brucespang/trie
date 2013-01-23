#include "string.h"

string* make_string() {
  return malloc(sizeof(string));
}

char last_char(string* s) {
  return s->value[s->length];
}

char remove_last_char(string* s) {
  if(s->length == 0) {
    return '\0';
  } else {
    s->length--;
    return s->value[s->length];
  }
}

void insert_char_at(string* s, uint8_t i, char c) {
  s->value[i] = c;
  if(i >= s->length)
    s->length += (i - s->length + 1);
}

void append_char(string* s, char c) {
  insert_char_at(s, s->length, c);
}

char* to_c_string(string* s) {
  s->value[s->length + 1] = '\0';
  return s->value;
}
