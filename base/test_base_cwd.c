#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Test CWD (16-bit)
static void test_cwd() {
    printf("Testing CWD (16-bit):\n");
    printf("=====================\n");
    
    uint16_t ax;
    uint16_t dx;
    uint64_t flags;
    
    // Test positive value
    ax = 0x7FFF;
    CLEAR_FLAGS;
    asm volatile (
        "cwd\n\t"
        : "=d" (dx)
        : "a" (ax)
        : "cc"
    );
    printf("Positive value (0x%04X):\n", ax);
    printf("  DX:AX = 0x%04X:%04X\n", dx, ax);
    
    // Test negative value
    ax = 0x8000;
    CLEAR_FLAGS;
    asm volatile (
        "cwd\n\t"
        : "=d" (dx)
        : "a" (ax)
        : "cc"
    );
    printf("Negative value (0x%04X):\n", ax);
    printf("  DX:AX = 0x%04X:%04X\n", dx, ax);
}

// Test CDQ (32-bit)
static void test_cdq() {
    printf("\nTesting CDQ (32-bit):\n");
    printf("=====================\n");
    
    uint32_t eax;
    uint32_t edx;
    uint64_t flags;
    
    // Test positive value
    eax = 0x7FFFFFFF;
    CLEAR_FLAGS;
    asm volatile (
        "cdq\n\t"
        : "=d" (edx)
        : "a" (eax)
        : "cc"
    );
    printf("Positive value (0x%08X):\n", eax);
    printf("  EDX:EAX = 0x%08X:%08X\n", edx, eax);
    
    // Test negative value
    eax = 0x80000000;
    CLEAR_FLAGS;
    asm volatile (
        "cdq\n\t"
        : "=d" (edx)
        : "a" (eax)
        : "cc"
    );
    printf("Negative value (0x%08X):\n", eax);
    printf("  EDX:EAX = 0x%08X:%08X\n", edx, eax);
}

// Test CQO (64-bit)
static void test_cqo() {
    printf("\nTesting CQO (64-bit):\n");
    printf("=====================\n");
    
    uint64_t rax;
    uint64_t rdx;
    uint64_t flags;
    
    // Test positive value
    rax = 0x7FFFFFFFFFFFFFFF;
    CLEAR_FLAGS;
    asm volatile (
        "cqo\n\t"
        : "=d" (rdx)
        : "a" (rax)
        : "cc"
    );
    printf("Positive value (0x%016lX):\n", rax);
    printf("  RDX:RAX = 0x%016lX:%016lX\n", rdx, rax);
    
    // Test negative value
    rax = 0x8000000000000000;
    CLEAR_FLAGS;
    asm volatile (
        "cqo\n\t"
        : "=d" (rdx)
        : "a" (rax)
        : "cc"
    );
    printf("Negative value (0x%016lX):\n", rax);
    printf("  RDX:RAX = 0x%016lX:%016lX\n", rdx, rax);
}

int main() {
    test_cwd();
    test_cdq();
    test_cqo();
    printf("\nCWD/CDQ/CQO tests completed.\n");
    return 0;
}
