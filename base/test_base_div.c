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
    // Case 1: Normal division
    dividend = 0x00001234;  // DX:AX = 0x0000:0x1234
    divisor = 0x56;
    CLEAR_FLAGS;
    asm volatile (
        "mov %[dividend], %%ax\n\t"  // Load AX
        "xor %%dx, %%dx\n\t"         // Clear DX
        "divw %[divisor]\n\t"
        : "=a" (quotient), "=d" (remainder)
        : [divisor] "r" (divisor), [dividend] "r" ((uint16_t)dividend)
        : "cc"
    );
    printf("0x%08X / 0x%04X:\n", dividend, divisor);
    printf("  Quotient: 0x%04X, Remainder: 0x%04X\n", quotient, remainder);
    
    // Case 2: Division with remainder
    dividend = 0x00001000;  // DX:AX = 0x0000:0x1000
    divisor = 0x3;
    CLEAR_FLAGS;
    asm volatile (
        "mov %[dividend], %%ax\n\t"  // Load AX
        "xor %%dx, %%dx\n\t"         // Clear DX
        "divw %[divisor]\n\t"
        : "=a" (quotient), "=d" (remainder)
        : [divisor] "r" (divisor), [dividend] "r" ((uint16_t)dividend)
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
    
    // Case 1: Normal division
    dividend = 100;
    divisor = 5;
    CLEAR_FLAGS;
    register uint32_t eax asm("eax") = (uint32_t)dividend;
    register uint32_t ecx asm("ecx") = divisor;
    asm volatile (
        "xor %%edx, %%edx\n\t"      // Clear EDX
        "divl %%ecx\n\t"            // Divide EDX:EAX by ECX
        : "=a" (quotient), "=d" (remainder)
        : "a" (eax), "c" (ecx)
        : "cc"
    );
    printf("%lu / %u:\n", dividend, divisor);
    printf("  Quotient: %u, Remainder: %u\n", quotient, remainder);
    
    // Case 2: Division with remainder
    dividend = 100;
    divisor = 7;
    CLEAR_FLAGS;
    eax = (uint32_t)dividend;
    ecx = divisor;
    asm volatile (
        "xor %%edx, %%edx\n\t"      // Clear EDX
        "divl %%ecx\n\t"            // Divide EDX:EAX by ECX
        : "=a" (quotient), "=d" (remainder)
        : "a" (eax), "c" (ecx)
        : "cc"
    );
    printf("%lu / %u:\n", dividend, divisor);
    printf("  Quotient: %u, Remainder: %u\n", quotient, remainder);
    
    // Case 3: Larger values
    dividend = 0x100000000; // 2^32
    divisor = 2;
    CLEAR_FLAGS;
    uint32_t dividend_low = (uint32_t)(dividend & 0xFFFFFFFF);
    uint32_t dividend_high = (uint32_t)(dividend >> 32);
    asm volatile (
        "mov %2, %%eax\n\t"
        "mov %3, %%edx\n\t"
        "divl %4\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "r" (dividend_low), "r" (dividend_high), "r" (divisor)
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
    // Case 1: Normal division
    dividend = 100;
    divisor = 5;
    CLEAR_FLAGS;
    asm volatile (
        "divq %2\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "r" (divisor), "a" ((uint64_t)dividend), "d" ((uint64_t)(dividend >> 64))
        : "cc"
    );
    printf("%llu / %llu:\n", (unsigned long long)dividend, (unsigned long long)divisor);
    printf("  Quotient: %llu, Remainder: %llu\n", (unsigned long long)quotient, (unsigned long long)remainder);
    
    // Case 2: Division with remainder
    dividend = 100;
    divisor = 7;
    CLEAR_FLAGS;
    asm volatile (
        "divq %2\n\t"
        : "=a" (quotient), "=d" (remainder)
        : "r" (divisor), "a" ((uint64_t)dividend), "d" ((uint64_t)(dividend >> 64))
        : "cc"
    );
    printf("%llu / %llu:\n", (unsigned long long)dividend, (unsigned long long)divisor);
    printf("  Quotient: %llu, Remainder: %llu\n", (unsigned long long)quotient, (unsigned long long)remainder);
}

// Test DIV with memory operands
static void test_div_mem() {
    printf("\nTesting DIV with memory operands:\n");
    printf("================================\n");
    
    char buffer[32] __attribute__((aligned(8)));
    
    // 8-bit memory divisor
    uint8_t *div8 = (uint8_t*)&buffer[0];
    *div8 = 10;
    uint16_t dividend8 = 1000;
    CLEAR_FLAGS;
    asm volatile (
        "divb %1\n\t"
        : "+a" (dividend8)
        : "m" (*div8)
        : "cc"
    );
    uint8_t quotient8 = dividend8 & 0xFF;
    uint8_t remainder8 = dividend8 >> 8;
    printf("8-bit memory divisor (1000 / 10):\n");
    printf("  Quotient: 0x%02X, Remainder: 0x%02X\n", quotient8, remainder8);
    
    // 16-bit memory divisor
    uint16_t *div16 = (uint16_t*)&buffer[0];
    *div16 = 0x9ABC;
    uint32_t dividend32 = 0x12345678;
    CLEAR_FLAGS;
    asm volatile (
        "divw %1\n\t"
        : "+a" (dividend32)
        : "m" (*div16)
        : "cc", "dx"
    );
    uint16_t quotient16 = dividend32 & 0xFFFF;
    uint16_t remainder16 = dividend32 >> 16;
    printf("16-bit memory divisor (0x12345678 / 0x9ABC):\n");
    printf("  Quotient: 0x%04X, Remainder: 0x%04X\n", quotient16, remainder16);
    
    // 32-bit memory divisor
    uint32_t *div32 = (uint32_t*)&buffer[0];
    *div32 = 0x12345678;
    uint64_t dividend64 = 0x123456789ABCDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "divl %1\n\t"
        : "+a" (dividend64)
        : "m" (*div32)
        : "cc", "dx"
    );
    uint32_t quotient32 = dividend64 & 0xFFFFFFFF;
    uint32_t remainder32 = dividend64 >> 32;
    printf("32-bit memory divisor (0x123456789ABCDEF0 / 0x12345678):\n");
    printf("  Quotient: 0x%08X, Remainder: 0x%08X\n", quotient32, remainder32);
    
    // 64-bit memory divisor
    uint64_t *div64 = (uint64_t*)&buffer[0];
    *div64 = 5;
    uint64_t rax = 100;
    uint64_t rdx = 0;
    CLEAR_FLAGS;
    asm volatile (
        "divq %2\n\t"
        : "=a" (rax), "=d" (rdx)
        : "m" (*div64), "a" (rax), "d" (rdx)
        : "cc"
    );
    printf("64-bit memory divisor (100 / 5):\n");
    printf("  Quotient: %llu, Remainder: %llu\n", (unsigned long long)rax, (unsigned long long)rdx);
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
