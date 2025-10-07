#include <stdio.h>
#include <assert.h>

#include "ringbuffer_interface.h"

int main() {
    ringbuffer_handle handle = ringbuffer_create(10, sizeof(int));

    printf("%zu", LENGTH(handle, int));

    int a = 10;

    if (!ENQUEUE(handle, &a)) goto cleanup;
    
    if (!POP(handle, &a)) goto cleanup;
    
    printf("%d\n", (*(int**)(void**)handle)[((size_t*)(void*)handle)[1] / 4]);

cleanup:
    ringbuffer_free(handle);
}