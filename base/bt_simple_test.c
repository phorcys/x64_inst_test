#include <stdio.h>
#include <stdint.h>

int main() {
    uint64_t val = 0x123456789ABCDEF0;
    uint64_t bit_offset = 32;
    uint8_t result;
    uint64_t expected = (val >> bit_offset) & 1;

    asm volatile (
        "bt %[bit], %[val]\n\t"
        "setc %[res]"
        : [res] "=r" (result)
        : [val] "r" (val), [bit] "r" (bit_offset)
        : "cc"
    );

    printf("BT(0x%016lx, %lu) = %d, expected %lu\n", 
           val, bit_offset, result, expected);
    return 0;
}
