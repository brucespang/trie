#ifndef __STRINGS_H
#define __STRINGS_H

#include <stdint.h>
#include <stdlib.h>
#include "string.h"

typedef struct strings strings;

strings* make_strings(uint32_t length);
string* string_at(strings* strs, uint32_t i);
uint32_t strings_length(strings* strs);

#endif
