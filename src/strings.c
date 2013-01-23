#include "strings.h"
#include "string.h"

struct strings {
  uint32_t length;
  string strings[];
};

strings* make_strings(uint32_t length) {
  strings* strs = malloc(sizeof(strings)+length*sizeof(string));
  strs->length = length;
  return strs;
}

string* string_at(strings* strs, uint32_t i) {
  return &strs->strings[i];
}

uint32_t strings_length(strings* strs) {
  return strs->length;
}
