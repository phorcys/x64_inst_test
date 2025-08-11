#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Helper function to print flags affected by RCR
void print_rcr_flags(uint64_t flags) {
    printf("CF: %lu, OF: %lu", 
           (flags >> 0) & 1,  // CF is bit 0
           (flags >> 11) & 1); // OF is bit 11
}

int main() {
    printf("RCR instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    uint8_t val8, result8;
    uint16_t val16, result16;
    uint32_t val32, result32;
    uint64_t val64, result64;
    uint64_t flags;
    
    // Test 8-bit RCR with count=1 and CF=1
    val8 = 0x01; // 00000001
    result8 = val8;
    __asm__ __volatile__ (
        "stc\n\t"          // Set carry flag (CF=1)
        "rcrb $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result8 != 0x80) { // Expected: 10000000 (CF rotated in as MSB)
        printf("Error: 8-bit RCR(1) expected 0x80, got 0x%x\n", result8);
        errors++;
    } else if (!(flags & (1 << 0))) { // Check CF
        printf("Error: 8-bit RCR(1) expected CF=1\n");
        errors++;
    } else {
        printf("8-bit RCR(1): PASS - ");
        print_rcr_flags(flags);
        printf("\n");
    }
    
    // Test 16-bit RCR with count=2 and CF=0
    val16 = 0x0001; // 0000000000000001
    result16 = val16;
    __asm__ __volatile__ (
        "clc\n\t"          // Clear carry flag (CF=0)
        "rcrw $2, %[val]"
        : [val] "+r" (result16)
        : 
        : "cc"
    );
    
    if (result16 != 0x8000) { // Expected: 1000000000000000
        printf("Error: 16-bit RCR(2) expected 0x8000, got 0x%x\n", result16);
        errors++;
    } else {
        printf("16-bit RCR(2): PASS\n");
    }
    
    // Test 32-bit RCR with count=33 (mod 32+1=33 mod 32 = 1)
    val32 = 0x00000001; 
    result32 = val32;
    __asm__ __volatile__ (
        "stc\n\t"          // Set carry flag (CF=1)
        "rcrl $33, %[val]"
        : [val] "+r" (result32)
        : 
        : "cc"
    );
    
    if (result32 != 0x80000000) { // Expected: 10000000000000000000000000000000
        printf("Error: 32-bit RCR(33) expected 0x80000000, got 0x%x\n", result32);
        errors++;
    } else {
        printf("32-bit RCR(33): PASS\n");
    }
    
    // Test 64-bit RCR with count=1 and CF=0
    val64 = 0x0000000000000001;
    result64 = val64;
    __asm__ __volatile__ (
        "clc\n\t"
        "rcrq $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result64), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result64 != 0x0000000000000000) {
        printf("Error: 64-bit RCR(1) expected 0x0, got 0x%lx\n", result64);
        errors++;
    } else if (!(flags & (1 << 0))) { // Check CF
        printf("Error: 64-bit RCR(1) expected CF=1\n");
        errors++;
    } else {
        printf("64-bit RCR(1): PASS - ");
        print_rcr_flags(flags);
        printf("\n");
    }
    
    // Test memory operand (16-bit)
    uint16_t mem_val = 0x5555; // 0101010101010101
    uint16_t mem_result = mem_val;
    __asm__ __volatile__ (
        "stc\n\t"
        "rcrw $1, %[mem]"
        : [mem] "+m" (mem_result)
        : 
        : "cc"
    );
    
    if (mem_result != 0xAAAA) { // 0101010101010101 -> 1010101010101010 + CF
        printf("Error: Memory RCR(1) expected 0xAAAA, got 0x%x\n", mem_result);
        errors++;
    } else {
        printf("Memory operand: PASS\n");
    }
    
    // Test count=0 (should be NOP, but sets flags)
    val8 = 0x80;
    uint8_t orig_val = val8;
    result8 = val8;
    __asm__ __volatile__ (
        "clc\n\t"
        "rcrb $0, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result8 != orig_val) {
        printf("Error: RCR(0) modified value. Expected 0x%x, got 0x%x\n", orig_val, result8);
        errors++;
    } else {
        printf("RCR(0) no change: PASS\n");
    }
    
    // Test rotating through carry multiple times
    val16 = 0x8000; // 1000000000000000
    result16 = val16;
    __asm__ __volatile__ (
        "clc\n\t"          // CF=0
        "rcrw $3, %[val]"  // Rotate 3 times through carry
        : [val] "+r" (result16)
        : 
        : "cc"
    );
    
    if (result16 != 0x1000) { // 1000000000000000 -> 0001000000000000
        printf("Error: Multi-rotate RCR expected 0x1000, got 0x%x\n", result16);
        errors++;
    } else {
        printf("Multi-rotate: PASS\n");
    }
    
    // Test boundary case (all bits set)
    val32 = 0xFFFFFFFF;
    result32 = val32;
    __asm__ __volatile__ (
        "stc\n\t"
        "rcrl $1, %[val]"
        : [val] "+r" (result32)
        : 
        : "cc"
    );
    
    if (result32 != 0xFFFFFFFF) { // 0xFFFFFFFF rotated right with CF=1 -> same value
        printf("Error: All bits set RCR expected 0xFFFFFFFF, got 0x%x\n", result32);
        errors++;
    } else {
        printf("All bits set: PASS\n");
    }
    
    printf("\nRCR test completed\n");
    printf("==================\n");
    printf("Total tests: 8\n");
    printf("Passed: %d\n", 8 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
