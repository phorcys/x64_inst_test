#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

// Test CMPXCHG with register operands
static void test_cmpxchg_reg() {
    printf("Testing CMPXCHG with register operands:\n");
    printf("======================================\n");
    
    // Test 8-bit
    uint8_t al8, dest8, src8;
    uint64_t flags;
    
    // Case 1: AL == dest (ZF=1)
    al8 = 0x12;
    dest8 = 0x12;
    src8 = 0x34;
    CLEAR_FLAGS;
    asm volatile (
        "movb %2, %%al\n\t"
        "cmpxchgb %3, %0\n\t"
        : "+r" (dest8), "+a" (al8)
        : "r" (al8), "r" (src8)
        : "cc"
    );
    flags = get_eflags();
    printf("8-bit (success):\n");
    printf("  AL before: 0x%02X, after: 0x%02X\n", 0x12, al8);
    printf("  Dest before: 0x%02X, after: 0x%02X\n", 0x12, dest8);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: AL != dest (ZF=0)
    al8 = 0x12;
    dest8 = 0x56;
    src8 = 0x78;
    CLEAR_FLAGS;
    asm volatile (
        "movb %2, %%al\n\t"
        "cmpxchgb %3, %0\n\t"
        : "+r" (dest8), "+a" (al8)
        : "r" (al8), "r" (src8)
        : "cc"
    );
    flags = get_eflags();
    printf("8-bit (failure):\n");
    printf("  AL before: 0x%02X, after: 0x%02X\n", 0x12, al8);
    printf("  Dest before: 0x%02X, after: 0x%02X\n", 0x56, dest8);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Test 16-bit
    uint16_t ax16, dest16, src16;
    
    // Case 1: AX == dest (ZF=1)
    ax16 = 0x1234;
    dest16 = 0x1234;
    src16 = 0x5678;
    CLEAR_FLAGS;
    asm volatile (
        "movw %2, %%ax\n\t"
        "cmpxchgw %3, %0\n\t"
        : "+r" (dest16), "+a" (ax16)
        : "r" (ax16), "r" (src16)
        : "cc"
    );
    flags = get_eflags();
    printf("16-bit (success):\n");
    printf("  AX before: 0x%04X, after: 0x%04X\n", 0x1234, ax16);
    printf("  Dest before: 0x%04X, after: 0x%04X\n", 0x1234, dest16);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: AX != dest (ZF=0)
    ax16 = 0x1234;
    dest16 = 0x9ABC;
    src16 = 0xDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "movw %2, %%ax\n\t"
        "cmpxchgw %3, %0\n\t"
        : "+r" (dest16), "+a" (ax16)
        : "r" (ax16), "r" (src16)
        : "cc"
    );
    flags = get_eflags();
    printf("16-bit (failure):\n");
    printf("  AX before: 0x%04X, after: 0x%04X\n", 0x1234, ax16);
    printf("  Dest before: 0x%04X, after: 0x%04X\n", 0x9ABC, dest16);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Test 32-bit
    uint32_t eax32, dest32, src32;
    
    // Case 1: EAX == dest (ZF=1)
    eax32 = 0x12345678;
    dest32 = 0x12345678;
    src32 = 0x9ABCDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "movl %2, %%eax\n\t"
        "cmpxchgl %3, %0\n\t"
        : "+r" (dest32), "+a" (eax32)
        : "r" (eax32), "r" (src32)
        : "cc"
    );
    flags = get_eflags();
    printf("32-bit (success):\n");
    printf("  EAX before: 0x%08X, after: 0x%08X\n", 0x12345678, eax32);
    printf("  Dest before: 0x%08X, after: 0x%08X\n", 0x12345678, dest32);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: EAX != dest (ZF=0)
    eax32 = 0x12345678;
    dest32 = 0x11223344;
    src32 = 0x55667788;
    CLEAR_FLAGS;
    asm volatile (
        "movl %2, %%eax\n\t"
        "cmpxchgl %3, %0\n\t"
        : "+r" (dest32), "+a" (eax32)
        : "r" (eax32), "r" (src32)
        : "cc"
    );
    flags = get_eflags();
    printf("32-bit (failure):\n");
    printf("  EAX before: 0x%08X, after: 0x%08X\n", 0x12345678, eax32);
    printf("  Dest before: 0x%08X, after: 0x%08X\n", 0x11223344, dest32);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Test 64-bit
    uint64_t rax64, dest64, src64;
    
    // Case 1: RAX == dest (ZF=1)
    rax64 = 0x123456789ABCDEF0;
    dest64 = 0x123456789ABCDEF0;
    src64 = 0xFEDCBA9876543210;
    CLEAR_FLAGS;
    asm volatile (
        "movq %2, %%rax\n\t"
        "cmpxchgq %3, %0\n\t"
        : "+r" (dest64), "+a" (rax64)
        : "r" (rax64), "r" (src64)
        : "cc"
    );
    flags = get_eflags();
    printf("64-bit (success):\n");
    printf("  RAX before: 0x%016lX, after: 0x%016lX\n", 0x123456789ABCDEF0UL, rax64);
    printf("  Dest before: 0x%016lX, after: 0x%016lX\n", 0x123456789ABCDEF0UL, dest64);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: RAX != dest (ZF=0)
    rax64 = 0x123456789ABCDEF0;
    dest64 = 0x1122334455667788;
    src64 = 0x99AABBCCDDEEFF00;
    CLEAR_FLAGS;
    asm volatile (
        "movq %2, %%rax\n\t"
        "cmpxchgq %3, %0\n\t"
        : "+r" (dest64), "+a" (rax64)
        : "r" (rax64), "r" (src64)
        : "cc"
    );
    flags = get_eflags();
    printf("64-bit (failure):\n");
    printf("  RAX before: 0x%016lX, after: 0x%016lX\n", 0x123456789ABCDEF0UL, rax64);
    printf("  Dest before: 0x%016lX, after: 0x%016lX\n", 0x1122334455667788UL, dest64);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
}

