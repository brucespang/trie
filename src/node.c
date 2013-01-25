#include "defs.h"
#include "node.h"
#include "string.h"

#include <glib.h>
#include <stdlib.h>
#include <stdint.h>

struct node {
  uint8_t value;
  bool end_of_word;
  uint8_t num_children;
  uint8_t array_size;
  uint64_t mask[4];
  struct node* children[];
};

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
  uint8_t popcount, index;
  popcount = 0;
  index = b / 64;

  // e.g. 00101 & (11111 -> 11100 -> 00011) -> 00001
  popcount += __builtin_popcountl(n.mask[index] & ~(MAX_UNSIGNED_LONG << b % 64));
  while(index--) {
      popcount += __builtin_popcountl(n.mask[index]);
  }

  return popcount;
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

bool has_child(node n, uint8_t b) {
  return get_bit(n.mask, b);
}

node* get_child(node* n, uint8_t b) {
  return n->children[child_index_of(*n, b)];
}

// TODO: rewrite with dynamic arrays. The slowest part of each invocation of this
// are the calls to the linked list functions.
GSList* reversed_suffixes(node* n) {
  GSList* res, * child_res;
  string* str;
  int i,len;

  res = NULL;

  if(n->end_of_word) {
    string* s = make_string();
    append_char(s, n->value);
    res = g_slist_prepend(res, s);
  }

  for(i = 0; i < n->num_children; i++) {
    child_res = reversed_suffixes(n->children[i]);

    for(len = g_slist_length(child_res); len > 0; len--) {
      str = g_slist_nth_data(child_res, 0);
      append_char(str, n->value);
      res = g_slist_prepend(res, str);
      child_res = g_slist_remove(child_res, str);
    }
  }

  return res;
}

void set_end_of_word(node* n, bool val) {
  n->end_of_word = val;
}

bool end_of_word(node* n) {
  return n->end_of_word;
}
