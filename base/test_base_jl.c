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

static void test_jl_basic() {
    printf("Testing basic JL functionality:\n");
    printf("------------------------------\n");
    
    uint64_t flags;
    volatile int result = 0;
    
    // Case 1: SF != OF - should jump
    // Set SF=1, OF=0 (SF != OF)
    asm volatile (
        "push $0\n"
        "popfq\n"               // Clear all flags
        "mov $0x80000000, %%rax\n"
        "add $0x1, %%rax\n"    // Sets SF=1 (negative result), OF=0 (no overflow)
        "jl 1f\n"               // Jump if less (SF != OF)
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
    
    printf("Case 1 (SF=1, OF=0): Jump %s\n", result ? "taken" : "not taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
    
    // Case 2: SF == OF - should not jump
    // Set SF=0, OF=0 (SF == OF)
    result = 0;
    asm volatile (
        "push $0\n"
        "popfq\n"               // Clear all flags
        "jl 1f\n"               // Should not jump
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
    
    printf("Case 2 (SF=0, OF=0): Jump %s\n", result ? "not taken" : "taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
}

static void test_jl_with_arithmetic() {
    printf("Testing JL with arithmetic operations:\n");
    printf("-------------------------------------\n");
    
    uint64_t flags;
    volatile int result = 0;
    int32_t a, b;
    
    // Case 1: a < b (signed) without overflow
    a = -5;
    b = 10;
    asm volatile (
        "movl %2, %%eax\n"
        "cmpl %3, %%eax\n"      // Compare a and b (-5 < 10)
        "jl 1f\n"               // Should jump
        "movl $0, %0\n"         // Should not execute
        "jmp 2f\n"
        "1:\n"
        "movl $1, %0\n"         // Jump target
        "2:\n"
        "pushfq\n"
        "popq %1\n"
        : "=m" (result), "=r" (flags)
        : "r" (a), "r" (b)
        : "eax", "cc"
    );
    
    printf("Case 1 (-5 < 10): Jump %s\n", result ? "taken" : "not taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
    
    // Case 2: a > b (signed) without overflow
    result = 0;
    a = 15;
    b = 10;
    asm volatile (
        "movl %2, %%eax\n"
        "cmpl %3, %%eax\n"      // Compare a and b (15 > 10)
        "jl 1f\n"               // Should not jump
        "movl $1, %0\n"         // Should execute
        "jmp 2f\n"
        "1:\n"
        "movl $0, %0\n"         // Should not execute
        "2:\n"
        "pushfq\n"
        "popq %1\n"
        : "=m" (result), "=r" (flags)
        : "r" (a), "r" (b)
        : "eax", "cc"
    );
    
    printf("Case 2 (15 > 10): Jump %s\n", result ? "not taken" : "taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
    
    // Case 3: Overflow case (SF != OF due to overflow)
    result = 0;
    a = 0x7FFFFFFF; // INT_MAX
    b = 1;
    asm volatile (
        "movl %2, %%eax\n"
        "addl %3, %%eax\n"      // INT_MAX + 1 -> overflow (sets OF=1, SF=1)
        "jl 1f\n"               // Should jump (SF != OF: 1 != 1? -> no, 1==1 -> no jump)
        "movl $1, %0\n"         // Should execute
        "jmp 2f\n"
        "1:\n"
        "movl $0, %0\n"         // Should not execute
        "2:\n"
        "pushfq\n"
        "popq %1\n"
        : "=m" (result), "=r" (flags)
        : "r" (a), "r" (b)
        : "eax", "cc", "memory"
    );
    
    // Actually, after INT_MAX + 1, we get a negative number (SF=1) and overflow (OF=1)
    // Since SF == OF (both 1), JL should not jump
    printf("Case 3 (INT_MAX + 1): Jump %s\n", result ? "not taken" : "taken");
    printf("Flags: ");
    print_flags(flags);
    printf("\n");
}

static void test_jl_with_memory_operand() {
    printf("Testing JL with memory operand:\n");
    printf("------------------------------\n");
    
    uint64_t flags;
    volatile int result = 0;
    int32_t mem_val = -10;
    
    // Compare memory value with immediate
    asm volatile (
        "cmpl $0, %2\n"         // Compare mem_val (-10) with 0
        "jl 1f\n"               // Should jump (-10 < 0)
        "movl $0, %0\n"         // Should not execute
        "jmp 2f\n"
        "1:\n"
        "movl $1, %0\n"         // Jump target
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
    printf("Starting JL instruction tests\n");
    printf("============================\n\n");
    
    test_jl_basic();
    test_jl_with_arithmetic();
    test_jl_with_memory_operand();
    
    printf("\nAll JL tests completed.\n");
    return 0;
}
