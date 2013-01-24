#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <stdint.h>

#include "defs.h"
#include "node.h"
#include "string.h"
#include "strings.h"
#include "trie.h"

trie make_trie() {
  return make_node('\0', 26);
}

trie insert(trie root, char* str) {
  trie* root_ptr = &root;
  trie* n = root_ptr;

  // There's a problem here if str isn't a null terminated string, but there's one
  // everywhere else too (e.g. strlen, printf), so we can probably not ignore it.
  for(; *str != '\0'; str++){
    n = add_child(n, *str);
  }

  set_end_of_word(*n, true);
  return *root_ptr;
}

bool contains(trie root, char* word) {
  trie n = root;

  for(; n != NULL && *word != '\0'; word++) {
    n = get_child(n, *word);
  }

  return n != NULL && end_of_word(n);
}

strings* complete(trie root, char* prefix) {
  uint32_t i, len;
  len = strlen(prefix);
  trie n = root;

  for(i = 0; n != NULL && i < len; i++) {
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
    int i,j;
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
