#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Test CMP with register operands
static void test_cmp_reg_reg() {
    printf("Testing CMP reg, reg:\n");
    printf("=====================\n");

    // Test 8-bit registers
    for (int i = 0; i < 3; i++) {
        uint8_t a = 0, b = 0;
        switch(i) {
            case 0: a = 0x12; b = 0x34; break;  // a < b
            case 1: a = 0x55; b = 0x55; break;  // a == b
            case 2: a = 0x78; b = 0x34; break;  // a > b
        }
        
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmp %b2, %b1\n\t"
            "pushfq\n\t"
            "pop %0"
            : "=r" (flags_after)
            : "r" ((uint64_t)a), "r" ((uint64_t)b)
            : "cc", "memory"
        );
        printf("  8-bit: a=0x%02X, b=0x%02X -> flags after: 0x%04lX\n", 
               a, b, flags_after & 0xFFF);
    }

    // Test 16-bit registers
    for (int i = 0; i < 3; i++) {
        uint16_t a = 0, b = 0;
        switch(i) {
            case 0: a = 0x1234; b = 0x5678; break;
            case 1: a = 0xAAAA; b = 0xAAAA; break;
            case 2: a = 0xFFFF; b = 0x0001; break;
        }
        
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmp %w2, %w1\n\t"
            "pushfq\n\t"
            "pop %0"
            : "=r" (flags_after)
            : "r" ((uint64_t)a), "r" ((uint64_t)b)
            : "cc", "memory"
        );
        printf("  16-bit: a=0x%04X, b=0x%04X -> flags after: 0x%04lX\n", 
               a, b, flags_after & 0xFFF);
    }

    // Test 32-bit registers
    for (int i = 0; i < 3; i++) {
        uint32_t a = 0, b = 0;
        switch(i) {
            case 0: a = 0x12345678; b = 0x87654321; break;
            case 1: a = 0xFFFFFFFF; b = 0xFFFFFFFF; break;
            case 2: a = 0x7FFFFFFF; b = 0x80000000; break;
        }
        
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmp %k2, %k1\n\t"
            "pushfq\n\t"
            "pop %0"
            : "=r" (flags_after)
            : "r" ((uint64_t)a), "r" ((uint64_t)b)
            : "cc", "memory"
        );
        printf("  32-bit: a=0x%08X, b=0x%08X -> flags after: 0x%04lX\n", 
               a, b, flags_after & 0xFFF);
    }

    // Test 64-bit registers
    for (int i = 0; i < 3; i++) {
        uint64_t a = 0, b = 0;
        switch(i) {
            case 0: a = 0x1122334455667788; b = 0x99AABBCCDDEEFF00; break;
            case 1: a = 0xFFFFFFFFFFFFFFFF; b = 0xFFFFFFFFFFFFFFFF; break;
            case 2: a = 0x7FFFFFFFFFFFFFFF; b = 0x8000000000000000; break;
        }
        
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmp %2, %1\n\t"
            "pushfq\n\t"
            "pop %0"
            : "=r" (flags_after)
            : "r" (a), "r" (b)
            : "cc", "memory"
        );
        printf("  64-bit: a=0x%016lX, b=0x%016lX -> flags after: 0x%04lX\n", 
               a, b, flags_after & 0xFFF);
    }
}

// Test CMP with immediate operands using registers
static void test_cmp_imm() {
    printf("\nTesting CMP with immediate (using register for immediate):\n");
    printf("=========================================================\n");
    
    // Test 8-bit immediate
    for (int i = 0; i < 3; i++) {
        uint8_t a = 0;
        uint8_t imm_val = 0;
        switch(i) {
            case 0: a = 0x10; imm_val = 0x20; break;
            case 1: a = 0x55; imm_val = 0x55; break;
            case 2: a = 0x80; imm_val = 0x40; break;
        }
        
        uint8_t imm = imm_val;
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmp %b2, %b1\n\t"
            "pushfq\n\t"
            "pop %0"
            : "=r" (flags_after)
            : "r" ((uint64_t)a), "r" ((uint64_t)imm)
            : "cc", "memory"
        );
        printf("  8-bit: a=0x%02X, imm=0x%02X -> flags after: 0x%04lX\n", 
               a, imm_val, flags_after & 0xFFF);
    }

    // Test 16-bit immediate
    for (int i = 0; i < 3; i++) {
        uint16_t a = 0;
        uint16_t imm_val = 0;
        switch(i) {
            case 0: a = 0x1000; imm_val = 0x2000; break;
            case 1: a = 0xAAAA; imm_val = 0xAAAA; break;
            case 2: a = 0x8000; imm_val = 0x4000; break;
        }
        
        uint16_t imm = imm_val;
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmp %w2, %w1\n\t"
            "pushfq\n\t"
            "pop %0"
            : "=r" (flags_after)
            : "r" ((uint64_t)a), "r" ((uint64_t)imm)
            : "cc", "memory"
        );
        printf("  16-bit: a=0x%04X, imm=0x%04X -> flags after: 0x%04lX\n", 
               a, imm_val, flags_after & 0xFFF);
    }

    // Test 32-bit immediate
    for (int i = 0; i < 3; i++) {
        uint32_t a = 0;
        uint32_t imm_val = 0;
        switch(i) {
            case 0: a = 0x10000000; imm_val = 0x20000000; break;
            case 1: a = 0xAAAAAAAA; imm_val = 0xAAAAAAAA; break;
            case 2: a = 0x80000000; imm_val = 0x40000000; break;
        }
        
        uint32_t imm = imm_val;
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmp %k2, %k1\n\t"
            "pushfq\n\t"
            "pop %0"
            : "=r" (flags_after)
            : "r" ((uint64_t)a), "r" ((uint64_t)imm)
            : "cc", "memory"
        );
        printf("  32-bit: a=0x%08X, imm=0x%08X -> flags after: 0x%04lX\n", 
               a, imm_val, flags_after & 0xFFF);
    }

    // Test 64-bit immediate
    for (int i = 0; i < 3; i++) {
        uint64_t a = 0;
        uint64_t imm_val = 0;
        switch(i) {
            case 0: a = 0x1000000000000000; imm_val = 0x2000000000000000; break;
            case 1: a = 0xAAAAAAAAAAAAAAAA; imm_val = 0xAAAAAAAAAAAAAAAA; break;
            case 2: a = 0x8000000000000000; imm_val = 0x4000000000000000; break;
        }
        
        uint64_t imm = imm_val;
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmp %2, %1\n\t"
            "pushfq\n\t"
            "pop %0"
            : "=r" (flags_after)
            : "r" (a), "r" (imm)
            : "cc", "memory"
        );
        printf("  64-bit: a=0x%016lX, imm=0x%016lX -> flags after: 0x%04lX\n", 
               a, imm_val, flags_after & 0xFFF);
    }
}

