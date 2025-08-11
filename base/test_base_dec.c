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
        "pushfq\n\t"
        "popq %[rflags]\n\t"
        : "+r" (val8), [rflags] "=r" (flags)
        : 
        : "cc"
    );
    printf("8-bit (0x42 -> 0x41):\n");
    printf("  Result: 0x%02X\n", val8);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: Underflow
    val8 = 0x00;
    CLEAR_FLAGS;
    asm volatile (
        "decb %0\n\t"
        "pushfq\n\t"
        "popq %[rflags]\n\t"        
        : "+r" (val8), [rflags] "=r" (flags)
        : 
        : "cc"
    );
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
        "pushfq\n\t"
        "popq %[rflags]\n\t"               
        : "+r" (val16), [rflags] "=r" (flags)
        : 
        : "cc"
    );
    printf("16-bit (0x1234 -> 0x1233):\n");
    printf("  Result: 0x%04X\n", val16);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: Underflow
    val16 = 0x0000;
    CLEAR_FLAGS;
    asm volatile (
        "decw %0\n\t"
        "pushfq\n\t"
        "popq %[rflags]\n\t"               
        : "+r" (val16), [rflags] "=r" (flags)
        : 
        : "cc"
    );
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
        "pushfq\n\t"
        "popq %[rflags]\n\t"               
        : "+r" (val32), [rflags] "=r" (flags)
        : 
        : "cc"
    );
    printf("32-bit (0x12345678 -> 0x12345677):\n");
    printf("  Result: 0x%08X\n", val32);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: Underflow
    val32 = 0x00000000;
    CLEAR_FLAGS;
    asm volatile (
        "decl %0\n\t"
        "pushfq\n\t"
        "popq %[rflags]\n\t"               
        : "+r" (val32), [rflags] "=r" (flags)
        : 
        : "cc"
    );
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
        "pushfq\n\t"
        "popq %[rflags]\n\t"               
        : "+r" (val64), [rflags] "=r" (flags)
        : 
        : "cc"
    );
    printf("64-bit (0x123456789ABCDEF0 -> 0x123456789ABCDEEF):\n");
    printf("  Result: 0x%016lX\n", val64);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // Case 2: Underflow
    val64 = 0x0000000000000000;
    CLEAR_FLAGS;
    asm volatile (
        "decq %0\n\t"
        "pushfq\n\t"
        "popq %[rflags]\n\t"               
        : "+r" (val64), [rflags] "=r" (flags)
        : 
        : "cc"
    );
    printf("64-bit underflow (0x0000000000000000 -> 0xFFFFFFFFFFFFFFFF):\n");
    printf("  Result: 0x%016lX\n", val64);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
}

