#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("MUL instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    
    // Test 8-bit MUL
    uint8_t al = 100;
    uint8_t bl = 5;
    uint16_t ax_result;
    
    __asm__ __volatile__ (
        "mov %[src], %%cl\n\t"
        "mov %[val], %%al\n\t"
        "mul %%cl\n\t"
        : "=a" (ax_result)
        : [val] "r" (al), [src] "r" (bl)
        : "ecx"
    );
    
    if (ax_result != 500) {
        printf("Error: 8-bit MUL expected 500, got %u\n", ax_result);
        errors++;
    } else {
        printf("8-bit MUL (small): PASS\n");
    }
    
    // Test 8-bit MUL with overflow
    al = 200;
    bl = 200;
    
    __asm__ __volatile__ (
        "mov %[src], %%cl\n\t"
        "mov %[val], %%al\n\t"
        "mul %%cl\n\t"
        : "=a" (ax_result)
        : [val] "r" (al), [src] "r" (bl)
        : "ecx"
    );
    
    if (ax_result != 40000) {
        printf("Error: 8-bit MUL (overflow) expected 40000, got %u\n", ax_result);
        errors++;
    } else {
        printf("8-bit MUL (overflow): PASS\n");
    }
    
    // Test 16-bit MUL
    uint16_t ax_val = 1000;
    uint16_t bx_val = 100;
    uint16_t ax_result_low;
    uint16_t dx_result;
    uint32_t dxax_result;
    
    __asm__ __volatile__ (
        "mov %[src], %%cx\n\t"
        "mov %[val], %%ax\n\t"
        "mul %%cx\n\t"
        : "=a" (ax_result_low), "=d" (dx_result)
        : [val] "r" (ax_val), [src] "r" (bx_val)
        : "ecx"
    );
    dxax_result = (dx_result << 16) | ax_result_low;
    
    if (dxax_result != 100000) {
        printf("Error: 16-bit MUL expected 100000, got %u\n", dxax_result);
        errors++;
    } else {
        printf("16-bit MUL: PASS\n");
    }
    
    // Test 32-bit MUL
    uint32_t eax_val = 100000;
    uint32_t ebx_val = 1000;
    uint32_t eax_result;
    uint32_t edx_result;
    uint64_t edxeax_result;
    
    __asm__ __volatile__ (
        "mov %[src], %%ecx\n\t"
        "mov %[val], %%eax\n\t"
        "mul %%ecx\n\t"
        : "=a" (eax_result), "=d" (edx_result)
        : [val] "r" (eax_val), [src] "r" (ebx_val)
        : "ecx"
    );
    edxeax_result = ((uint64_t)edx_result << 32) | eax_result;
    
    if (edxeax_result != 100000000) {
        printf("Error: 32-bit MUL expected 100000000, got %lu\n", edxeax_result);
        errors++;
    } else {
        printf("32-bit MUL: PASS\n");
    }
    
    // Test 64-bit MUL without overflow
    uint64_t rax_val = 1000000000;
    uint64_t rbx_val = 1000000;
    uint64_t rax_result_low;
    uint64_t rdx_result;
    
    __asm__ __volatile__ (
        "mov %[src], %%rcx\n\t"
        "mov %[val], %%rax\n\t"
        "mul %%rcx\n\t"
        : "=a" (rax_result_low), "=d" (rdx_result)
        : [val] "r" (rax_val), [src] "r" (rbx_val)
        : "rcx"
    );
    
    if (rdx_result != 0 || rax_result_low != 1000000000000000) {
        printf("Error: 64-bit MUL (no overflow) expected 1000000000000000, got %lu (high=%lu)\n", rax_result_low, rdx_result);
        errors++;
    } else {
        printf("64-bit MUL (no overflow): PASS\n");
    }
    
    // Test 64-bit MUL with overflow (high bits non-zero)
    rax_val = 0x100000000;
    rbx_val = 0x100000000;
    __asm__ __volatile__ (
        "mov %[src], %%rcx\n\t"
        "mov %[val], %%rax\n\t"
        "mul %%rcx\n\t"
        : "=a" (rax_result_low), "=d" (rdx_result)
        : [val] "r" (rax_val), [src] "r" (rbx_val)
        : "rcx"
    );
    if (rdx_result != 1 || rax_result_low != 0) {
        printf("Error: 64-bit MUL (overflow) expected 0x10000000000000000, got 0x%lx (high=0x%lx)\n", rax_result_low, rdx_result);
        errors++;
    } else {
        printf("64-bit MUL (overflow): PASS\n");
    }
    
    printf("\nMUL test completed\n");
    printf("==================\n");
    printf("Total tests: 6\n");
    printf("Passed: %d\n", 6 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
