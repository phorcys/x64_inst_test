#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Test DIV with 8-bit divisor
static void test_div8() {
    printf("Testing DIV (8-bit):\n");
    printf("====================\n");
    
    uint16_t dividend;
    uint8_t divisor;
    uint8_t quotient, remainder;
    uint64_t flags;
    
    // Case 1: Normal division
    dividend = 0x1234;
    divisor = 0x56;
    CLEAR_FLAGS;
    asm volatile (
        "divb %2\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "r" (divisor), "a" (dividend)
        : "cc"
    );
    printf("0x%04X / 0x%02X:\n", dividend, divisor);
    printf("  Quotient: 0x%02X, Remainder: 0x%02X\n", quotient, remainder);
    
    // Case 2: Division with remainder
    dividend = 0xABCD;
    divisor = 0xEF;
    CLEAR_FLAGS;
    asm volatile (
        "divb %2\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "r" (divisor), "a" (dividend)
        : "cc"
    );
    printf("0x%04X / 0x%02X:\n", dividend, divisor);
    printf("  Quotient: 0x%02X, Remainder: 0x%02X\n", quotient, remainder);
}

// Test DIV with 16-bit divisor
static void test_div16() {
    printf("\nTesting DIV (16-bit):\n");
    printf("=====================\n");
    
    uint32_t dividend;
    uint16_t divisor;
    uint16_t quotient, remainder;
    uint64_t flags;
    
    // Case 1: Normal division
    dividend = 0x12345678;
    divisor = 0x9ABC;
    CLEAR_FLAGS;
    asm volatile (
        "divw %2\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "r" (divisor), "a" (dividend)
        : "cc"
    );
    printf("0x%08X / 0x%04X:\n", dividend, divisor);
    printf("  Quotient: 0x%04X, Remainder: 0x%04X\n", quotient, remainder);
    
    // Case 2: Division with remainder
    dividend = 0x87654321;
    divisor = 0x1234;
    CLEAR_FLAGS;
    asm volatile (
        "divw %2\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "r" (divisor), "a" (dividend)
        : "cc"
    );
    printf("0x%08X / 0x%04X:\n", dividend, divisor);
    printf("  Quotient: 0x%04X, Remainder: 0x%04X\n", quotient, remainder);
}

// Test DIV with 32-bit divisor
static void test_div32() {
    printf("\nTesting DIV (32-bit):\n");
    printf("=====================\n");
    
    uint64_t dividend;
    uint32_t divisor;
    uint32_t quotient, remainder;
    uint64_t flags;
    
    // Case 1: Normal division
    dividend = 0x123456789ABCDEF0;
    divisor = 0x12345678;
    CLEAR_FLAGS;
    asm volatile (
        "divl %2\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "r" (divisor), "a" (dividend)
        : "cc"
    );
    printf("0x%016lX / 0x%08X:\n", dividend, divisor);
    printf("  Quotient: 0x%08X, Remainder: 0x%08X\n", quotient, remainder);
    
    // Case 2: Division with remainder
    dividend = 0xFEDCBA9876543210;
    divisor = 0xABCDEF01;
    CLEAR_FLAGS;
    asm volatile (
        "divl %2\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "r" (divisor), "a" (dividend)
        : "cc"
    );
    printf("0x%016lX / 0x%08X:\n", dividend, divisor);
    printf("  Quotient: 0x%08X, Remainder: 0x%08X\n", quotient, remainder);
}

// Test DIV with 64-bit divisor
static void test_div64() {
    printf("\nTesting DIV (64-bit):\n");
    printf("=====================\n");
    
    unsigned __int128 dividend;
    uint64_t divisor;
    uint64_t quotient, remainder;
    uint64_t flags;
    
    // Case 1: Normal division
    dividend = (unsigned __int128)0x123456789ABCDEF0 << 64 | 0xFEDCBA9876543210;
    divisor = 0x123456789ABCDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "divq %2\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "r" (divisor), "a" ((uint64_t)dividend), "d" ((uint64_t)(dividend >> 64))
        : "cc"
    );
    printf("0x%016lX%016lX / 0x%016lX:\n", 
           (uint64_t)(dividend >> 64), (uint64_t)dividend, divisor);
    printf("  Quotient: 0x%016lX, Remainder: 0x%016lX\n", quotient, remainder);
    
    // Case 2: Division with remainder
    dividend = (unsigned __int128)0xFEDCBA9876543210 << 64 | 0x123456789ABCDEF0;
    divisor = 0x123456789ABCDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "divq %2\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "r" (divisor), "a" ((uint64_t)dividend), "d" ((uint64_t)(dividend >> 64))
        : "cc"
    );
    printf("0x%016lX%016lX / 0x%016lX:\n", 
           (uint64_t)(dividend >> 64), (uint64_t)dividend, divisor);
    printf("  Quotient: 0x%016lX, Remainder: 0x%016lX\n", quotient, remainder);
}

// Test DIV with memory operands
static void test_div_mem() {
    printf("\nTesting DIV with memory operands:\n");
    printf("================================\n");
    
    // Aligned memory
    uint32_t aligned_divisor __attribute__((aligned(4)));
    uint32_t unaligned_divisor;
    uint32_t *unaligned_ptr = (uint32_t*)((char*)&unaligned_divisor + 1);
    
    uint64_t dividend;
    uint32_t quotient, remainder;
    uint64_t flags;
    
    // Test aligned memory
    dividend = 0x123456789ABCDEF0;
    aligned_divisor = 0x12345678;
    CLEAR_FLAGS;
    asm volatile (
        "divl %1\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "m" (aligned_divisor), "a" (dividend), "d" (dividend >> 32)
        : "cc"
    );
    printf("Aligned memory (0x%016lX / 0x%08X):\n", dividend, aligned_divisor);
    printf("  Quotient: 0x%08X, Remainder: 0x%08X\n", quotient, remainder);
    
    // Test unaligned memory
    dividend = 0xFEDCBA9876543210;
    *unaligned_ptr = 0xABCDEF01;
    CLEAR_FLAGS;
    asm volatile (
        "divl %1\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "m" (*unaligned_ptr), "a" (dividend), "d" (dividend >> 32)
        : "cc"
    );
    printf("Unaligned memory (0x%016lX / 0x%08X):\n", dividend, *unaligned_ptr);
    printf("  Quotient: 0x%08X, Remainder: 0x%08X\n", quotient, remainder);
}

int main() {
    test_div8();
    test_div16();
    test_div32();
    test_div64();
    test_div_mem();
    printf("\nDIV tests completed.\n");
    return 0;
}
