#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Helper function to print flags affected by RCL
void print_rcl_flags(uint64_t flags) {
    printf("CF: %lu, OF: %lu", 
           (flags >> 0) & 1,  // CF is bit 0
           (flags >> 11) & 1); // OF is bit 11
}

int main() {
    printf("RCL instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    uint8_t val8, result8;
    uint16_t val16, result16;
    uint32_t val32, result32;
    uint64_t val64, result64;
    uint64_t flags;
    
    // Test 8-bit RCL with count=1
    val8 = 0x81; // 10000001
    result8 = val8;
    __asm__ __volatile__ (
        "stc\n\t"          // Set carry flag (CF=1)
        "rclb $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result8 != 0x03) { // Expected: 00000011 (CF rotated in as LSB)
        printf("Error: 8-bit RCL(1) expected 0x03, got 0x%x\n", result8);
        errors++;
    } else if (!(flags & (1 << 0))) { // Check CF
        printf("Error: 8-bit RCL(1) expected CF=1\n");
        errors++;
    } else {
        printf("8-bit RCL(1): PASS - ");
        print_rcl_flags(flags);
        printf("\n");
    }
    
    // Test 16-bit RCL with count=2
    val16 = 0x4000; // 0100000000000000
    result16 = val16;
    __asm__ __volatile__ (
        "clc\n\t"          // Clear carry flag (CF=0)
        "rclw $2, %[val]"
        : [val] "+r" (result16)
        : 
        : "cc"
    );
    
    if (result16 != 0x0000) { // Expected: 0000000000000000
        printf("Error: 16-bit RCL(2) expected 0x0000, got 0x%x\n", result16);
        errors++;
    } else {
        printf("16-bit RCL(2): PASS\n");
    }
    
    // Test 32-bit RCL with count=33 (mod 32+1=33 mod 32 = 1)
    val32 = 0x80000000; // 10000000000000000000000000000000
    result32 = val32;
    __asm__ __volatile__ (
        "stc\n\t"          // Set carry flag (CF=1)
        "rcll $33, %[val]"
        : [val] "+r" (result32)
        : 
        : "cc"
    );
    
    if (result32 != 0x00000001) { // Expected: 00000000000000000000000000000001
        printf("Error: 32-bit RCL(33) expected 0x00000001, got 0x%x\n", result32);
        errors++;
    } else {
        printf("32-bit RCL(33): PASS\n");
    }
    
    // Test 64-bit RCL with count=1 and CF=0
    val64 = 0x8000000000000000;
    result64 = val64;
    __asm__ __volatile__ (
        "clc\n\t"
        "rclq $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result64), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result64 != 0x0000000000000000) {
        printf("Error: 64-bit RCL(1) expected 0x0, got 0x%lx\n", result64);
        errors++;
    } else if (!(flags & (1 << 0)) || !(flags & (1 << 11))) { // Check CF and OF
        printf("Error: 64-bit RCL(1) expected CF=1 and OF=1\n");
        errors++;
    } else {
        printf("64-bit RCL(1): PASS - ");
        print_rcl_flags(flags);
        printf("\n");
    }
    
    // Test memory operand (16-bit)
    uint16_t mem_val = 0xAAAA;
    uint16_t mem_result = mem_val; // Initialize with the value
    __asm__ __volatile__ (
        "stc\n\t"
        "rclw $1, %[mem]"
        : [mem] "+m" (mem_result)
        : 
        : "cc"
    );
    
    if (mem_result != 0x5555) { // 1010101010101010 -> 0101010101010101 + CF
        printf("Error: Memory RCL(1) expected 0x5555, got 0x%x\n", mem_result);
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
        "rclb $0, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result8 != orig_val) {
        printf("Error: RCL(0) modified value. Expected 0x%x, got 0x%x\n", orig_val, result8);
        errors++;
    } else {
        printf("RCL(0) no change: PASS\n");
    }
    
    // Test rotating through carry multiple times
    val16 = 0x0001;
    result16 = val16;
    __asm__ __volatile__ (
        "clc\n\t"          // CF=0
        "rclw $3, %[val]"  // Rotate 3 times through carry
        : [val] "+r" (result16)
        : 
        : "cc"
    );
    
    if (result16 != 0x0008) { // 0000000000000001 -> 0000000000001000
        printf("Error: Multi-rotate RCL expected 0x0008, got 0x%x\n", result16);
        errors++;
    } else {
        printf("Multi-rotate: PASS\n");
    }
    
    // Test boundary case (all bits set)
    val32 = 0xFFFFFFFF;
    result32 = val32;
    __asm__ __volatile__ (
        "stc\n\t"
        "rcll $1, %[val]"
        : [val] "+r" (result32)
        : 
        : "cc"
    );
    
    if (result32 != 0xFFFFFFFF) { // 0xFFFFFFFF rotated left with CF=1 -> same value
        printf("Error: All bits set RCL expected 0xFFFFFFFF, got 0x%x\n", result32);
        errors++;
    } else {
        printf("All bits set: PASS\n");
    }
    
    printf("\nRCL test completed\n");
    printf("==================\n");
    printf("Total tests: 8\n");
    printf("Passed: %d\n", 8 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
