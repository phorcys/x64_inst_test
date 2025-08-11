#include <stdio.h>
#include <stdint.h>
#include "base.h"

static void print_flags(uint64_t flags) {
    printf("CF=%d ", (flags & 1) ? 1 : 0);
    printf("PF=%d ", (flags & 4) ? 1 : 0);
    printf("AF=%d ", (flags & 16) ? 1 : 0);
    printf("ZF=%d ", (flags & 64) ? 1 : 0);
    printf("SF=%d ", (flags & 128) ? 1 : 0);
    printf("OF=%d\n", (flags & 2048) ? 1 : 0);
}

static void test_jb_basic() {
    printf("Testing basic JB functionality:\n");
    printf("------------------------------\n");
    
    uint64_t flags;
    volatile int result = 0;
    
    // Case 1: CF set - should jump
    asm volatile (
        "push $0\n"
        "popfq\n"               // Clear all flags
        "stc\n"                 // Set carry flag
        "jb 1f\n"               // Jump if below (CF=1)
        "movl $0, %0\n"         // Should not execute
        "jmp 2f\n"
        "1:\n"
        "movl $1, %0\n"         // Jump target
        "2:\n"
        "pushfq\n"
        "popq %1\n"
        : "=m" (result), "=r" (flags)
        :
        : "cc", "memory"
    );
    
    printf("Case 1 (CF=1): Jump %s\n", result ? "taken" : "not taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
    
    // Case 2: CF clear - should not jump
    result = 0;
    asm volatile (
        "push $0\n"
        "popfq\n"               // Clear all flags
        "clc\n"                 // Clear carry flag
        "jb 1f\n"               // Should not jump
        "movl $1, %0\n"         // Should execute
        "jmp 2f\n"
        "1:\n"
        "movl $0, %0\n"         // Should not execute
        "2:\n"
        "pushfq\n"
        "popq %1\n"
        : "=m" (result), "=r" (flags)
        :
        : "cc", "memory"
    );
    
    printf("Case 2 (CF=0): Jump %s\n", result ? "not taken" : "taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
}

static void test_jb_with_arithmetic() {
    printf("Testing JB with arithmetic operations:\n");
    printf("-------------------------------------\n");
    
    uint64_t flags;
    volatile int result = 0;
    uint64_t a, b;
    
    // Case 1: Subtraction that sets CF (a < b)
    a = 5;
    b = 10;
    asm volatile (
        "movq %2, %%rax\n"
        "subq %3, %%rax\n"      // 5 - 10 = -5 (sets CF)
        "jb 1f\n"               // Should jump
        "movl $0, %0\n"         // Should not execute
        "jmp 2f\n"
        "1:\n"
        "movl $1, %0\n"         // Jump target
        "2:\n"
        "pushfq\n"
        "popq %1\n"
        : "=m" (result), "=r" (flags)
        : "r" (a), "r" (b)
        : "rax", "cc"
    );
    
    printf("Case 1 (5 - 10): Jump %s\n", result ? "taken" : "not taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
    
    // Case 2: Subtraction that clears CF (a > b)
    result = 0;
    a = 10;
    b = 5;
    asm volatile (
        "movq %2, %%rax\n"
        "subq %3, %%rax\n"      // 10 - 5 = 5 (clears CF)
        "jb 1f\n"               // Should not jump
        "movl $1, %0\n"         // Should execute
        "jmp 2f\n"
        "1:\n"
        "movl $0, %0\n"         // Should not execute
        "2:\n"
        "pushfq\n"
        "popq %1\n"
        : "=m" (result), "=r" (flags)
        : "r" (a), "r" (b)
        : "rax", "cc"
    );
    
    printf("Case 2 (10 - 5): Jump %s\n", result ? "not taken" : "taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
}

static void test_jb_with_memory_operand() {
    printf("Testing JB with memory operand:\n");
    printf("------------------------------\n");
    
    uint64_t flags;
    volatile int result = 0;
    uint64_t mem_val = 0x12345678;
    
    // Compare memory value with immediate
    asm volatile (
        "cmpq $0x12345679, %2\n"  // Compare mem_val with larger value
        "jb 1f\n"                 // Should jump (mem_val < immediate)
        "movl $0, %0\n"           // Should not execute
        "jmp 2f\n"
        "1:\n"
        "movl $1, %0\n"           // Jump target
        "2:\n"
        "pushfq\n"
        "popq %1\n"
        : "=m" (result), "=r" (flags)
        : "m" (mem_val)
        : "cc"
    );
    
    printf("Case 1 (mem < imm): Jump %s\n", result ? "taken" : "not taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
}

int main() {
    printf("Starting JB instruction tests\n");
    printf("============================\n\n");
    
    test_jb_basic();
    test_jb_with_arithmetic();
    test_jb_with_memory_operand();
    
    printf("\nAll JB tests completed.\n");
    return 0;
}
