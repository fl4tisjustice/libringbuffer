#include <stdio.h>
#include <assert.h>

#include "ringbuffer_interface.h"

int main() {
    ringbuffer_handle handle = ringbuffer_create(10, sizeof(int));
    
    int a = 10;

    printf("%lu\n", LENGTH(handle, int));

    if (!ENQUEUE(handle, &a)) goto cleanup;
    
    printf("%lu\n", LENGTH(handle, int));
    
    if (!POP(handle, &a)) goto cleanup;
    
    printf("%lu\n", LENGTH(handle, int));

    printf("%lu\n", CAPACITY(handle, int));
    
    // printf("%d\n", (*(int**)(void**)handle)[((size_t*)(void*)handle)[1] / 4]);

cleanup:
    ringbuffer_free(handle);
}