#include <stdio.h>

void vAssertCalled(char* filename, const int linenr) {
   printf("Assert was called in file %s ,line %i\n", filename, linenr);
}

