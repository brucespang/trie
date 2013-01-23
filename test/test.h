#include <stdio.h>
#include <stdlib.h>
#include "../src/defs.h"

void assert(int b, char* message) {
  if(!b) {
    fprintf(stderr, "Test failure: %s\n", message);
  }
}
