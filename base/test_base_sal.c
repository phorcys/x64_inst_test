#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Helper function to print flags affected by SAL
void print_sal_flags(uint64_t flags) {
    printf("CF: %lu, OF: %lu, ZF: %lu, SF: %lu", 
           (flags >> 0) & 1,  // CF
           (flags >> 11) & 1, // OF
           (flags >> 6) & 1,  // ZF
           (flags >> 7) & 1); // SF
}

int main() {
    printf("SAL instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    uint8_t val8, result8;
    uint16_t val16, result16;
    uint32_t val32, result32;
    uint64_t val64, result64;
    uint64_t flags;
    
    // Test 1: 8-bit SAL with count=1 (sign change)
    val8 = 0x42; // 01000010 -> becomes 10000100 (sign changed)
    result8 = val8;
    __asm__ __volatile__ (
        "salb $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result8 != 0x84) {
        printf("Error: 8-bit SAL(1) expected 0x84, got 0x%x\n", result8);
        errors++;
    } else if (!(flags & (1 << 11))) { // OF should be 1 (sign changed)
        printf("Error: 8-bit SAL(1) expected OF=1\n");
        errors++;
    } else {
        printf("8-bit SAL(1): PASS - ");
        print_sal_flags(flags);
        printf("\n");
    }
    
    // Test 2: 16-bit SAL with count=1 (sign change)
    val16 = 0x4000; // 01000000 00000000
    result16 = val16;
    __asm__ __volatile__ (
        "salw $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result16), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result16 != 0x8000) {
        printf("Error: 16-bit SAL(1) expected 0x8000, got 0x%x\n", result16);
        errors++;
    } else if (!(flags & (1 << 11))) { // OF should be 1 (sign changed)
        printf("Error: 16-bit SAL(1) expected OF=1\n");
        errors++;
    } else {
        printf("16-bit SAL(1): PASS - ");
        print_sal_flags(flags);
        printf("\n");
    }
    
    // Test 3: 32-bit SAL with count=3
    val32 = 0x11111111;
    result32 = val32;
    __asm__ __volatile__ (
        "sall $3, %[val]"
        : [val] "+r" (result32)
        : 
        : "cc"
    );
    
    if (result32 != 0x88888888) {
        printf("Error: 32-bit SAL(3) expected 0x88888888, got 0x%x\n", result32);
        errors++;
    } else {
        printf("32-bit SAL(3): PASS\n");
    }
    
    // Test 4: 64-bit SAL with count=63
    val64 = 0x1;
    result64 = val64;
    __asm__ __volatile__ (
        "salq $63, %[val]"
        : [val] "+r" (result64)
        : 
        : "cc"
    );
    
    if (result64 != 0x8000000000000000) {
        printf("Error: 64-bit SAL(63) expected 0x8000000000000000, got 0x%lx\n", result64);
        errors++;
    } else {
        printf("64-bit SAL(63): PASS\n");
    }
    
    // Test 5: Memory operand (16-bit)
    uint16_t mem_val = 0x0001;
    uint16_t mem_result = mem_val;
    __asm__ __volatile__ (
        "salw $15, %[mem]"
        : [mem] "+m" (mem_result)
        : 
        : "cc"
    );
    
    if (mem_result != 0x8000) {
        printf("Error: Memory SAL(15) expected 0x8000, got 0x%x\n", mem_result);
        errors++;
    } else {
        printf("Memory operand: PASS\n");
    }
    
    // Test 6: Using CL register (8-bit)
    val8 = 0x01;
    uint8_t count = 7;
    result8 = val8;
    __asm__ __volatile__ (
        "mov %[count], %%cl\n\t"
        "salb %%cl, %[val]"
        : [val] "+r" (result8)
        : [count] "r" (count)
        : "cl", "cc"
    );
    
    if (result8 != 0x80) {
        printf("Error: CL SAL(7) expected 0x80, got 0x%x\n", result8);
        errors++;
    } else {
        printf("CL register: PASS\n");
    }
    
    // Test 7: Carry flag behavior (8-bit)
    val8 = 0x80; // 10000000
    result8 = val8;
    __asm__ __volatile__ (
        "salb $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (!(flags & (1 << 0))) { // CF should be 1 (MSB was shifted out)
        printf("Error: SAL CF behavior failed. Expected CF=1\n");
        errors++;
    } else {
        printf("CF behavior: PASS\n");
    }
    
    // Test 8: Zero and sign flags
    val32 = 0x40000000;
    result32 = val32;
    __asm__ __volatile__ (
        "sall $1, %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result32), [flags] "=r" (flags)
        : 
        : "cc"
    );
    
    if (result32 != 0x80000000) {
        printf("Error: SAL sign flag test value error\n");
        errors++;
    } else if (!(flags & (1 << 7))) { // SF should be 1 (negative)
        printf("Error: SAL sign flag not set\n");
        errors++;
    } else if (flags & (1 << 6)) { // ZF should be 0
        printf("Error: SAL zero flag set incorrectly\n");
        errors++;
    } else {
        printf("ZF/SF behavior: PASS\n");
    }
    
    // Test 9: Shift count=0 (should be NOP)
    val8 = 0x55;
    uint8_t orig_val = val8;
    result8 = val8;
    __asm__ __volatile__ (
        "salb $0, %[val]"
        : [val] "+r" (result8)
        : 
        : "cc"
    );
    
    if (result8 != orig_val) {
        printf("Error: SAL(0) modified value. Expected 0x%x, got 0x%x\n", orig_val, result8);
        errors++;
    } else {
        printf("SAL(0) no change: PASS\n");
    }
    
    // Test 10: Shift count greater than width
    val16 = 0x0001;
    result16 = val16;
    __asm__ __volatile__ (
        "salw $17, %[val]"
        : [val] "+r" (result16)
        : 
        : "cc"
    );
    
    // 17 mod 32 = 17 shifts for 16-bit value, which clears all bits
    if (result16 != 0x0000) {
        printf("Error: SAL(17) expected 0x0000, got 0x%x\n", result16);
        errors++;
    } else {
        printf("Shift count modulo: PASS\n");
    }
    
    printf("\nSAL test completed\n");
    printf("==================\n");
    printf("Total tests: 10\n");
    printf("Passed: %d\n", 10 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
