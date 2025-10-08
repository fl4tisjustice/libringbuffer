#include <stdio.h>
#include <assert.h>

#include "ringqueue_interface.h"

int main() {
    ringqueue_handle handle = ringqueue_create(10, sizeof(int));

    printf("%zu\n", LENGTH(handle, int));

    int a = 10;

    if (!ENQUEUE(handle, &a)) goto cleanup;
    
    if (!POP(handle, &a)) goto cleanup;
    
    printf("%d\n", (*(int**)(void**)handle)[((size_t*)(void*)handle)[1] / 4]);

cleanup:
    ringqueue_free(handle);
}