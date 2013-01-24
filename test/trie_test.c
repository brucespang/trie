#include "../src/defs.h"
#include "../src/trie.h"
#include "test.h"

int main() {
  trie t = make_trie();
  t = insert(t, "abt");
  t = insert(t, "abc");
  t = insert(t, "test");
  t = insert(t, "katrina");
  t = insert(t, "cat");

  assert(contains(t, "abt"), "contains abt");
  assert(contains(t, "abc"), "contains abc");
  assert(contains(t, "test"), "contains test");
  assert(contains(t, "katrina"), "contains katrina");
  assert(contains(t, "cat"), "contains cat");
  assert(!contains(t, "asdf"), "does not contain asdf");
  assert(!contains(t, "ca"), "does not contain ca");

  /* assert("abc" == complete(t, "a")[0], "abc is first completion for a"); */
  /* assert("abc" == complete(t, "ab")[0], "abc is first completion for a"); */
  /* assert("abt" == complete(t, "a")[1], "abc is second completion for ab"); */
  /* assert("test" == complete(t, "t")[0], "test is a completion for t"); */
  /* assert("katrina" == complete(t, "k")[0], "katrina is a completion for k"); */
  /* assert("cat" == complete(t, "c")[0], "cat is a completion for c"); */

  return 0;
}
