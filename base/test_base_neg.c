#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("NEG instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    uint16_t flags;
    
    // Test 8-bit NEG
    int8_t val8 = 5;
    int8_t result8;
    __asm__ __volatile__ (
        "mov %[val], %%al\n\t"
        "neg %%al\n\t"
        "pushfq\n\t"
        "popq %%rdx\n\t"
        : "=a" (result8), "=d" (flags)
        : [val] "r" (val8)
    );
    
    if (result8 != -5) {
        printf("Error: 8-bit NEG expected -5, got %d\n", result8);
        errors++;
    } else if (!(flags & (1 << 0))) { // Check CF (should be set for non-zero)
        printf("Error: 8-bit NEG flags expected CF=1, got flags=0x%x\n", flags);
        errors++;
    } else {
        printf("8-bit NEG (positive): PASS\n");
    }
    
    // Test 8-bit NEG with zero
    val8 = 0;
    __asm__ __volatile__ (
        "mov %[val], %%al\n\t"
        "neg %%al\n\t"
        "pushfq\n\t"
        "popq %%rdx\n\t"
        : "=a" (result8), "=d" (flags)
        : [val] "r" (val8)
    );
    
    if (result8 != 0) {
        printf("Error: 8-bit NEG (zero) expected 0, got %d\n", result8);
        errors++;
    } else if (flags & (1 << 0)) { // Check CF (should be clear for zero)
        printf("Error: 8-bit NEG (zero) flags expected CF=0, got flags=0x%x\n", flags);
        errors++;
    } else {
        printf("8-bit NEG (zero): PASS\n");
    }
    
    // Test 8-bit NEG with negative (should be positive)
    val8 = -42;
    __asm__ __volatile__ (
        "mov %[val], %%al\n\t"
        "neg %%al\n\t"
        : "=a" (result8)
        : [val] "r" (val8)
    );
    
    if (result8 != 42) {
        printf("Error: 8-bit NEG (negative) expected 42, got %d\n", result8);
        errors++;
    } else {
        printf("8-bit NEG (negative): PASS\n");
    }
    
    // Test 16-bit NEG
    int16_t val16 = 1000;
    int16_t result16;
    __asm__ __volatile__ (
        "mov %[val], %%ax\n\t"
        "neg %%ax\n\t"
        : "=a" (result16)
        : [val] "r" (val16)
    );
    
    if (result16 != -1000) {
        printf("Error: 16-bit NEG expected -1000, got %d\n", result16);
        errors++;
    } else {
        printf("16-bit NEG: PASS\n");
    }
    
    // Test 32-bit NEG
    int32_t val32 = 100000;
    int32_t result32;
    __asm__ __volatile__ (
        "mov %[val], %%eax\n\t"
        "neg %%eax\n\t"
        : "=a" (result32)
        : [val] "r" (val32)
    );
    
    if (result32 != -100000) {
        printf("Error: 32-bit NEG expected -100000, got %d\n", result32);
        errors++;
    } else {
        printf("32-bit NEG: PASS\n");
    }
    
    // Test 64-bit NEG
    int64_t val64 = 10000000000;
    int64_t result64;
    __asm__ __volatile__ (
        "mov %[val], %%rax\n\t"
        "neg %%rax\n\t"
        : "=a" (result64)
        : [val] "r" (val64)
    );
    
    if (result64 != -10000000000) {
        printf("Error: 64-bit NEG expected -10000000000, got %ld\n", result64);
        errors++;
    } else {
        printf("64-bit NEG: PASS\n");
    }
    
    // Test memory operand
    int16_t mem_val = 1234;
    int16_t mem_result;
    __asm__ __volatile__ (
        "negw %[mem]\n\t"
        : [mem] "+m" (mem_val)
    );
    mem_result = mem_val;
    
    if (mem_result != -1234) {
        printf("Error: Memory operand NEG expected -1234, got %d\n", mem_result);
        errors++;
    } else {
        printf("Memory operand: PASS\n");
    }
    
    // Add stack alignment to prevent segfault
    __asm__ __volatile__ ("nop");
    
    // Test overflow case (most negative value)
    val8 = -128; // Most negative 8-bit value
    __asm__ __volatile__ (
        "mov %[val], %%al\n\t"
        "neg %%al\n\t"
        "pushfq\n\t"
        "popq %%rdx\n\t"
        : "=a" (result8), "=d" (flags)
        : [val] "r" (val8)
    );
    
    // Result should be -128 (since 128 can't be represented in 8-bit signed)
    if (result8 != -128) {
        printf("Error: 8-bit NEG (overflow) expected -128, got %d\n", result8);
        errors++;
    } else if (!(flags & (1 << 11))) { // Check OF (should be set)
        printf("Error: 8-bit NEG (overflow) flags expected OF=1, got flags=0x%x\n", flags);
        errors++;
    } else {
        printf("Overflow case: PASS\n");
    }
    
    printf("\nNEG test completed\n");
    printf("==================\n");
    printf("Total tests: 8\n");
    printf("Passed: %d\n", 8 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