// Test CMP with memory operands
static void test_cmp_mem() {
    printf("\nTesting CMP with memory operands:\n");
    printf("================================\n");
    
    // Test 8-bit memory
    for (int i = 0; i < 3; i++) {
        uint8_t mem_val = 0;
        uint8_t reg_val = 0;
        switch(i) {
            case 0: mem_val = 0x10; reg_val = 0x20; break;
            case 1: mem_val = 0x55; reg_val = 0x55; break;
            case 2: mem_val = 0x80; reg_val = 0x40; break;
        }
        
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmpb %1, %b0\n\t"
            "pushfq\n\t"
            "pop %2"
            : "=r" (flags_after)
            : "m" (mem_val), "r" ((uint64_t)reg_val)
            : "cc", "memory"
        );
        printf("  8-bit: mem=0x%02X, reg=0x%02X -> flags after: 0x%04lX\n", 
               mem_val, reg_val, flags_after & 0xFFF);
    }

    // Test 16-bit memory
    for (int i = 0; i < 3; i++) {
        uint16_t mem_val = 0;
        uint16_t reg_val = 0;
        switch(i) {
            case 0: mem_val = 0x1000; reg_val = 0x2000; break;
            case 1: mem_val = 0xAAAA; reg_val = 0xAAAA; break;
            case 2: mem_val = 0x8000; reg_val = 0x4000; break;
        }
        
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmpw %1, %w0\n\t"
            "pushfq\n\t"
            "pop %2"
            : "=r" (flags_after)
            : "m" (mem_val), "r" ((uint64_t)reg_val)
            : "cc", "memory"
        );
        printf("  16-bit: mem=0x%04X, reg=0x%04X -> flags after: 0x%04lX\n", 
               mem_val, reg_val, flags_after & 0xFFF);
    }

    // Test 32-bit memory
    for (int i = 0; i < 3; i++) {
        uint32_t mem_val = 0;
        uint32_t reg_val = 0;
        switch(i) {
            case 0: mem_val = 0x10000000; reg_val = 0x20000000; break;
            case 1: mem_val = 0xAAAAAAAA; reg_val = 0xAAAAAAAA; break;
            case 2: mem_val = 0x80000000; reg_val = 0x40000000; break;
        }
        
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmpl %1, %k0\n\t"
            "pushfq\n\t"
            "pop %2"
            : "=r" (flags_after)
            : "m" (mem_val), "r" ((uint64_t)reg_val)
            : "cc", "memory"
        );
        printf("  32-bit: mem=0x%08X, reg=0x%08X -> flags after: 0x%04lX\n", 
               mem_val, reg_val, flags_after & 0xFFF);
    }

    // Test 64-bit memory
    for (int i = 0; i < 3; i++) {
        uint64_t mem_val = 0;
        uint64_t reg_val = 0;
        switch(i) {
            case 0: mem_val = 0x1000000000000000; reg_val = 0x2000000000000000; break;
            case 1: mem_val = 0xAAAAAAAAAAAAAAAA; reg_val = 0xAAAAAAAAAAAAAAAA; break;
            case 2: mem_val = 0x8000000000000000; reg_val = 0x4000000000000000; break;
        }
        
        uint64_t flags_after;
        asm volatile (
            "push $0\n\t"
            "popfq\n\t"
            "push $0\n\t"
            "popfq\n\t"
            "cmp %1, %0\n\t"
            "pushfq\n\t"
            "pop %2"
            : "=r" (flags_after)
            : "r" (reg_val), "m" (mem_val)
            : "cc", "memory"
        );
        printf("  64-bit: mem=0x%016lX, reg=0x%016lX -> flags after: 0x%04lX\n", 
               mem_val, reg_val, flags_after & 0xFFF);
    }
}

int main() {
    test_cmp_reg_reg();
    test_cmp_imm();
    test_cmp_mem();
    printf("\nCMP tests completed.\n");
    return 0;
}
