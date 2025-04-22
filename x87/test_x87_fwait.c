#include <stdio.h>
#include <stdint.h>
#include <fenv.h>

// Test FWAIT instruction by checking FPU control word synchronization
void test_fwait() {
    uint16_t cw1, cw2;
    uint16_t status1, status2;
    
    printf("Testing FWAIT instruction\n");
    
    // Test 1: Basic synchronization test
    printf("\nTest 1: Basic synchronization\n");
    asm volatile ("fninit");
    
    uint16_t control_word = 0x037F;
    
    // Test without FWAIT
    asm volatile (
        "fldcw %1\n\t"
        "fnstsw %0"
        : "=m" (cw1)
        : "m" (control_word)
        : "memory"
    );
    
    // Test with FWAIT
    asm volatile (
        "fldcw %1\n\t"
        "fwait\n\t"
        "fnstsw %0"
        : "=m" (cw2)
        : "m" (control_word)
        : "memory"
    );
    
    printf("Control word without FWAIT: 0x%04x\n", cw1);
    printf("Control word with FWAIT: 0x%04x\n", cw2);
    
    if (cw1 == cw2) {
        printf("Test 1 PASSED - Control words match\n");
    } else {
        printf("Test 1 FAILED - Control words differ\n");
    }

    // Test 2: Exception state synchronization
    printf("\nTest 2: Exception state\n");
    asm volatile ("fninit");
    
    // Generate divide by zero exception
    float zero = 0.0f;
    float one = 1.0f;
    asm volatile (
        "flds %1\n\t"
        "fdivs %0\n\t"
        "fwait\n\t"
        "fnstsw %2"
        : 
        : "m" (zero), "m" (one), "m" (status1)
        : "memory"
    );
    
    // Without FWAIT
    asm volatile (
        "flds %1\n\t"
        "fdivs %0\n\t"
        "fnstsw %2"
        : 
        : "m" (zero), "m" (one), "m" (status2)
        : "memory"
    );
    
    printf("Status with FWAIT: 0x%04x\n", status1);
    printf("Status without FWAIT: 0x%04x\n", status2);
    
    if ((status1 & 0x3F) == (status2 & 0x3F)) {
        printf("Test 2 PASSED - Exception states match\n");
    } else {
        printf("Test 2 FAILED - Exception states differ\n");
    }

    // Test 3: Multiple FWAIT instructions
    printf("\nTest 3: Multiple FWAITs\n");
    asm volatile ("fninit");
    uint16_t cw3, cw4;
    
    asm volatile (
        "fldcw %1\n\t"
        "fwait\n\t"
        "fwait\n\t"
        "fnstsw %0"
        : "=m" (cw3)
        : "m" (control_word)
        : "memory"
    );
    
    asm volatile (
        "fldcw %1\n\t"
        "fnstsw %0"
        : "=m" (cw4)
        : "m" (control_word)
        : "memory"
    );
    
    printf("Control word with double FWAIT: 0x%04x\n", cw3);
    printf("Control word without FWAIT: 0x%04x\n", cw4);
    
    if (cw3 == cw4) {
        printf("Test 3 PASSED - Multiple FWAITs work correctly\n");
    } else {
        printf("Test 3 FAILED - Multiple FWAITs show inconsistency\n");
    }
}

int main() {
    test_fwait();
    return 0;
}
