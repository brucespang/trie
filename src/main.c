#include <stdio.h>
#include <stdlib.h>

#include "strings.h"
#include "string.h"
#include "trie.h"

int main(int argc, char* argv[]) {
  uint32_t i;
  FILE* in;
  char* line = malloc(MAX_STRING_SIZE);
  trie root = make_trie();

  if(argc == 1) {
    fprintf(stderr, "Usage: %s /path/to/word-list\n", argv[0]);
    return 1;
  }

  while(argc-- > 1) {
    in = fopen(argv[argc], "r");

    if(in == NULL) {
      fprintf(stderr, "Error opening: %s\n", argv[argc]);
      return 1;
    }

    while(fgets(line,MAX_STRING_SIZE,in) != NULL) {
      for(i = 0; i < MAX_STRING_SIZE; i++) {
      	if(line[i] == '\n') {
      	  line[i] = '\0';
      	}
      }

      root = insert(root, line);
    }

    fclose(in);
  }

  strings* res = complete(root, "a");
  for(i = 0; i < strings_length(res); i++) {
    printf("%s\n", to_c_string(string_at(res, i)));
  }

  return 0;
}
