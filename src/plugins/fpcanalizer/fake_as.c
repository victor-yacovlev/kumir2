#include <stdio.h>

int main(int argc, char** argv) {
    char buffer [65536u];
    size_t sz;
    fwrite("===BEGIN_FAKE_AS_OUTPUT\n", sizeof(char), 24u, stdout);
    while ( (sz=fread(buffer, sizeof(char), 65536u, stdin)) > 0) {
        fwrite(buffer, sizeof(char), sz, stdout);
    }
    fwrite("===END_FAKE_AS_OUTPUT\n", sizeof(char), 22u, stdout);
    return 0;
}
