#include "base.h"
#include <stdint.h>
#include <stdio.h>

void test_bswap_32bit() {
    printf("\nTesting BSWAP 32-bit\n");
    
    uint32_t values[] = {
        0x00000000, 0x12345678, 0xAABBCCDD, 0x11223344,
        0xDEADBEEF, 0x00FF00FF, 0x0F0F0F0F, 0xFFFFFFFF
    };
    
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        uint32_t input = values[i];
        uint32_t result;
        
        asm volatile (
            "movl %1, %%eax\n\t"
            "bswap %%eax\n\t"
            "movl %%eax, %0"
            : "=r" (result)
            : "r" (input)
            : "eax"
        );
        
        uint32_t expected = 
            ((input & 0xFF000000) >> 24) |
            ((input & 0x00FF0000) >> 8)  |
            ((input & 0x0000FF00) << 8)  |
            ((input & 0x000000FF) << 24);
        
        printf("BSWAP(0x%08x) = 0x%08x [%s]\n", 
               input, result, 
               result == expected ? "PASS" : "FAIL");
    }
}

void test_bswap_64bit() {
    printf("\nTesting BSWAP 64-bit\n");
    
    uint64_t values[] = {
        0x0000000000000000, 0x1122334455667788, 
        0xAABBCCDDEEFF0011, 0xDEADBEEFCAFEBABE,
        0x00FF00FF00FF00FF, 0x0F0F0F0F0F0F0F0F,
        0xFFFFFFFFFFFFFFFF, 0x1234567890ABCDEF
    };
    
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        uint64_t input = values[i];
        uint64_t result;
        
        asm volatile (
            "movq %1, %%rax\n\t"
            "bswap %%rax\n\t"
            "movq %%rax, %0"
            : "=r" (result)
            : "r" (input)
            : "rax"
        );
        
        uint64_t expected = 
            ((input & 0xFF00000000000000) >> 56) |
            ((input & 0x00FF000000000000) >> 40) |
            ((input & 0x0000FF0000000000) >> 24) |
            ((input & 0x000000FF00000000) >> 8)  |
            ((input & 0x00000000FF000000) << 8)  |
            ((input & 0x0000000000FF0000) << 24) |
            ((input & 0x000000000000FF00) << 40) |
            ((input & 0x00000000000000FF) << 56);
        
        printf("BSWAP(0x%016lx) = 0x%016lx [%s]\n", 
               input, result, 
               result == expected ? "PASS" : "FAIL");
    }
}

int main() {
    test_bswap_32bit();
    test_bswap_64bit();
    
    printf("\nBSWAP tests completed.\n");
    return 0;
}