// Test CMPXCHG with memory operands
static void test_cmpxchg_mem() {
    printf("\nTesting CMPXCHG with memory operands:\n");
    printf("====================================\n");
    
    // Aligned memory (16-byte aligned)
    uint32_t aligned_mem __attribute__((aligned(16)));
    uint32_t unaligned_mem;
    uint32_t *unaligned_ptr = (uint32_t*)((char*)&unaligned_mem + 1);
    
    uint32_t eax_val, src_val;
    uint64_t flags;
    
    // Test aligned memory (success)
    aligned_mem = 0x12345678;
    eax_val = 0x12345678;
    src_val = 0x9ABCDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "movl %2, %%eax\n\t"
        "cmpxchgl %3, %0\n\t"
        : "+m" (aligned_mem), "+a" (eax_val)
        : "r" (eax_val), "r" (src_val)
        : "cc"
    );
    flags = get_eflags();
    printf("Aligned memory (success):\n");
    printf("  Memory before: 0x%08X, after: 0x%08X\n", 0x12345678, aligned_mem);
    printf("  EAX before: 0x%08X, after: 0x%08X\n", 0x12345678, eax_val);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Test aligned memory (failure)
    aligned_mem = 0x11223344;
    eax_val = 0x12345678;
    src_val = 0x55667788;
    CLEAR_FLAGS;
    asm volatile (
        "movl %2, %%eax\n\t"
        "cmpxchgl %3, %0\n\t"
        : "+m" (aligned_mem), "+a" (eax_val)
        : "r" (eax_val), "r" (src_val)
        : "cc"
    );
    flags = get_eflags();
    printf("Aligned memory (failure):\n");
    printf("  Memory before: 0x%08X, after: 0x%08X\n", 0x11223344, aligned_mem);
    printf("  EAX before: 0x%08X, after: 0x%08X\n", 0x12345678, eax_val);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Test unaligned memory (success)
    *unaligned_ptr = 0x12345678;
    eax_val = 0x12345678;
    src_val = 0x9ABCDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "movl %2, %%eax\n\t"
        "cmpxchgl %3, %0\n\t"
        : "+m" (*unaligned_ptr), "+a" (eax_val)
        : "r" (eax_val), "r" (src_val)
        : "cc"
    );
    flags = get_eflags();
    printf("Unaligned memory (success):\n");
    printf("  Memory before: 0x%08X, after: 0x%08X\n", 0x12345678, *unaligned_ptr);
    printf("  EAX before: 0x%08X, after: 0x%08X\n", 0x12345678, eax_val);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Test unaligned memory (failure)
    *unaligned_ptr = 0x11223344;
    eax_val = 0x12345678;
    src_val = 0x55667788;
    CLEAR_FLAGS;
    asm volatile (
        "movl %2, %%eax\n\t"
        "cmpxchgl %3, %0\n\t"
        : "+m" (*unaligned_ptr), "+a" (eax_val)
        : "r" (eax_val), "r" (src_val)
        : "cc"
    );
    flags = get_eflags();
    printf("Unaligned memory (failure):\n");
    printf("  Memory before: 0x%08X, after: 0x%08X\n", 0x11223344, *unaligned_ptr);
    printf("  EAX before: 0x%08X, after: 0x%08X\n", 0x12345678, eax_val);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
}

// Test LOCK prefix with CMPXCHG
static void test_lock_cmpxchg() {
    printf("\nTesting LOCK CMPXCHG:\n");
    printf("=====================\n");
    
    uint32_t shared_mem = 0;
    uint32_t eax_val, src_val;
    uint64_t flags;
    
    // Test with LOCK prefix (success)
    eax_val = 0;
    src_val = 0x12345678;
    CLEAR_FLAGS;
    asm volatile (
        "movl %3, %%eax\n\t"
        "lock cmpxchgl %4, %0\n\t"
        "pushfq\n\t"
        "pop %2\n\t"
        : "+m" (shared_mem), "+a" (eax_val), "=r"(flags)
        : "r" (eax_val), "r" (src_val)
        : "cc"
    );
    flags = get_eflags();
    printf("LOCK CMPXCHG (success):\n");
    printf("  Memory before: 0x%08X, after: 0x%08X\n", 0, shared_mem);
    printf("  EAX before: 0x%08X, after: 0x%08X\n", 0, eax_val);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Test with LOCK prefix (failure)
    shared_mem = 0x11223344;
    eax_val = 0;
    src_val = 0x55667788;
    CLEAR_FLAGS;
    asm volatile (
        "movl %3, %%eax\n\t"
        "lock cmpxchgl %4, %0\n\t"
        "pushfq\n\t"
        "pop %2\n\t"        
        : "+m" (shared_mem), "+a" (eax_val), "=r"(flags)
        : "r" (eax_val), "r" (src_val)
        : "cc"
    );
    flags = get_eflags();
    printf("LOCK CMPXCHG (failure):\n");
    printf("  Memory before: 0x%08X, after: 0x%08X\n", 0x11223344, shared_mem);
    printf("  EAX before: 0x%08X, after: 0x%08X\n", 0, eax_val);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
}

int main() {
    test_cmpxchg_reg();
    test_cmpxchg_mem();
    test_lock_cmpxchg();
    printf("\nCMPXCHG tests completed.\n");
    return 0;
}
