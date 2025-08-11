#include <stdio.h>
#include <stdint.h>
#include "base.h"

static void test_jecxz_basic() {
    printf("Testing basic JECXZ functionality:\n");
    printf("--------------------------------\n");
    
    volatile int result = 0;
    
    // Case 1: RCX = 0 - should jump
    asm volatile (
        "mov $0, %%rcx\n"       // Set RCX to 0
        "jecxz 1f\n"            // Jump if RCX is zero
        "movl $0, %0\n"         // Should not execute
        "jmp 2f\n"
        "1:\n"
        "movl $1, %0\n"         // Jump target
        "2:\n"
        : "=m" (result)
        :
        : "rcx", "cc", "memory"
    );
    
    printf("Case 1 (RCX=0): Jump %s\n", result ? "taken" : "not taken");
    printf("\n");
    
    // Case 2: RCX = 1 - should not jump
    result = 0;
    asm volatile (
        "mov $1, %%rcx\n"       // Set RCX to 1
        "jecxz 1f\n"            // Should not jump
        "movl $1, %0\n"         // Should execute
        "jmp 2f\n"
        "1:\n"
        "movl $0, %0\n"         // Should not execute
        "2:\n"
        : "=m" (result)
        :
        : "rcx", "cc", "memory"
    );
    
    printf("Case 2 (RCX=1): Jump %s\n", result ? "not taken" : "taken");
    printf("\n");
    
    // Case 3: RCX = 0x100000000 - should not jump (only checks lower 32 bits)
    result = 0;
    asm volatile (
        "mov $0x100000000, %%rcx\n" // Set RCX to 0x100000000 (ECX=0)
        "jecxz 1f\n"            // Should jump (ECX=0)
        "movl $0, %0\n"         // Should not execute
        "jmp 2f\n"
        "1:\n"
        "movl $1, %0\n"         // Jump target
        "2:\n"
        : "=m" (result)
        :
        : "rcx", "cc", "memory"
    );
    
    printf("Case 3 (RCX=0x100000000): Jump %s\n", result ? "taken" : "not taken");
    printf("\n");
}

static void test_jecxz_with_memory() {
    printf("Testing JECXZ with memory operations:\n");
    printf("-----------------------------------\n");
    
    volatile int result = 0;
    uint64_t mem_val = 0;
    
    // Case 1: Load 0 into RCX from memory
    asm volatile (
        "mov %1, %%rcx\n"       // Load mem_val into RCX
        "jecxz 1f\n"            // Jump if RCX is zero
        "movl $0, %0\n"         // Should not execute
        "jmp 2f\n"
        "1:\n"
        "movl $1, %0\n"         // Jump target
        "2:\n"
        : "=m" (result)
        : "m" (mem_val)
        : "rcx", "cc", "memory"
    );
    
    printf("Case 1 (mem=0): Jump %s\n", result ? "taken" : "not taken");
    printf("\n");
    
    // Case 2: Load non-zero into RCX from memory
    result = 0;
    mem_val = 42;
    asm volatile (
        "mov %1, %%rcx\n"       // Load mem_val into RCX
        "jecxz 1f\n"            // Should not jump
        "movl $1, %0\n"         // Should execute
        "jmp 2f\n"
        "1:\n"
        "movl $0, %0\n"         // Should not execute
        "2:\n"
        : "=m" (result)
        : "m" (mem_val)
        : "rcx", "cc", "memory"
    );
    
    printf("Case 2 (mem=42): Jump %s\n", result ? "not taken" : "taken");
    printf("\n");
}

int main() {
    printf("Starting JECXZ instruction tests\n");
    printf("===============================\n\n");
    
    test_jecxz_basic();
    test_jecxz_with_memory();
    
    printf("\nAll JECXZ tests completed.\n");
    return 0;
}
