#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

// Test CMPXCHG8B with memory operands
static void test_cmpxchg8b() {
    printf("Testing CMPXCHG8B:\n");
    printf("==================\n");
    
    // Aligned memory (8-byte aligned)
    uint64_t aligned_mem __attribute__((aligned(8)));
    uint64_t unaligned_mem;
    uint64_t *unaligned_ptr = (uint64_t*)((char*)&unaligned_mem + 1);
    
    uint64_t expected, new_val;
    uint32_t eflags;
    
    // Test aligned memory (success)
    aligned_mem = 0x123456789ABCDEF0;
    expected = 0x123456789ABCDEF0;
    new_val = 0xFEDCBA9876543210;
    CLEAR_FLAGS;
    asm volatile (
        "movl $0x9ABCDEF0, %%eax\n\t"
        "movl $0x12345678, %%edx\n\t"
        "movl $0x76543210, %%ebx\n\t"
        "movl $0xFEDCBA98, %%ecx\n\t"
        "cmpxchg8b %0\n\t"
        : "+m" (aligned_mem)
        : 
        : "eax", "ebx", "ecx", "edx", "cc"
    );
    eflags = get_eflags();
    printf("Aligned memory (success):\n");
    printf("  Expected: 0x%016lX, Actual: 0x%016lX\n", expected, aligned_mem);
    printf("  ZF: %d\n", (eflags & X_ZF) ? 1 : 0);
    printf("  Flags: 0x%03X\n", eflags & 0x8D5);
    
    // Test aligned memory (failure)
    aligned_mem = 0x1122334455667788;
    expected = 0x123456789ABCDEF0;
    new_val = 0xFEDCBA9876543210;
    CLEAR_FLAGS;
    asm volatile (
        "movl $0x9ABCDEF0, %%eax\n\t"
        "movl $0x12345678, %%edx\n\t"
        "movl $0x76543210, %%ebx\n\t"
        "movl $0xFEDCBA98, %%ecx\n\t"
        "cmpxchg8b %0\n\t"
        : "+m" (aligned_mem)
        : 
        : "eax", "ebx", "ecx", "edx", "cc"
    );
    eflags = get_eflags();
    printf("Aligned memory (failure):\n");
    printf("  Expected: 0x%016lX, Actual: 0x%016lX\n", expected, aligned_mem);
    printf("  ZF: %d\n", (eflags & X_ZF) ? 1 : 0);
    printf("  Flags: 0x%03X\n", eflags & 0x8D5);
    
    // Test unaligned memory (success)
    *unaligned_ptr = 0x123456789ABCDEF0;
    expected = 0x123456789ABCDEF0;
    new_val = 0xFEDCBA9876543210;
    CLEAR_FLAGS;
    asm volatile (
        "movl $0x9ABCDEF0, %%eax\n\t"
        "movl $0x12345678, %%edx\n\t"
        "movl $0x76543210, %%ebx\n\t"
        "movl $0xFEDCBA98, %%ecx\n\t"
        "cmpxchg8b %0\n\t"
        : "+m" (*unaligned_ptr)
        : 
        : "eax", "ebx", "ecx", "edx", "cc"
    );
    eflags = get_eflags();
    printf("Unaligned memory (success):\n");
    printf("  Expected: 0x%016lX, Actual: 0x%016lX\n", expected, *unaligned_ptr);
    printf("  ZF: %d\n", (eflags & X_ZF) ? 1 : 0);
    printf("  Flags: 0x%03X\n", eflags & 0x8D5);
    
    // Test unaligned memory (failure)
    *unaligned_ptr = 0x1122334455667788;
    expected = 0x123456789ABCDEF0;
    new_val = 0xFEDCBA9876543210;
    CLEAR_FLAGS;
    asm volatile (
        "movl $0x9ABCDEF0, %%eax\n\t"
        "movl $0x12345678, %%edx\n\t"
        "movl $0x76543210, %%ebx\n\t"
        "movl $0xFEDCBA98, %%ecx\n\t"
        "cmpxchg8b %0\n\t"
        : "+m" (*unaligned_ptr)
        : 
        : "eax", "ebx", "ecx", "edx", "cc"
    );
    eflags = get_eflags();
    printf("Unaligned memory (failure):\n");
    printf("  Expected: 0x%016lX, Actual: 0x%016lX\n", expected, *unaligned_ptr);
    printf("  ZF: %d\n", (eflags & X_ZF) ? 1 : 0);
    printf("  Flags: 0x%03X\n", eflags & 0x8D5);
}

// Test LOCK prefix with CMPXCHG8B
static void test_lock_cmpxchg8b() {
    printf("\nTesting LOCK CMPXCHG8B:\n");
    printf("=======================\n");
    
    uint64_t shared_mem = 0;
    uint64_t expected, new_val;
    uint32_t eflags;
    
    // Test with LOCK prefix (success)
    expected = 0;
    new_val = 0x123456789ABCDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "movl $0, %%eax\n\t"
        "movl $0, %%edx\n\t"
        "movl $0x9ABCDEF0, %%ebx\n\t"
        "movl $0x12345678, %%ecx\n\t"
        "lock cmpxchg8b %0\n\t"
        : "+m" (shared_mem)
        : 
        : "eax", "ebx", "ecx", "edx", "cc"
    );
    eflags = get_eflags();
    printf("LOCK CMPXCHG8B (success):\n");
    printf("  Expected: 0x%016lX, Actual: 0x%016lX\n", 0UL, shared_mem);
    printf("  ZF: %d\n", (eflags & X_ZF) ? 1 : 0);
    printf("  Flags: 0x%03X\n", eflags & 0x8D5);
    
    // Test with LOCK prefix (failure)
    shared_mem = 0x1122334455667788;
    expected = 0;
    new_val = 0x123456789ABCDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "movl $0, %%eax\n\t"
        "movl $0, %%edx\n\t"
        "movl $0x9ABCDEF0, %%ebx\n\t"
        "movl $0x12345678, %%ecx\n\t"
        "lock cmpxchg8b %0\n\t"
        : "+m" (shared_mem)
        : 
        : "eax", "ebx", "ecx", "edx", "cc"
    );
    eflags = get_eflags();
    printf("LOCK CMPXCHG8B (failure):\n");
    printf("  Expected: 0x%016lX, Actual: 0x%016lX\n", 0UL, shared_mem);
    printf("  ZF: %d\n", (eflags & X_ZF) ? 1 : 0);
    printf("  Flags: 0x%03X\n", eflags & 0x8D5);
}

int main() {
    test_cmpxchg8b();
    test_lock_cmpxchg8b();
    printf("\nCMPXCHG8B tests completed.\n");
    return 0;
}