// Test DEC with memory operands
static void test_dec_mem() {
    printf("\nTesting DEC with memory operands:\n");
    printf("================================\n");
    
    char buffer[16] __attribute__((aligned(8)));
    uint64_t flags;
    
    // 8-bit tests
    uint8_t *mem8_aligned = (uint8_t*)&buffer[0];
    uint8_t *mem8_unaligned = (uint8_t*)&buffer[1];
    
    // 8-bit aligned
    *mem8_aligned = 0x42;
    CLEAR_FLAGS;
    asm volatile (
        "decb %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem8_aligned), "=r" (flags)
        : 
        : "cc"
    );
    printf("8-bit aligned (0x42 -> 0x41):\n");
    printf("  Result: 0x%02X\n", *mem8_aligned);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // 8-bit unaligned
    *mem8_unaligned = 0x42;
    CLEAR_FLAGS;
    asm volatile (
        "decb %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem8_unaligned), "=r" (flags)
        : 
        : "cc"
    );
    printf("8-bit unaligned (0x42 -> 0x41):\n");
    printf("  Result: 0x%02X\n", *mem8_unaligned);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // 16-bit tests
    uint16_t *mem16_aligned = (uint16_t*)&buffer[0];
    uint16_t *mem16_unaligned = (uint16_t*)&buffer[1];
    
    // 16-bit aligned
    *mem16_aligned = 0x1234;
    CLEAR_FLAGS;
    asm volatile (
        "decw %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem16_aligned), "=r" (flags)
        : 
        : "cc"
    );
    printf("16-bit aligned (0x1234 -> 0x1233):\n");
    printf("  Result: 0x%04X\n", *mem16_aligned);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // 16-bit unaligned
    *mem16_unaligned = 0x1234;
    CLEAR_FLAGS;
    asm volatile (
        "decw %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem16_unaligned), "=r" (flags)
        : 
        : "cc"
    );
    printf("16-bit unaligned (0x1234 -> 0x1233):\n");
    printf("  Result: 0x%04X\n", *mem16_unaligned);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // 32-bit tests
    uint32_t *mem32_aligned = (uint32_t*)&buffer[0];
    uint32_t *mem32_unaligned = (uint32_t*)&buffer[1];
    
    // 32-bit aligned
    *mem32_aligned = 0x12345678;
    CLEAR_FLAGS;
    asm volatile (
        "decl %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem32_aligned), "=r" (flags)
        : 
        : "cc"
    );
    printf("32-bit aligned (0x12345678 -> 0x12345677):\n");
    printf("  Result: 0x%08X\n", *mem32_aligned);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // 32-bit unaligned
    *mem32_unaligned = 0x12345678;
    CLEAR_FLAGS;
    asm volatile (
        "decl %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem32_unaligned), "=r" (flags)
        : 
        : "cc"
    );
    printf("32-bit unaligned (0x12345678 -> 0x12345677):\n");
    printf("  Result: 0x%08X\n", *mem32_unaligned);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // 64-bit tests
    uint64_t *mem64_aligned = (uint64_t*)&buffer[0];
    uint64_t *mem64_unaligned = (uint64_t*)&buffer[1];
    
    // 64-bit aligned
    *mem64_aligned = 0x123456789ABCDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "decq %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem64_aligned), "=r" (flags)
        : 
        : "cc"
    );
    printf("64-bit aligned (0x123456789ABCDEF0 -> 0x123456789ABCDEEF):\n");
    printf("  Result: 0x%016lX\n", *mem64_aligned);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // 64-bit unaligned
    *mem64_unaligned = 0x123456789ABCDEF0;
    CLEAR_FLAGS;
    asm volatile (
        "decq %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem64_unaligned), "=r" (flags)
        : 
        : "cc"
    );
    printf("64-bit unaligned (0x123456789ABCDEF0 -> 0x123456789ABCDEEF):\n");
    printf("  Result: 0x%016lX\n", *mem64_unaligned);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
}

// Test LOCK prefix with DEC
static void test_lock_dec() {
    printf("\nTesting LOCK DEC:\n");
    printf("=================\n");
    
    char buffer[16] __attribute__((aligned(8)));
    uint64_t flags;
    
    // 8-bit LOCK DEC
    uint8_t *mem8 = (uint8_t*)&buffer[0];
    *mem8 = 0x00;
    CLEAR_FLAGS;
    asm volatile (
        "lock decb %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem8), "=r" (flags)
        : 
        : "cc"
    );
    printf("LOCK DEC 8-bit underflow (0x00 -> 0xFF):\n");
    printf("  Result: 0x%02X\n", *mem8);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // 16-bit LOCK DEC
    uint16_t *mem16 = (uint16_t*)&buffer[0];
    *mem16 = 0x0000;
    CLEAR_FLAGS;
    asm volatile (
        "lock decw %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem16), "=r" (flags)
        : 
        : "cc"
    );
    printf("LOCK DEC 16-bit underflow (0x0000 -> 0xFFFF):\n");
    printf("  Result: 0x%04X\n", *mem16);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // 32-bit LOCK DEC
    uint32_t *mem32 = (uint32_t*)&buffer[0];
    *mem32 = 0x00000000;
    CLEAR_FLAGS;
    asm volatile (
        "lock decl %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem32), "=r" (flags)
        : 
        : "cc"
    );
    printf("LOCK DEC 32-bit underflow (0x00000000 -> 0xFFFFFFFF):\n");
    printf("  Result: 0x%08X\n", *mem32);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
    
    // 64-bit LOCK DEC
    uint64_t *mem64 = (uint64_t*)&buffer[0];
    *mem64 = 0x0000000000000000;
    CLEAR_FLAGS;
    asm volatile (
        "lock decq %0\n\t"
        "pushfq\n\t"
        "popq %1\n\t"
        : "+m" (*mem64), "=r" (flags)
        : 
        : "cc"
    );
    printf("LOCK DEC 64-bit underflow (0x0000000000000000 -> 0xFFFFFFFFFFFFFFFF):\n");
    printf("  Result: 0x%016lX\n", *mem64);
    printf("  Flags: 0x%03lX\n", flags & 0x8D5);
}

int main() {
    test_dec_reg();
    test_dec_mem();
    test_lock_dec();
    printf("\nDEC tests completed.\n");
    return 0;
}
