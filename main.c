#include <stdio.h>
#include <stdlib.h>

#include "strings.h"
#include "string.h"
#include "trie.h"

int main(int argc, char* argv[]) {
  int i,j;
  FILE* in;
  char* line = malloc(MAX_STRING_SIZE);
  trie root = make_node('\0', 26);

  if(argc == 1) {
    fprintf(stderr, "Usage: %s /path/to/word-list\n", argv[0]);
    return 1;
  }

  for(i = 1; i < argc; i++) {
    in = fopen(argv[i], "r");

    if(in == NULL) {
      fprintf(stderr, "Error opening: %s\n", argv[i]);
      return 1;
    }

    while(fgets(line,MAX_STRING_SIZE,in) != NULL) {
      for(j = 0; j < MAX_STRING_SIZE; j++) {
      	if(line[j] == '\n') {
      	  line[j] = '\0';
      	}
      }

      root = insert(root, line);
    }

    fclose(in);
  }

  unsigned int k;
  strings* res = complete(root, "a");
  for(k = 0; k < strings_length(res); k++) {
    printf("%s\n", to_c_string(string_at(res, k)));
  }

  return 0;
}
