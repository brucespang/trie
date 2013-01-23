#include <stdint.h>
#include <glib.h>

#ifndef __NODE_H
#define __NODE_H

typedef struct node node;

node* make_node(char value, uint8_t array_size);

GSList* reversed_suffixes(node* n);

bool has_child(node n, uint8_t b);

node* get_child(node* n, uint8_t b);

node** add_child(node** node_pointer, uint8_t b);

void set_end_of_word(node* n, bool val);

bool end_of_word(node* n);

#endif
