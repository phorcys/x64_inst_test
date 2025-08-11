#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Helper function to print flags affected by SAR
void print_sar_flags(uint64_t flags) {
    printf("CF: %lu, OF: %lu, ZF: %lu, SF: %lu", 
           (flags >> 0) & 1,  // CF
           (flags >> 11) & 1, // OF
           (flags >> 6) & 1,  // ZF
           (flags >> 7) & 1); // SF
}

int main() {
    printf("SAR instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    uint8_t val8, result8;
    uint16_t val16, result16;
    uint32_t val32, result32;
    uint64_t val64, result64;
    uint64_t flags;
    
    // Test 1: 8-bit SAR with count=1 (negative number)
    val8 = 0x85; // 10000101 (-123)
    result8 = val8;
    __asm__ __volatile__ (
        "sarb $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result8 != 0xC2) {
        printf("Error: 8-bit SAR(1) expected 0xC2, got 0x%x\n", result8);
        errors++;
    } else if (flags & (1 << 11)) { // OF should be 0
        printf("Error: 8-bit SAR(1) expected OF=0\n");
        errors++;
    } else {
        printf("8-bit SAR(1): PASS - ");
        print_sar_flags(flags);
        printf("\n");
    }
    
    // Test 2: 16-bit SAR with count=1 (positive number)
    val16 = 0x4000; // 01000000 00000000
    result16 = val16;
    __asm__ __volatile__ (
        "sarw $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result16), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result16 != 0x2000) {
        printf("Error: 16-bit SAR(1) expected 0x2000, got 0x%x\n", result16);
        errors++;
    } else if (flags & (1 << 11)) { // OF should be 0
        printf("Error: 16-bit SAR(1) expected OF=0\n");
        errors++;
    } else {
        printf("16-bit SAR(1): PASS - ");
        print_sar_flags(flags);
        printf("\n");
    }
    
    // Test 3: 32-bit SAR with count=3
    val32 = 0x88888888; // Negative number
    result32 = val32;
    __asm__ __volatile__ (
        "sarl $3, %[val]"
        : [val] "+r" (result32)
        : 
        : "cc"
    );
    
    if (result32 != 0xF1111111) {
        printf("Error: 32-bit SAR(3) expected 0xF1111111, got 0x%x\n", result32);
        errors++;
    } else {
        printf("32-bit SAR(3): PASS\n");
    }
    
    // Test 4: 64-bit SAR with count=63
    val64 = 0x8000000000000000; // Negative number
    result64 = val64;
    __asm__ __volatile__ (
        "sarq $63, %[val]"
        : [val] "+r" (result64)
        : 
        : "cc"
    );
    
    if (result64 != 0xFFFFFFFFFFFFFFFF) {
        printf("Error: 64-bit SAR(63) expected 0xFFFFFFFFFFFFFFFF, got 0x%lx\n", result64);
        errors++;
    } else {
        printf("64-bit SAR(63): PASS\n");
    }
    
    // Test 5: Memory operand (16-bit)
    uint16_t mem_val = 0x8000; // Negative number
    uint16_t mem_result = mem_val;
    __asm__ __volatile__ (
        "sarw $4, %[mem]"
        : [mem] "+m" (mem_result)
        : 
        : "cc"
    );
    
    if (mem_result != 0xF800) {
        printf("Error: Memory SAR(4) expected 0xF800, got 0x%x\n", mem_result);
        errors++;
    } else {
        printf("Memory operand: PASS\n");
    }
    
    // Test 6: Using CL register (8-bit)
    val8 = 0x80; // Negative number
    uint8_t count = 3;
    result8 = val8;
    __asm__ __volatile__ (
        "mov %[count], %%cl\n\t"
        "sarb %%cl, %[val]"
        : [val] "+r" (result8)
        : [count] "r" (count)
        : "cl", "cc"
    );
    
    if (result8 != 0xF0) {
        printf("Error: CL SAR(3) expected 0xF0, got 0x%x\n", result8);
        errors++;
    } else {
        printf("CL register: PASS\n");
    }
    
    // Test 7: Carry flag behavior (8-bit)
    val8 = 0x81; // 10000001 (-127)
    result8 = val8;
    __asm__ __volatile__ (
        "sarb $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (!(flags & (1 << 0))) { // CF should be 1 (LSB was shifted out)
        printf("Error: SAR CF behavior failed. Expected CF=1\n");
        errors++;
    } else {
        printf("CF behavior: PASS\n");
    }
    
    // Test 8: Zero and sign flags
    val32 = 0x80000000; // Negative number
    result32 = val32;
    __asm__ __volatile__ (
        "sarl $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result32), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result32 != 0xC0000000) {
        printf("Error: SAR sign flag test value error\n");
        errors++;
    } else if (!(flags & (1 << 7))) { // SF should be 1 (negative)
        printf("Error: SAR sign flag not set\n");
        errors++;
    } else if (flags & (1 << 6)) { // ZF should be 0
        printf("Error: SAR zero flag set incorrectly\n");
        errors++;
    } else {
        printf("ZF/SF behavior: PASS\n");
    }
    
    // Test 9: Shift count=0 (should be NOP)
    val8 = 0x55;
    uint8_t orig_val = val8;
    result8 = val8;
    __asm__ __volatile__ (
        "sarb $0, %[val]"
        : [val] "+r" (result8)
        : 
        : "cc"
    );
    
    if (result8 != orig_val) {
        printf("Error: SAR(0) modified value. Expected 0x%x, got 0x%x\n", orig_val, result8);
        errors++;
    } else {
        printf("SAR(0) no change: PASS\n");
    }
    
    // Test 10: Shift count greater than width
    val16 = 0x8000; // Negative number
    result16 = val16;
    __asm__ __volatile__ (
        "sarw $17, %[val]"
        : [val] "+r" (result16)
        : 
        : "cc"
    );
    
    // 17 mod 16 = 1 shift, so should be all 1s (0xFFFF)
    if (result16 != 0xFFFF) {
        printf("Error: SAR(17) expected 0xFFFF, got 0x%x\n", result16);
        errors++;
    } else {
        printf("Shift count modulo: PASS\n");
    }
    
    printf("\nSAR test completed\n");
    printf("==================\n");
    printf("Total tests: 10\n");
    printf("Passed: %d\n", 10 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
