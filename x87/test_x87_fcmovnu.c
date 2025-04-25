#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Test FCMOVNU instruction
void test_fcmovnu() {
    long double src = 0.0L, dst = 0.0L;
    
    printf("=== Testing FCMOVNU ===\n");
    
    // Test 1: PF=0 (should move)
    asm volatile (
        "fldz\n\t"          // st(0) = 0.0
        "fld1\n\t"          // st(0) = 1.0, st(1) = 0.0
        "fcomi %%st(1)\n\t" // Compare st(0) and st(1) (1.0 != 0.0, PF=0)
        "fcmovnu %%st(1), %%st(0)\n\t" // Should move st(1) to st(0)
        "fstpt %0\n\t"       // Store result (80-bit)
        "fstpt %1\n\t"       // Pop stack (80-bit)
        : "=m" (dst), "=m" (src)
        :
        : "cc"
    );
    
    printf("Test 1 (PF=0):\n");
    printf("Expected: 0.0, Actual: %.20Lf\n", dst);
    print_x87_status();
    
    // Test 2: PF=1 (should not move)
    asm volatile (
        "fldz\n\t"          // st(0) = 0.0
        "fldz\n\t"          // st(0) = 0.0, st(1) = 0.0
        "fcomi %%st(1)\n\t" // Compare st(0) and st(1) (0.0 == 0.0, PF=1)
        "fcmovnu %%st(1), %%st(0)\n\t" // Should NOT move
        "fstpt %0\n\t"       // Store result (80-bit)
        "fstpt %1\n\t"       // Pop stack (80-bit)
        : "=m" (dst), "=m" (src)
        :
        : "cc"
    );
    
    printf("\nTest 2 (PF=1):\n");
    printf("Expected: 0.0, Actual: %.20Lf\n", dst);
    print_x87_status();
    
    // Test 3: With various float values (same as FCMOVNE)
    long double values[] = {POS_ZERO, NEG_ZERO, POS_ONE, NEG_ONE, 
                           POS_INF, NEG_INF, POS_NAN, NEG_NAN,
                           POS_DENORM, NEG_DENORM, PI, E};
    int num_values = sizeof(values)/sizeof(values[0]);
    
    for (int i = 0; i < num_values; i++) {
        for (int j = 0; j < num_values; j++) {
            if (i == j) continue; // Skip equal values
            
            asm volatile (
                "fldt %2\n\t"      // Load src value
                "fldt %3\n\t"      // Load dst value
                "fcomi %%st(1)\n\t" // Compare (should set PF=0 since i != j)
                "fcmovnu %%st(1), %%st(0)\n\t"
                "fstpt %0\n\t"  // Store result (80-bit)
                "fstpt %1\n\t"  // Pop stack (80-bit)
                : "=m" (dst), "=m" (src)
                : "m" (values[i]), "m" (values[j])
                : "cc"
            );
            
            printf("\nTest 3.%d.%d (%Lf != %Lf):\n", i+1, j+1, values[i], values[j]);
            printf("Expected: %Lf, Actual: %.20Lf\n", values[i], dst);
            print_x87_status();
        }
    }
}

int main() {
    test_fcmovnu();
    return 0;
}
