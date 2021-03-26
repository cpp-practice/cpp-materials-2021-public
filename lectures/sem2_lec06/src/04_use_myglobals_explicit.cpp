#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

// https://www.opennet.ru/man.shtml?topic=dlopen&category=3

int main() {
    void *handle;
    char *error;

    typedef int(*funcType)();
    funcType foo;

    handle = dlopen("libmyglobals.so", RTLD_LAZY);
    if (!handle) {
        fputs(dlerror(), stderr);
        exit(1);
    }

    foo = reinterpret_cast<funcType>(dlsym(handle, "foo"));
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }

    printf ("Explicit linking code:  %i\n", (*foo)());
    dlclose(handle);
}
