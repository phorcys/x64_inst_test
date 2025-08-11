#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Helper function to print flags affected by ROR
void print_ror_flags(uint64_t flags) {
    printf("CF: %lu, OF: %lu", 
           (flags >> 0) & 1,  // CF is bit 0
           (flags >> 11) & 1); // OF is bit 11
}

int main() {
    printf("ROR instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    uint8_t val8, result8;
    uint16_t val16, result16;
    uint32_t val32, result32;
    uint64_t val64, result64;
    uint64_t flags;
    
    // Test 8-bit ROR with count=1
    val8 = 0x03; // 00000011
    result8 = val8;
    __asm__ __volatile__ (
        "rorb $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result8 != 0x81) { // 10000001 (CF=1)
        printf("Error: 8-bit ROR(1) expected 0x81, got 0x%x\n", result8);
        errors++;
    } else if (!(flags & (1 << 0))) { // Check CF
        printf("Error: 8-bit ROR(1) expected CF=1\n");
        errors++;
    } else {
        printf("8-bit ROR(1): PASS - ");
        print_ror_flags(flags);
        printf("\n");
    }
    
    // Test 16-bit ROR with count=4
    val16 = 0x1234; // 00010010 00110100
    result16 = val16;
    __asm__ __volatile__ (
        "rorw $4, %[val]"
        : [val] "+r" (result16)
        : 
        : "cc"
    );
    
    if (result16 != 0x4123) { // 01000001 00100011
        printf("Error: 16-bit ROR(4) expected 0x4123, got 0x%x\n", result16);
        errors++;
    } else {
        printf("16-bit ROR(4): PASS\n");
    }
    
    // Test 32-bit ROR with count=17
    val32 = 0x00010001;
    result32 = val32;
    __asm__ __volatile__ (
        "rorl $17, %[val]"
        : [val] "+r" (result32)
        : 
        : "cc"
    );

    if (result32 != 0x80008000) {
        printf("Error: 32-bit ROR(17) expected 0x80008000, got 0x%x\n", result32);
        errors++;
    } else {
        printf("32-bit ROR(17): PASS\n");
    }
    
    // Test 64-bit ROR with count=1
    val64 = 0x0000000000000001;
    result64 = val64;
    __asm__ __volatile__ (
        "rorq $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result64), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result64 != 0x8000000000000000) {
        printf("Error: 64-bit ROR(1) expected 0x8000000000000000, got 0x%lx\n", result64);
        errors++;
    } else if (!(flags & (1 << 0))) { // Check CF
        printf("Error: 64-bit ROR(1) expected CF=1\n");
        errors++;
    } else {
        printf("64-bit ROR(1): PASS - ");
        print_ror_flags(flags);
        printf("\n");
    }
    
    // Test memory operand (16-bit)
    uint16_t mem_val = 0x5555; // 0101010101010101
    uint16_t mem_result = mem_val;
    __asm__ __volatile__ (
        "rorw $1, %[mem]"
        : [mem] "+m" (mem_result)
        : 
        : "cc"
    );
    
    if (mem_result != 0xAAAA) { // 1010101010101010
        printf("Error: Memory ROR(1) expected 0xAAAA, got 0x%x\n", mem_result);
        errors++;
    } else {
        printf("Memory operand: PASS\n");
    }
    
    // Test using CL register (8-bit)
    val8 = 0x80;
    uint8_t count = 7;
    result8 = val8;
    __asm__ __volatile__ (
        "mov %[count], %%cl\n\t"
        "rorb %%cl, %[val]"
        : [val] "+r" (result8)
        : [count] "r" (count)
        : "cl", "cc"
    );
    
    if (result8 != 0x01) {
        printf("Error: CL ROR(7) expected 0x01, got 0x%x\n", result8);
        errors++;
    } else {
        printf("CL register: PASS\n");
    }
    
    // Test OF flag for single-bit rotate
    val8 = 0x80; // 10000000
    result8 = val8;
    __asm__ __volatile__ (
        "rorb $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    // OF = (result[7] XOR result[6]) for single-bit rotate
    if (!(flags & (1 << 11))) { // Check OF (0x80 -> 0x40, MSB changed)
        printf("Error: ROR(1) expected OF=1 (0x80 -> 0x40)\n");
        errors++;
    } else {
        printf("OF flag: PASS\n");
    }
    
    // Test count=0 (should be NOP, but sets flags)
    val8 = 0x40;
    uint8_t orig_val = val8;
    result8 = val8;
    __asm__ __volatile__ (
        "rorb $0, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result8 != orig_val) {
        printf("Error: ROR(0) modified value. Expected 0x%x, got 0x%x\n", orig_val, result8);
        errors++;
    } else {
        printf("ROR(0) no change: PASS\n");
    }
    
    // Test boundary case (all bits set)
    val32 = 0xFFFFFFFF;
    result32 = val32;
    __asm__ __volatile__ (
        "rorl $1, %[val]"
        : [val] "+r" (result32)
        : 
        : "cc"
    );
    
    if (result32 != 0xFFFFFFFF) { // 0xFFFFFFFF rotated right by 1 is same
        printf("Error: All bits set ROR expected 0xFFFFFFFF, got 0x%x\n", result32);
        errors++;
    } else {
        printf("All bits set: PASS\n");
    }
    
    // Test wrapping behavior
    val16 = 0x0001;
    result16 = val16;
    __asm__ __volatile__ (
        "rorw $1, %[val]"
        : [val] "+r" (result16)
        : 
        : "cc"
    );
    
    if (result16 != 0x8000) {
        printf("Error: Wrapping ROR expected 0x8000, got 0x%x\n", result16);
        errors++;
    } else {
        printf("Wrapping behavior: PASS\n");
    }
    
    printf("\nROR test completed\n");
    printf("==================\n");
    printf("Total tests: 10\n");
    printf("Passed: %d\n", 10 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
