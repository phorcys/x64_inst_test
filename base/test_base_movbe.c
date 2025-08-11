#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("MOVBE instruction test\n");
    printf("======================\n");
    
    int errors = 0;
    
    // Test 16-bit MOVBE
    uint16_t in16 = 0x1234;
    uint16_t out16;
    uint16_t mem16;
    
    // MOVBE from register to memory
    __asm__ __volatile__ (
        "movbe %1, %0"
        : "=m" (mem16)
        : "r" (in16)
    );
    
    // MOVBE from memory to register
    __asm__ __volatile__ (
        "movbe %1, %0"
        : "=r" (out16)
        : "m" (mem16)
    );
    
    if (out16 != in16) {
        printf("Error: 16-bit MOVBE expected 0x%x, got 0x%x\n", in16, out16);
        errors++;
    } else {
        printf("16-bit MOVBE: PASS\n");
    }
    
    // Test 32-bit MOVBE
    uint32_t in32 = 0x12345678;
    uint32_t out32;
    uint32_t mem32;
    
    __asm__ __volatile__ (
        "movbe %1, %0"
        : "=m" (mem32)
        : "r" (in32)
    );
    
    __asm__ __volatile__ (
        "movbe %1, %0"
        : "=r" (out32)
        : "m" (mem32)
    );
    
    if (out32 != in32) {
        printf("Error: 32-bit MOVBE expected 0x%x, got 0x%x\n", in32, out32);
        errors++;
    } else {
        printf("32-bit MOVBE: PASS\n");
    }
    
    // Test 64-bit MOVBE
    uint64_t in64 = 0x123456789ABCDEF0;
    uint64_t out64;
    uint64_t mem64;
    
    __asm__ __volatile__ (
        "movbe %1, %0"
        : "=m" (mem64)
        : "r" (in64)
    );
    
    __asm__ __volatile__ (
        "movbe %1, %0"
        : "=r" (out64)
        : "m" (mem64)
    );
    
    if (out64 != in64) {
        printf("Error: 64-bit MOVBE expected 0x%lx, got 0x%lx\n", in64, out64);
        errors++;
    } else {
        printf("64-bit MOVBE: PASS\n");
    }
    
    // Test with unaligned memory
    uint8_t buffer[10] = {0};
    uint32_t unaligned_in = 0xAABBCCDD;
    uint32_t unaligned_out;
    
    // Store to unaligned address (buffer+1)
    __asm__ __volatile__ (
        "movbe %1, %0"
        : "=m" (*(uint32_t*)(buffer+1))
        : "r" (unaligned_in)
    );
    
    // Load from unaligned address
    __asm__ __volatile__ (
        "movbe %1, %0"
        : "=r" (unaligned_out)
        : "m" (*(uint32_t*)(buffer+1))
    );
    
    if (unaligned_out != unaligned_in) {
        printf("Error: Unaligned MOVBE expected 0x%x, got 0x%x\n", unaligned_in, unaligned_out);
        errors++;
    } else {
        printf("Unaligned MOVBE: PASS\n");
    }
    
    printf("\nMOVBE test completed\n");
    printf("====================\n");
    printf("Total tests: 4\n");
    printf("Passed: %d\n", 4 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
