#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "base.h"

// Global variable for direct addressing test
uint64_t direct_addr = 0xDEADBEEF;

int main() {
    int errors = 0;
    uint64_t base = 0x1000;
    uint64_t index = 0x100;
    uint64_t result64;
    uint32_t result32;
    uint16_t result16;

    printf("Starting LEA tests\n");
    printf("==================\n\n");

    // Test 1: 64-bit operand, 64-bit addressing (REX.W)
    __asm__ __volatile__(
        "mov %[base], %%rbx\n\t"
        "mov %[index], %%rcx\n\t"
        "lea 0x12345678(%%rbx, %%rcx, 4), %[result]"
        : [result] "=r" (result64)
        : [base] "r" (base), [index] "r" (index)
        : "rbx", "rcx"
    );

    uint64_t expected = base + index * 4 + 0x12345678;
    printf("Test 1: LEA with 64-bit operand (expected: 0x%" PRIx64 ")\n", expected);
    printf("  Result: 0x%" PRIx64 " - %s\n", result64, 
           result64 == expected ? "PASS" : "FAIL");
    if (result64 != expected) errors++;

    // Test 2: 32-bit operand, 64-bit addressing (truncation)
    __asm__ __volatile__(
        "mov %[base], %%rbx\n\t"
        "mov %[index], %%rcx\n\t"
        "lea 0x12345678(%%rbx, %%rcx, 4), %[result]"
        : [result] "=r" (result32)
        : [base] "r" (base), [index] "r" (index)
        : "rbx", "rcx"
    );

    uint32_t expected32 = (uint32_t)expected;
    printf("\nTest 2: LEA with 32-bit operand (expected: 0x%x)\n", expected32);
    printf("  Result: 0x%x - %s\n", result32, 
           result32 == expected32 ? "PASS" : "FAIL");
    if (result32 != expected32) errors++;

    // Test 3: 16-bit operand, 64-bit addressing (truncation)
    __asm__ __volatile__(
        "mov %[base], %%rbx\n\t"
        "mov %[index], %%rcx\n\t"
        "lea 0x12345678(%%rbx, %%rcx, 4), %[result]"
        : [result] "=r" (result16)
        : [base] "r" (base), [index] "r" (index)
        : "rbx", "rcx"
    );

    uint16_t expected16 = (uint16_t)expected;
    printf("\nTest 3: LEA with 16-bit operand (expected: 0x%x)\n", expected16);
    printf("  Result: 0x%x - %s\n", result16, 
           result16 == expected16 ? "PASS" : "FAIL");
    if (result16 != expected16) errors++;

    // Test 4: 64-bit operand, 32-bit addressing (address size override)
    __asm__ __volatile__(
        "mov %[base], %%ebx\n\t"
        "mov %[index], %%ecx\n\t"
        "lea 0x12345678(%%ebx, %%ecx, 4), %[result]"
        : [result] "=r" (result64)
        : [base] "r" ((uint32_t)base), [index] "r" ((uint32_t)index)
        : "rbx", "rcx"
    );

    expected = (uint32_t)(base + index * 4 + 0x12345678);
    printf("\nTest 4: LEA with 32-bit addressing (expected: 0x%" PRIx64 ")\n", expected);
    printf("  Result: 0x%" PRIx64 " - %s\n", result64, 
           result64 == expected ? "PASS" : "FAIL");
    if (result64 != expected) errors++;

    // Test 5: Simple direct addressing
    uint64_t expected_addr = (uint64_t)&direct_addr;
    uint64_t result_addr;
    
    __asm__ __volatile__(
        "lea direct_addr(%%rip), %[result]"
        : [result] "=r" (result_addr)
        :: 
    );

    printf("\nTest 5: Direct addressing");
    printf("  Result: %s\n", 
           result_addr == expected_addr ? "PASS" : "FAIL");
    if (result_addr != expected_addr) errors++;

    // Test 6: Register indirect
    uint64_t value = 0xCAFEBABE;
    __asm__ __volatile__(
        "mov %[value], %%rbx\n\t"
        "lea (%%rbx), %[result]"
        : [result] "=r" (result64)
        : [value] "r" (value)
        : "rbx"
    );

    printf("\nTest 6: Register indirect (expected: 0x%" PRIx64 ")\n", value);
    printf("  Result: 0x%" PRIx64 " - %s\n", result64, 
           result64 == value ? "PASS" : "FAIL");
    if (result64 != value) errors++;

    // Test 7: Base + displacement
    __asm__ __volatile__(
        "mov %[base], %%rbx\n\t"
        "lea 0x20(%%rbx), %[result]"
        : [result] "=r" (result64)
        : [base] "r" (base)
        : "rbx"
    );

    expected = base + 0x20;
    printf("\nTest 7: Base + displacement (expected: 0x%" PRIx64 ")\n", expected);
    printf("  Result: 0x%" PRIx64 " - %s\n", result64, 
           result64 == expected ? "PASS" : "FAIL");
    if (result64 != expected) errors++;

    // Test 8: Negative displacement
    __asm__ __volatile__(
        "mov %[base], %%rbx\n\t"
        "lea -0x20(%%rbx), %[result]"
        : [result] "=r" (result64)
        : [base] "r" (base)
        : "rbx"
    );

    expected = base - 0x20;
    printf("\nTest 8: Negative displacement (expected: 0x%" PRIx64 ")\n", expected);
    printf("  Result: 0x%" PRIx64 " - %s\n", result64, 
           result64 == expected ? "PASS" : "FAIL");
    if (result64 != expected) errors++;

    // Test 9: Base + index
    __asm__ __volatile__(
        "mov %[base], %%rbx\n\t"
        "mov %[index], %%rcx\n\t"
        "lea (%%rbx, %%rcx), %[result]"
        : [result] "=r" (result64)
        : [base] "r" (base), [index] "r" (index)
        : "rbx", "rcx"
    );

    expected = base + index;
    printf("\nTest 9: Base + index (expected: 0x%" PRIx64 ")\n", expected);
    printf("  Result: 0x%" PRIx64 " - %s\n", result64, 
           result64 == expected ? "PASS" : "FAIL");
    if (result64 != expected) errors++;

    // Test 10: Different scale factor
    __asm__ __volatile__(
        "mov %[base], %%rbx\n\t"
        "mov %[index], %%rcx\n\t"
        "lea (%%rbx, %%rcx, 8), %[result]"
        : [result] "=r" (result64)
        : [base] "r" (base), [index] "r" (index)
        : "rbx", "rcx"
    );

    expected = base + index * 8;
    printf("\nTest 10: Scale factor 8 (expected: 0x%" PRIx64 ")\n", expected);
    printf("  Result: 0x%" PRIx64 " - %s\n", result64, 
           result64 == expected ? "PASS" : "FAIL");
    if (result64 != expected) errors++;

    printf("\nLEA tests completed\n");
    printf("====================\n");
    printf("Total tests: 10\n");
    printf("Passed: %d\n", 10 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
