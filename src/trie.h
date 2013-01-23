#ifndef __TRIE_H
#define __TRIE_H

#include "defs.h"
#include "node.h"
#include "strings.h"

typedef struct node* trie;

trie make_trie();
trie insert(trie root, char* str);
bool contains(trie root, char* word);
strings* complete(trie root, char* prefix);

#endif
