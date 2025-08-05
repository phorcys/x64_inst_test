#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Test DEC with register operands
static void test_dec_reg() {
    printf("Testing DEC with register operands:\n");
    printf("==================================\n");
    
    // Test 8-bit
    uint8_t val8;
    uint64_t flags;
    
    // Case 1: Normal decrement
    val8 = 0x42;
    CLEAR_FLAGS;
    asm volatile (
        "decb %0\n\t"
        : "+r" (val8)
        : 
        : "cc"
    );
    flags = get_eflags();
    printf("8-bit (0x42 -> 0x41):\n");
    printf("  Result: 0x%02X\n", val8);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: Underflow
    val8 = 0x00;
    CLEAR_FLAGS;
    asm volatile (
        "decb %0\n\t"
        : "+r" (val8)
        : 
        : "cc"
    );
    flags = get_eflags();
    printf("8-bit underflow (0x00 -> 0xFF):\n");
    printf("  Result: 0x%02X\n", val8);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Test 16-bit
    uint16_t val16;
    
    // Case 1: Normal decrement
    val16 = 0x1234;
    CLEAR_FLAGS;
    asm volatile (
        "decw %0\n\t"
        : "+r" (val16)
        : 
        : "cc"
    );
    flags = get_eflags();
    printf("16-bit (0x1234 -> 0x1233):\n");
    printf("  Result: 0x%04X\n", val16);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: Underflow
    val16 = 0x0000;
    CLEAR_FLAGS;
    asm volatile (
        "decw %0\n\t"
        : "+r" (val16)
        : 
        : "cc"
    );
    flags = get_eflags();
    printf("16-bit underflow (0x0000 -> 0xFFFF):\n");
    printf("  Result: 0x%04X\n", val16);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Test 32-bit
    uint32_t val32;
    
    // Case 1: Normal decrement
    val32 = 0x12345678;
    CLEAR_FLAGS;
    asm volatile (
        "decl %0\n\t"
        : "+r" (val32)
        : 
        : "cc"
    );
    flags = get_eflags();
    printf("32-bit (0x12345678 -> 0x12345677):\n");
    printf("  Result: 0x%08X\n", val32);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: Underflow
    val32 = 0x00000000;
    CLEAR_FLAGS;
    asm volatile (
        "decl %0\n\t"
        : "+r" (val32)
        : 
        : "cc"
    );
    flags = get_eflags();
    printf("32-bit underflow (0x00000000 -> 0xFFFFFFFF):\n");
    printf("  Result: 0x%08X\n", val32);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Test 64-bit
    uint64_t val64;
    
    // Case 1: Normal decrement
    val64 = 0x123456789ABCDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "decq %0\n\t"
        : "+r" (val64)
        : 
        : "cc"
    );
    flags = get_eflags();
    printf("64-bit (0x123456789ABCDEF0 -> 0x123456789ABCDEEF):\n");
    printf("  Result: 0x%016lX\n", val64);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: Underflow
    val64 = 0x0000000000000000;
    CLEAR_FLAGS;
    asm volatile (
        "decq %0\n\t"
        : "+r" (val64)
        : 
        : "cc"
    );
    flags = get_eflags();
    printf("64-bit underflow (0x0000000000000000 -> 0xFFFFFFFFFFFFFFFF):\n");
    printf("  Result: 0x%016lX\n", val64);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
}

// Test DEC with memory operands
static void test_dec_mem() {
    printf("\nTesting DEC with memory operands:\n");
    printf("================================\n");
    
    // Aligned memory
    uint32_t aligned_mem __attribute__((aligned(4)));
    uint32_t unaligned_mem;
    uint32_t *unaligned_ptr = (uint32_t*)((char*)&unaligned_mem + 1);
    
    uint64_t flags;
    
    // Test aligned memory
    aligned_mem = 0x12345678;
    CLEAR_FLAGS;
    asm volatile (
        "decl %0\n\t"
        : "+m" (aligned_mem)
        : 
        : "cc"
    );
    flags = get_eflags();
    printf("Aligned memory (0x12345678 -> 0x12345677):\n");
    printf("  Result: 0x%08X\n", aligned_mem);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Test unaligned memory
    *unaligned_ptr = 0x12345678;
    CLEAR_FLAGS;
    asm volatile (
        "decl %0\n\t"
        : "+m" (*unaligned_ptr)
        : 
        : "cc"
    );
    flags = get_eflags();
    printf("Unaligned memory (0x12345678 -> 0x12345677):\n");
    printf("  Result: 0x%08X\n", *unaligned_ptr);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
}

// Test LOCK prefix with DEC
static void test_lock_dec() {
    printf("\nTesting LOCK DEC:\n");
    printf("=================\n");
    
    uint32_t shared_mem = 0x12345678;
    uint64_t flags;
    
    CLEAR_FLAGS;
    asm volatile (
        "lock decl %0\n\t"
        : "+m" (shared_mem)
        : 
        : "cc"
    );
    flags = get_eflags();
    printf("LOCK DEC (0x12345678 -> 0x12345677):\n");
    printf("  Result: 0x%08X\n", shared_mem);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
}

int main() {
    test_dec_reg();
    test_dec_mem();
    test_lock_dec();
    printf("\nDEC tests completed.\n");
    return 0;
}
