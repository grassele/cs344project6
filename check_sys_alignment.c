#include <stdio.h>
#include <stddef.h>
#include <stdalign.h>

int main(void)
{
    printf("%zu\n", alignof(max_align_t));
}
