#include <cstdio>
#include <tpp/TPP.hpp>

int main(const int argc, const char **ppArgv)
{
    printf("Hello World!\n");
    for (unsigned a = 0; a < argc; ++a) printf("%-5d | %s\n", a, ppArgv[a]);
}
