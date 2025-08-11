#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

// Helper function to print binary representation
void print_binary(uint64_t value, int size) {
    for (int i = size - 1; i >= 0; i--) {
        putchar((value & (1ULL << i)) ? '1' : '0');
        if (i % 4 == 0) putchar(' ');
    }
    printf("\n");
}

int main() {
    printf("SHL instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    
    // Test 8-bit SHL with immediate count
    uint8_t val8 = 0b00001011; // 11
    uint8_t expected8 = 0b00010110; // 22
    __asm__ __volatile__ (
        "shl $1, %0"
        : "+r" (val8)
    );
    
    printf("8-bit SHL (<<1): ");
    print_binary(val8, 8);
    if (val8 != expected8) {
        printf("Error: Expected 0x%02X, got 0x%02X\n", expected8, val8);
        errors++;
    }

    // Test 16-bit SHL with CL count
    uint16_t val16 = 0x1234;
    uint16_t expected16 = 0x2468;
    uint8_t count = 1;
    __asm__ __volatile__ (
        "mov %1, %%cl\n\t"
        "shl %%cl, %0"
        : "+r" (val16)
        : "r" (count)
        : "cl"
    );
    
    printf("16-bit SHL (<<1): 0x%04X\n", val16);
    if (val16 != expected16) {
        printf("Error: Expected 0x%04X, got 0x%04X\n", expected16, val16);
        errors++;
    }

    // Test 32-bit SHL with immediate count >1
    uint32_t val32 = 0x00000001;
    uint32_t expected32 = 0x00000100;
    __asm__ __volatile__ (
        "shl $8, %0"
        : "+r" (val32)
    );
    
    printf("32-bit SHL (<<8): 0x%08X\n", val32);
    if (val32 != expected32) {
        printf("Error: Expected 0x%08X, got 0x%08X\n", expected32, val32);
        errors++;
    }

    // Test 64-bit SHL with masked count (65 masked to 1)
    uint64_t val64 = 0x1;
    uint64_t expected64 = 0x2;
    __asm__ __volatile__ (
        "shl $65, %0"
        : "+r" (val64)
    );
    
    printf("64-bit SHL (<<65 masked to 1): 0x%016lX\n", val64);
    if (val64 != expected64) {
        printf("Error: Expected 0x%016lX, got 0x%016lX\n", expected64, val64);
        errors++;
    }

    // Test memory operand SHL
    uint16_t mem_val = 0x4000;
    uint16_t expected_mem = 0x8000;
    __asm__ __volatile__ (
        "shlw $1, %0"  // Use 'w' suffix for word (16-bit) operation
        : "+m" (mem_val)
    );
    
    printf("Memory SHL (<<1): 0x%04X\n", mem_val);
    if (mem_val != expected_mem) {
        printf("Error: Expected 0x%04X, got 0x%04X\n", expected_mem, mem_val);
        errors++;
    }

    // Test carry flag (CF) after SHL
    uint8_t carry_val = 0x80;
    uint8_t carry_expected = 0x00;
    uint8_t carry_flag = 0;
    __asm__ __volatile__ (
        "shlb $1, %1\n\t"  // Use 'b' suffix for byte (8-bit) operation
        "setc %0"
        : "=r" (carry_flag), "+r" (carry_val)
    );
    
    printf("8-bit SHL CF test: val=0x%02X, CF=%d\n", carry_val, carry_flag);
    if (carry_val != carry_expected || carry_flag != 1) {
        printf("Error: Expected val=0x%02X with CF=1, got val=0x%02X CF=%d\n", 
               carry_expected, carry_val, carry_flag);
        errors++;
    }

    // Test overflow flag (OF) for 1-bit shift
    // Shifting 0x40 (64) left by 1 gives 0x80 (-128) which changes the sign bit
    // OF should be set to 1 in this case
    uint8_t of_val = 0x40;
    uint8_t of_expected = 0x80;
    uint8_t of_flag = 0;
    __asm__ __volatile__ (
        "shlb $1, %1\n\t"  // Use 'b' suffix for byte (8-bit) operation
        "seto %0"
        : "=r" (of_flag), "+r" (of_val)
    );
    
    printf("8-bit SHL OF test: val=0x%02X, OF=%d\n", of_val, of_flag);
    if (of_val != of_expected || of_flag != 1) {
        printf("Error: Expected val=0x%02X with OF=1, got val=0x%02X OF=%d\n", 
               of_expected, of_val, of_flag);
        errors++;
    }

    // Test zero flag (ZF) after SHL
    uint8_t zf_val = 0x80;
    uint8_t zf_expected = 0x00;
    uint8_t zf_flag = 0;
    __asm__ __volatile__ (
        "shlb $1, %1\n\t"  // Use 'b' suffix for byte (8-bit) operation
        "setz %0"
        : "=r" (zf_flag), "+r" (zf_val)
    );
    
    printf("8-bit SHL ZF test: val=0x%02X, ZF=%d\n", zf_val, zf_flag);
    if (zf_val != zf_expected || zf_flag != 1) {
        printf("Error: Expected val=0x%02X with ZF=1, got val=0x%02X ZF=%d\n", 
               zf_expected, zf_val, zf_flag);
        errors++;
    }

    printf("\nSHL test completed\n");
    printf("==================\n");
    printf("Total tests: 8\n");
    printf("Passed: %d\n", 8 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
