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

static void test_jbe_basic() {
    printf("Testing basic JBE functionality:\n");
    printf("------------------------------\n");
    
    uint64_t flags;
    volatile int result = 0;
    
    // Case 1: CF set - should jump
    asm volatile (
        "push $0\n"
        "popfq\n"               // Clear all flags
        "stc\n"                 // Set carry flag
        "jbe 1f\n"              // Jump if below or equal (CF=1 or ZF=1)
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
    
    // Case 2: ZF set - should jump
    result = 0;
    asm volatile (
        "push $0\n"
        "popfq\n"               // Clear all flags
        "mov $0, %%rax\n"
        "test %%rax, %%rax\n"   // Set zero flag
        "jbe 1f\n"              // Jump if below or equal (CF=1 or ZF=1)
        "movl $0, %0\n"         // Should not execute
        "jmp 2f\n"
        "1:\n"
        "movl $1, %0\n"         // Jump target
        "2:\n"
        "pushfq\n"
        "popq %1\n"
        : "=m" (result), "=r" (flags)
        :
        : "rax", "cc", "memory"
    );
    
    printf("Case 2 (ZF=1): Jump %s\n", result ? "taken" : "not taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
    
    // Case 3: Both flags clear - should not jump
    result = 0;
    asm volatile (
        "push $0\n"
        "popfq\n"               // Clear all flags
        "jbe 1f\n"              // Should not jump
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
    
    printf("Case 3 (CF=0, ZF=0): Jump %s\n", result ? "not taken" : "taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
}

static void test_jbe_with_arithmetic() {
    printf("Testing JBE with arithmetic operations:\n");
    printf("-------------------------------------\n");
    
    uint64_t flags;
    volatile int result = 0;
    uint64_t a, b;
    
    // Case 1: a < b (sets CF)
    a = 5;
    b = 10;
    asm volatile (
        "movq %2, %%rax\n"
        "cmpq %3, %%rax\n"      // Compare a and b (5 < 10)
        "jbe 1f\n"              // Should jump (CF=1)
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
    
    printf("Case 1 (5 < 10): Jump %s\n", result ? "taken" : "not taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
    
    // Case 2: a == b (sets ZF)
    result = 0;
    a = 10;
    b = 10;
    asm volatile (
        "movq %2, %%rax\n"
        "cmpq %3, %%rax\n"      // Compare a and b (10 == 10)
        "jbe 1f\n"              // Should jump (ZF=1)
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
    
    printf("Case 2 (10 == 10): Jump %s\n", result ? "taken" : "not taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
    
    // Case 3: a > b (clears both CF and ZF)
    result = 0;
    a = 15;
    b = 10;
    asm volatile (
        "movq %2, %%rax\n"
        "cmpq %3, %%rax\n"      // Compare a and b (15 > 10)
        "jbe 1f\n"              // Should not jump
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
    
    printf("Case 3 (15 > 10): Jump %s\n", result ? "not taken" : "taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
}

static void test_jbe_with_memory_operand() {
    printf("Testing JBE with memory operand:\n");
    printf("------------------------------\n");
    
    uint64_t flags;
    volatile int result = 0;
    uint64_t mem_val = 0x12345678;
    
    // Compare memory value with immediate (mem_val == immediate - 1)
    asm volatile (
        "cmpq $0x12345679, %2\n"  // Compare mem_val with larger value
        "jbe 1f\n"                 // Should jump (mem_val < immediate)
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
    
    // Compare memory value with immediate (mem_val == immediate)
    result = 0;
    asm volatile (
        "cmpq $0x12345678, %2\n"  // Compare mem_val with same value
        "jbe 1f\n"                 // Should jump (mem_val == immediate)
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
    
    printf("Case 2 (mem == imm): Jump %s\n", result ? "taken" : "not taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
}

int main() {
    printf("Starting JBE instruction tests\n");
    printf("============================\n\n");
    
    test_jbe_basic();
    test_jbe_with_arithmetic();
    test_jbe_with_memory_operand();
    
    printf("\nAll JBE tests completed.\n");
    return 0;
}
