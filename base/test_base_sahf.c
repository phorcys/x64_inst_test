#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Helper function to print flags set by SAHF
void print_sahf_flags(uint64_t flags) {
    printf("SF: %lu, ZF: %lu, AF: %lu, PF: %lu, CF: %lu", 
           (flags >> 7) & 1,  // SF is bit 7
           (flags >> 6) & 1,  // ZF is bit 6
           (flags >> 4) & 1,  // AF is bit 4
           (flags >> 2) & 1,  // PF is bit 2
           (flags >> 0) & 1); // CF is bit 0
}

int main() {
    printf("SAHF instruction test\n");
    printf("=====================\n");
    
    int errors = 0;
    uint8_t ah_val;
    uint64_t flags;
    
    // Test 1: Set all flags
    ah_val = 0xD7; // 11010111 (SF=1, ZF=1, AF=1, PF=1, CF=1) - corrected AF=1
    __asm__ __volatile__ (
        "mov %[ah_val], %%ah\n\t"
        "sahf\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (flags)
        : [ah_val] "r" (ah_val)
        : "ah", "cc"
    );

    uint64_t expected_flags = 0xD7; // SF=1, ZF=1, AF=1, PF=1, CF=1 (bit7,6,4,2,0) + reserved bits
    if ((flags & 0xFF) != expected_flags) {
        printf("Error: SAHF (all flags) expected 0x%lx, got 0x%lx\n", expected_flags, flags & 0xFF);
        errors++;
    } else {
        printf("SAHF (all flags): PASS - ");
        print_sahf_flags(flags);
        printf("\n");
    }
    
    // Test 2: Clear all flags
    ah_val = 0x00; // 00000000 (all flags cleared)
    __asm__ __volatile__ (
        "mov %[ah_val], %%ah\n\t"
        "sahf\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (flags)
        : [ah_val] "r" (ah_val)
        : "ah", "cc"
    );

    expected_flags = 0x02; // Reserved bit 1 is always set
    if ((flags & 0xFF) != expected_flags) {
        printf("Error: SAHF (clear flags) expected 0x%lx, got 0x%lx\n", expected_flags, flags & 0xFF);
        errors++;
    } else {
        printf("SAHF (clear flags): PASS - ");
        print_sahf_flags(flags);
        printf("\n");
    }
    
    // Test 3: Partial flags (SF=1, ZF=0, AF=1, PF=1, CF=1) - corrected PF=1
    ah_val = 0xB5; // 10110101 (SF=1, ZF=0, AF=1, PF=1, CF=1)
    __asm__ __volatile__ (
        "mov %[ah_val], %%ah\n\t"
        "sahf\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (flags)
        : [ah_val] "r" (ah_val)
        : "ah", "cc"
    );

    expected_flags = 0x97; // SF=1, AF=1, PF=1, CF=1 + reserved bits
    if ((flags & 0xFF) != expected_flags) {
        printf("Error: SAHF (partial flags) expected 0x%lx, got 0x%lx\n", expected_flags, flags & 0xFF);
        errors++;
    } else {
        printf("SAHF (partial flags): PASS - ");
        print_sahf_flags(flags);
        printf("\n");
    }
    
    // Test 4: Check behavior with conditional jumps (using SAHF to set ZF)
    ah_val = 0x40; // 01000000 (ZF=1)
    int jump_taken = 0;
    __asm__ __volatile__ (
        "mov %[ah_val], %%ah\n\t"
        "sahf\n\t"
        "jnz not_taken\n\t"
        "mov $1, %[result]\n\t"
        "jmp done\n\t"
        "not_taken:\n\t"
        "mov $0, %[result]\n\t"
        "done:"
        : [result] "=r" (jump_taken)
        : [ah_val] "r" (ah_val)
        : "ah", "cc"
    );
    
    if (!jump_taken) {
        printf("Error: SAHF conditional jump failed. Expected JZ to be taken\n");
        errors++;
    } else {
        printf("SAHF conditional jump (ZF=1): PASS\n");
    }
    
    // Test 5: Check carry flag behavior
    ah_val = 0x01; // 00000001 (CF=1)
    uint64_t result = 0;
    __asm__ __volatile__ (
        "mov %[ah_val], %%ah\n\t"
        "sahf\n\t"
        "mov $0, %%rax\n\t"
        "adc $0, %%rax\n\t"  // Add with carry (0 + 0 + CF)
        "mov %%rax, %[result]"
        : [result] "=r" (result)
        : [ah_val] "r" (ah_val)
        : "rax", "ah", "cc"
    );
    
    if (result != 1) {
        printf("Error: SAHF CF behavior failed. Expected 1, got %lu\n", result);
        errors++;
    } else {
        printf("SAHF CF behavior: PASS\n");
    }
    
    // Test 6: Verify SAHF doesn't affect higher bits of FLAGS
    uint64_t original_flags;
    ah_val = 0x80; // 10000000 (SF=1)
    
    // Get original flags
    __asm__ __volatile__ (
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (original_flags)
        :
        : "cc"
    );
    
    __asm__ __volatile__ (
        "mov %[ah_val], %%ah\n\t"
        "sahf\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (flags)
        : [ah_val] "r" (ah_val)
        : "ah", "cc"
    );
    
    // Mask to only the bits affected by SAHF (bits 0, 2, 4, 6, 7)
    uint64_t sahf_mask = (1<<0) | (1<<2) | (1<<4) | (1<<6) | (1<<7);
    uint64_t unchanged_bits = (original_flags & ~sahf_mask) ^ (flags & ~sahf_mask);
    
    if (unchanged_bits != 0) {
        printf("Error: SAHF modified non-target flags. Changed bits: 0x%lx\n", unchanged_bits);
        errors++;
    } else {
        printf("SAHF non-target flags: PASS\n");
    }
    
    printf("\nSAHF test completed\n");
    printf("===================\n");
    printf("Total tests: 6\n");
    printf("Passed: %d\n", 6 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
