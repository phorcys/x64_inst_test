#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("PUSHF instruction test\n");
    printf("======================\n");
    
    int errors = 0;
    uint64_t flags_value, pushed_flags;
    
    // Test basic PUSHF functionality
    __asm__ __volatile__ (
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (flags_value)
    );
    
    // Bit 1 is reserved and should always be set (1)
    if ((flags_value & (1 << 1)) == 0) {
        printf("Error: Reserved bit 1 not set. Got flags: 0x%lx\n", flags_value);
        errors++;
    } else {
        printf("Reserved bit set: PASS\n");
    }
    
    // Test with CF set
    __asm__ __volatile__ (
        "stc\n\t"             // Set carry flag
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (pushed_flags)
    );
    
    if ((pushed_flags & (1 << 0)) == 0) { // Check CF bit (0)
        printf("Error: CF not set. Expected CF=1, got flags: 0x%lx\n", pushed_flags);
        errors++;
    } else {
        printf("CF set: PASS\n");
    }
    
    // Test with ZF set
    uint64_t zero = 0;
    __asm__ __volatile__ (
        "xor %%rax, %%rax\n\t"  // Set ZF by zeroing RAX
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (pushed_flags)
        : 
        : "rax"
    );
    
    if ((pushed_flags & (1 << 6)) == 0) { // Check ZF bit (6)
        printf("Error: ZF not set. Expected ZF=1, got flags: 0x%lx\n", pushed_flags);
        errors++;
    } else {
        printf("ZF set: PASS\n");
    }
    
    // Test with SF set
    __asm__ __volatile__ (
        "mov $-1, %%rax\n\t"  // Set negative value
        "test %%rax, %%rax\n\t"  // Actually set flags
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (pushed_flags)
        : 
        : "rax"
    );
    
    if ((pushed_flags & (1 << 7)) == 0) { // Check SF bit (7)
        printf("Error: SF not set. Expected SF=1, got flags: 0x%lx\n", pushed_flags);
        errors++;
    } else {
        printf("SF set: PASS\n");
    }
    
    // Test with DF set
    __asm__ __volatile__ (
        "std\n\t"             // Set direction flag
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (pushed_flags)
    );
    
    if ((pushed_flags & (1 << 10)) == 0) { // Check DF bit (10)
        printf("Error: DF not set. Expected DF=1, got flags: 0x%lx\n", pushed_flags);
        errors++;
    } else {
        printf("DF set: PASS\n");
    }
    
    // Test with OF set
    __asm__ __volatile__ (
        "mov $0x7FFFFFFFFFFFFFFF, %%rax\n\t"
        "add $1, %%rax\n\t"   // Cause signed overflow
        "pushf\n\t"
        "pop %[flags]"
        : [flags] "=r" (pushed_flags)
        : 
        : "rax"
    );
    
    if ((pushed_flags & (1 << 11)) == 0) { // Check OF bit (11)
        printf("Error: OF not set. Expected OF=1, got flags: 0x%lx\n", pushed_flags);
        errors++;
    } else {
        printf("OF set: PASS\n");
    }
    
    // Test multiple pushes
    uint64_t rsp_before, rsp_after;
    __asm__ __volatile__ (
        "mov %%rsp, %[before]\n\t"
        "pushf\n\t"
        "pushf\n\t"
        "pushf\n\t"
        "mov %%rsp, %[after]"
        : [before] "=r" (rsp_before), [after] "=r" (rsp_after)
        : 
        : "memory"
    );
    
    if (rsp_before - rsp_after != 24) {
        printf("Error: Multiple PUSHF expected 24 bytes stack change, got %ld\n", rsp_before - rsp_after);
        errors++;
    } else {
        printf("Multiple pushes: PASS\n");
    }
    __asm__ __volatile__ ("add $24, %rsp"); // Clean up stack
    
    // Test stack alignment
    __asm__ __volatile__ (
        "mov %%rsp, %[before]\n\t"
        "pushf\n\t"
        "mov %%rsp, %[after]"
        : [before] "=r" (rsp_before), [after] "=r" (rsp_after)
    );
    
    // Check alignment (should be 16-byte aligned)
    if ((rsp_after % 16) != 0) {
        printf("Error: Stack not 16-byte aligned after PUSHF. \n");
        errors++;
    } else {
        printf("Stack alignment: PASS\n");
    }
    __asm__ __volatile__ ("add $8, %rsp"); // Clean up stack
    
    printf("\nPUSHF test completed\n");
    printf("===================\n");
    printf("Total tests: 8\n");
    printf("Passed: %d\n", 8 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
