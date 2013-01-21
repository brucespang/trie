#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <glib.h>
#include <stdint.h>

#define MAX_STRING_SIZE 64

#define bool uint8_t
#define true 1
#define false 0

#define MAX_UNSIGNED_LONG ((uint64_t) (pow(2, 64) - 1))

struct node {
  uint8_t value;
  // Tyler mentioned this ridiculously crazy thing where you can encode information
  // like this in the lower 4 bits of pointers to the node.
  bool end_of_word;
  uint8_t num_children;
  uint8_t array_size;
  uint64_t mask[4];
  struct node* children[];
};
typedef struct node node;

struct string {
  uint8_t length;
  char value[MAX_STRING_SIZE];
};
typedef struct string string;

struct strings {
  uint32_t length;
  string strings[];
};
typedef struct strings strings;

uint8_t arr_popcount(uint64_t* arr, int size) {
  int i, sum;
  sum = 0;

  for(i = 0; i < size; i++) {
    sum += __builtin_popcountl(arr[i]);
  }

  return sum;
}

void set_bit(uint64_t x[], uint8_t n) {
  int offset = n % 64;
  int index = n / 64;
  x[index] |= ((uint64_t) 1) << offset;
}

uint8_t get_bit(uint64_t x[], uint8_t n) {
  int index = n / 64;
  int offset = n % 64;
  uint64_t mask = ((uint64_t) 1) << offset;
  return (x[index] & mask) != 0;
}

void init_node(node* n, uint8_t value) {
  int i;

  n->value = value;
  for(i = 0; i < 4; i++)
    n->mask[i] = 0;
  n->num_children = 0;
  n->array_size = 0;
  n->end_of_word = false;
}

node* make_node(char value, uint8_t array_size) {
  node* res = malloc(sizeof(node) + array_size*sizeof(node*));
  init_node(res, value);
  res->array_size = array_size;
  return res;
}

uint8_t child_index_of(node n, uint8_t b) {
  uint64_t mask_below_b[4];
  int i;

  for(i = 0; i < 4; i++) {
    if(i > (b / 64)) {
      mask_below_b[i] = 0;
    } else if(i < (b / 64)) {
      mask_below_b[i] = n.mask[i];
    } else {
      int offset = b % 64;
      // e.g. 00101 & (11111 -> 11100 -> 00011) -> 00001
      mask_below_b[i] = n.mask[i] & ~(MAX_UNSIGNED_LONG << offset);
    }
  }

  return arr_popcount(mask_below_b, 4);
}

bool has_child(node n, uint8_t b) {
  return get_bit(n.mask, b);
}

node* resize_node(node* n, uint8_t new_size) {
  n->array_size = new_size;
  return realloc(n, sizeof(node) + new_size*sizeof(node*));
}

// This pointer to a pointer should be a pointer to the node's parent's pointer to
// the node. This way if we realloc the node and it moves, we can adjust the
// parent's pointer and keep the tree intact.
node** add_child(node** node_pointer, uint8_t b) {
  node* n = *node_pointer;
  uint8_t index = child_index_of(*n, b);

  if(!has_child(*n, b)) {
    set_bit(n->mask, b);

    if(index >= n->array_size) {
      // We need the +1 so that we actually resize a node with no children
      *node_pointer = resize_node(n, 2 * n->array_size + 1);
      n = *node_pointer;
    }

    // If this is true, we're inserting a child out of order, and need to make some
    // space for it.
    if(index < n->num_children) {
      uint8_t i;
      *node_pointer = resize_node(n, n->array_size + 1);
      n = *node_pointer;

      for(i = n->num_children; i > index && i != 0; i--) {
	n->children[i] = n->children[i-1];
      }
    }

    n->children[index] = make_node(b, 1);
    n->num_children++;
  }

  return &n->children[index];
}

node* insert(node* root, char* value) {
  uint32_t i;
  node** root_ptr = &root;
  node** n = root_ptr;

  for(i = 0; i < strlen(value); i++) {
    n = add_child(n, value[i]);
  }

  (*n)->end_of_word = true;
  return *root_ptr;
}

GSList* reversed_suffixes(node* n) {
  GSList* res = NULL;
  int i;

  if(n->end_of_word) {
    string* s = malloc(sizeof(string));
    s->value[0] = n->value;
    s->length++;
    res = g_slist_prepend(res, s);
  }

  for(i = 0; i < n->num_children; i++) {
    GSList* child_res = reversed_suffixes(n->children[i]);

    while(g_slist_length(child_res) > 0) {
      string* str = (string*) g_slist_nth_data(child_res, 0);
      str->value[str->length] = n->value;
      str->length++;
      res = g_slist_prepend(res, str);
      child_res = g_slist_remove(child_res, str);
    }
  }

  return res;
}

bool contains(node* root, char* word) {
  uint32_t i;
  node* n = root;

  for(i = 0; n != NULL && i < strlen(word); i++) {
    uint8_t b = word[i];
    n = n->children[child_index_of(*n, b)];
  }

  return n != NULL && n->end_of_word;
}

strings* complete(node* root, char* prefix) {
  uint32_t i;
  node* n = root;

  for(i = 0; n != NULL && i < strlen(prefix); i++) {
    uint8_t b = prefix[i];
    n = n->children[child_index_of(*n, b)];
  }

  if(n == NULL) {
    return NULL;
  } else {
    GSList* suffixes = reversed_suffixes(n);
    uint32_t len, prefix_len;
    len = g_slist_length(suffixes);
    prefix_len = strlen(prefix);
    strings* res = malloc(sizeof(strings)+len*sizeof(string));
    res->length = len;
    string* str;
    uint32_t j;

    // Loop from length to 0, inclusive. We can't check if i >= 0, since i is unsigned.
    for(i = len; i--;) {
      str = g_slist_nth_data(suffixes, 0);

      res->strings[i].value[str->length + prefix_len] = '\0';
      // We don't want to include the last item in the prefix (since it's going to be in the generated suffixes)
      for(j = 0; j < prefix_len-1; j++) {
	res->strings[i].value[j] = prefix[j];
	res->strings[i].length++;
      }
      for(j = prefix_len-1; str->length--; j++) {
	res->strings[i].value[j] = str->value[str->length];
        res->strings[i].length++;
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
  node* root = make_node('\0', 26);

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
  for(k = 0; k < res->length; k++) {
    printf("%s\n", res->strings[k].value);
  }

  return 0;
}