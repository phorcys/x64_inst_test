#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("SBB instruction test\n");
    printf("====================\n");
    
    int errors = 0;
    uint8_t val8, src8, result8;
    uint16_t val16, src16, result16;
    uint32_t val32, src32, result32;
    uint64_t val64, src64;
    uint64_t flags;
    
    // Test 1: 8-bit SBB with carry flag set
    val8 = 0x40;
    src8 = 0x20;
    result8 = val8;
    __asm__ __volatile__ (
        "stc\n\t"                   // Set carry flag (CF=1)
        "sbbb %[src], %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : [src] "r" (src8)
        : "cc"
    );
    
    // 0x40 - 0x20 - 1 = 0x1F
    if (result8 != 0x1F) {
        printf("Error: 8-bit SBB with carry expected 0x1F, got 0x%x\n", result8);
        errors++;
    } else {
        printf("8-bit SBB with carry: PASS\n");
    }
    
    // Test 2: 16-bit SBB without carry
    val16 = 0x5000;
    src16 = 0x3000;
    result16 = val16;
    __asm__ __volatile__ (
        "clc\n\t"                   // Clear carry flag (CF=0)
        "sbbw %[src], %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result16), [flags] "=r" (flags)
        : [src] "r" (src16)
        : "cc"
    );
    
    // 0x5000 - 0x3000 - 0 = 0x2000
    if (result16 != 0x2000) {
        printf("Error: 16-bit SBB without carry expected 0x2000, got 0x%x\n", result16);
        errors++;
    } else {
        printf("16-bit SBB without carry: PASS\n");
    }
    
    // Test 3: 32-bit SBB with immediate
    val32 = 0x10000000;
    result32 = val32;
    __asm__ __volatile__ (
        "clc\n\t"                   // Clear carry flag
        "sbbl $0x0F00000F, %[val]"
        : [val] "+r" (result32)
        : 
        : "cc"
    );
    
    // 0x10000000 - 0x0F00000F - 0 = 0x01000000 - 0x0000000F = 0x00FFFFF1
    if (result32 != 0x00FFFFF1) {
        printf("Error: 32-bit SBB immediate expected 0x00FFFFF1, got 0x%x\n", result32);
        errors++;
    } else {
        printf("32-bit SBB immediate: PASS\n");
    }
    
    // Test 4: 64-bit SBB with carry and memory operand
    val64 = 0x8000000000000000;
    src64 = 0x7FFFFFFFFFFFFFFF;
    uint64_t mem_result = val64;
    __asm__ __volatile__ (
        "stc\n\t"                   // Set carry flag
        "sbbq %[src], %[mem]"
        : [mem] "+m" (mem_result)
        : [src] "r" (src64)
        : "cc"
    );
    
    // 0x8000000000000000 - 0x7FFFFFFFFFFFFFFF - 1 = 0
    if (mem_result != 0) {
        printf("Error: 64-bit SBB memory expected 0, got 0x%lx\n", mem_result);
        errors++;
    } else {
        printf("64-bit SBB memory: PASS\n");
    }
    
    // Test 5: Flags check (zero flag and carry flag)
    val8 = 0x01;
    src8 = 0x01;
    result8 = val8;
    __asm__ __volatile__ (
        "clc\n\t"                   // Clear carry flag
        "sbbb %[src], %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : [src] "r" (src8)
        : "cc"
    );
    
    // 0x01 - 0x01 - 0 = 0 -> ZF=1, CF=0
    if (result8 != 0) {
        printf("Error: SBB zero flag test value error\n");
        errors++;
    } else if (!(flags & (1 << 6))) { // ZF should be 1
        printf("Error: SBB zero flag not set\n");
        errors++;
    } else if (flags & (1 << 0)) { // CF should be 0
        printf("Error: SBB carry flag set incorrectly\n");
        errors++;
    } else {
        printf("ZF/CF flags: PASS\n");
    }
    
    // Test 6: Sign flag and overflow
    val32 = 0x80000000;
    src32 = 0x00000001;
    result32 = val32;
    __asm__ __volatile__ (
        "clc\n\t"                   // Clear carry flag
        "sbbl %[src], %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result32), [flags] "=r" (flags)
        : [src] "r" (src32)
        : "cc"
    );
    
    // 0x80000000 - 0x00000001 = 0x7FFFFFFF -> SF=0, OF=1 (signed overflow)
    if (result32 != 0x7FFFFFFF) {
        printf("Error: SBB overflow test value error\n");
        errors++;
    } else if (flags & (1 << 7)) { // SF should be 0
        printf("Error: SBB sign flag set incorrectly\n");
        errors++;
    } else if (!(flags & (1 << 11))) { // OF should be 1
        printf("Error: SBB overflow flag not set\n");
        errors++;
    } else {
        printf("SF/OF flags: PASS\n");
    }
    
    // Test 7: Adjust flag (AF) and parity flag (PF)
    val8 = 0x10;
    src8 = 0x01;
    result8 = val8;
    __asm__ __volatile__ (
        "clc\n\t"                   // Clear carry flag
        "sbbb %[src], %[val]\n\t"
        "pushf\n\t"
        "pop %[flags]"
        : [val] "+r" (result8), [flags] "=r" (flags)
        : [src] "r" (src8)
        : "cc"
    );
    
    // 0x10 - 0x01 = 0x0F -> AF=1 (bit 4 borrow), PF=1 (odd number of 1s: 00001111 -> 4 ones -> even -> PF=0)
    // Note: PF is set for low byte of result: 0x0F has 4 ones -> even parity -> PF=1? Actually 0x0F is 00001111 -> 4 ones -> even -> PF=1
    if (result8 != 0x0F) {
        printf("Error: SBB AF/PF test value error\n");
        errors++;
    } else if (!(flags & (1 << 4))) { // AF should be 1
        printf("Error: SBB adjust flag not set\n");
        errors++;
    } else if (!(flags & (1 << 2))) { // PF should be 1 (even parity)
        printf("Error: SBB parity flag not set\n");
        errors++;
    } else {
        printf("AF/PF flags: PASS\n");
    }
    
    printf("\nSBB test completed\n");
    printf("==================\n");
    printf("Total tests: 7\n");
    printf("Passed: %d\n", 7 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
