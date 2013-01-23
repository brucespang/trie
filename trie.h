#ifndef __TRIE_H
#define __TRIE_H

#include "defs.h"
#include "node.h"

typedef struct node* trie;

trie insert(trie root, char* str);
bool contains(trie root, char* word);
strings* complete(trie root, char* prefix);

#endif
