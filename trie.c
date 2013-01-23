#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <stdint.h>

#include "defs.h"
#include "node.h"
#include "string.h"
#include "strings.h"

typedef struct node* trie;

trie insert(trie root, char* value) {
  uint32_t i;
  trie* root_ptr = &root;
  trie* n = root_ptr;

  for(i = 0; i < strlen(value); i++) {
    n = add_child(n, value[i]);
  }

  set_end_of_word(*n, true);
  return *root_ptr;
}

bool contains(trie root, char* word) {
  uint32_t i;
  trie n = root;

  for(i = 0; n != NULL && i < strlen(word); i++) {
    n = get_child(n, word[i]);
  }

  return n != NULL && end_of_word(n);
}

strings* complete(trie root, char* prefix) {
  uint32_t i;
  trie n = root;

  for(i = 0; n != NULL && i < strlen(prefix); i++) {
    n = get_child(n, prefix[i]);
  }

  if(n == NULL) {
    return NULL;
  } else {
    GSList* suffixes = reversed_suffixes(n);
    uint32_t len, prefix_len;
    len = g_slist_length(suffixes);
    prefix_len = strlen(prefix);
    strings* res = make_strings(len);
    string* str;
    int j;
    char c;

    // Loop through each suffix. We can't check if i >= 0, since i is unsigned.
    for(i = len; i--;) {
      str = g_slist_nth_data(suffixes, 0);

      // We don't want to include the last item in the prefix
      // (since it's going to be in the generated suffixes)
      for(j = 0; j < (int) prefix_len-1; j++) {
	append_char(string_at(res, i), prefix[j]);
      }
      while((c = remove_last_char(str)) != '\0') {
	append_char(string_at(res, i), c);
      }

      suffixes = g_slist_remove(suffixes, str);
      free(str);
    }

    return res;
  }
}

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
