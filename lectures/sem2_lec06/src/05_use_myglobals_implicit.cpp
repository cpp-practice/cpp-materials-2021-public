#include <stdio.h>

extern "C" int foo();

int main() {
    printf("Implicit linking code: %i\n", foo());
}
