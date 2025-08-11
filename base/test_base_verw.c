#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Helper function to check ZF flag after VERW instruction
static void check_verw_result(int expected, uint64_t eflags) {
    int zf = (eflags & X_ZF) != 0;
    printf("  Expected ZF: %d, Actual ZF: %d\n", expected, zf);
    if (zf != expected) {
        printf("    ERROR: ZF expected %d, got %d\n", expected, zf);
    } else {
        printf("    OK\n");
    }
}

// Test VERW with register operand
static void test_verw_reg() {
    printf("Testing VERW with register operand:\n");
    
    uint16_t seg_reg;
    uint64_t eflags;
    
    // Test valid writable segment (SS register)
    __asm__ volatile (
        "mov %%ss, %[seg]\n\t"
        "verw %[seg]\n\t"
        "pushfq\n\t"
        "pop %[eflags]"
        : [seg] "=r" (seg_reg), [eflags] "=r" (eflags)
        : 
        : "cc"
    );
    printf("SS register: 0x%04x\n", seg_reg);
    check_verw_result(1, eflags);
    
    // Test invalid segment (0)
    __asm__ volatile (
        "mov $0, %[seg]\n\t"
        "verw %[seg]\n\t"
        "pushfq\n\t"
        "pop %[eflags]"
        : [seg] "=r" (seg_reg), [eflags] "=r" (eflags)
        : 
        : "cc"
    );
    printf("Null selector: 0x%04x\n", seg_reg);
    check_verw_result(0, eflags);
}

// Test VERW with memory operand
static void test_verw_mem() {
    printf("Testing VERW with memory operand:\n");
    
    uint16_t seg_val;
    uint64_t eflags;
    
    // Use SS segment which we know is valid
    __asm__ volatile ("mov %%ss, %0" : "=r" (seg_val));
    
    // Test valid writable segment (SS)
    __asm__ volatile (
        "verw %[mem]\n\t"
        "pushfq\n\t"
        "pop %[eflags]"
        : [eflags] "=r" (eflags)
        : [mem] "m" (seg_val)
        : "cc"
    );
    printf("SS selector: 0x%04x\n", seg_val);
    check_verw_result(1, eflags);
    
    // Test invalid segment (0)
    seg_val = 0;
    __asm__ volatile (
        "verw %[mem]\n\t"
        "pushfq\n\t"
        "pop %[eflags]"
        : [eflags] "=r" (eflags)
        : [mem] "m" (seg_val)
        : "cc"
    );
    printf("Null selector: 0x%04x\n", seg_val);
    check_verw_result(0, eflags);
    
    // Test with explicit memory location
    uint16_t mem_location = 0;
    __asm__ volatile ("mov %%ss, %0" : "=r" (mem_location));
    
    __asm__ volatile (
        "verw %0\n\t"
        "pushfq\n\t"
        "pop %1"
        : "=m" (mem_location), "=r" (eflags)
        : 
        : "cc"
    );
    printf("Explicit memory location: 0x%04x\n", mem_location);
    check_verw_result(1, eflags);
}

int main() {
    printf("VERW instruction test\n");
    printf("=====================\n");
    
    test_verw_reg();
    test_verw_mem();
    
    printf("\nVERW test completed\n");
    printf("==================\n");
    
    return 0;
}
