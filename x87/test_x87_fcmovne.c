#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Test FCMOVNE instruction
void test_fcmovne() {
    long double src = 0.0L, dst = 0.0L;
    
    printf("=== Testing FCMOVNE ===\n");
    
    // Test 1: ZF=0 (should move)
    asm volatile (
        "fldz\n\t"          // st(0) = 0.0
        "fld1\n\t"          // st(0) = 1.0, st(1) = 0.0
        "fcomi %%st(1)\n\t" // Compare st(0) and st(1) (1.0 != 0.0, ZF=0)
        "fcmovne %%st(1), %%st(0)\n\t" // Should move st(1) to st(0)
        "fstpt %0\n\t"       // Store result (80-bit)
        "fstpt %1\n\t"       // Pop stack (80-bit)
        : "=m" (dst), "=m" (src)
        :
        : "cc"
    );
    
    printf("Test 1 (ZF=0):\n");
    printf("Expected: 0.0, Actual: %.20Lf\n", dst);
    print_x87_status();
    
    // Test 2: ZF=1 (should not move)
    asm volatile (
        "fldz\n\t"          // st(0) = 0.0
        "fldz\n\t"          // st(0) = 0.0, st(1) = 0.0
        "fcomi %%st(1)\n\t" // Compare st(0) and st(1) (0.0 == 0.0, ZF=1)
        "fcmovne %%st(1), %%st(0)\n\t" // Should NOT move
        "fstpt %0\n\t"       // Store result (80-bit)
        "fstpt %1\n\t"       // Pop stack (80-bit)
        : "=m" (dst), "=m" (src)
        :
        : "cc"
    );
    
    printf("\nTest 2 (ZF=1):\n");
    printf("Expected: 0.0, Actual: %.20Lf\n", dst);
    print_x87_status();
    
        // Test 3: With various float values (excluding NaN cases which are handled separately)
        long double values[] = {POS_ZERO, NEG_ZERO, POS_ONE, NEG_ONE, 
                               POS_INF, NEG_INF, POS_DENORM, NEG_DENORM, PI, E};
        int num_values = sizeof(values)/sizeof(values[0]);

        // Special NaN tests
        printf("\n=== Special NaN Tests ===\n");
        long double nan_values[] = {POS_NAN, NEG_NAN};
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < num_values; j++) {
                asm volatile (
                    "fldt %2\n\t"      // Load NaN value
                    "fldt %3\n\t"      // Load comparison value
                    "fcomi %%st(1)\n\t" // Compare (should set ZF=0 for NaN)
                    "fcmovne %%st(1), %%st(0)\n\t"
                    "fstpt %0\n\t"  // Store result (80-bit)
                    "fstpt %1\n\t"  // Pop stack (80-bit)
                    : "=m" (dst), "=m" (src)
                    : "m" (nan_values[i]), "m" (values[j])
                    : "cc"
                );
                
                printf("\nNaN Test %d.%d (NaN vs %Lf):\n", i+1, j+1, values[j]);
                printf("Result: %.20Lf\n", dst);
                print_x87_status();
            }
        }
    
    for (int i = 0; i < num_values; i++) {
        for (int j = 0; j < num_values; j++) {
            if (i == j) continue; // Skip equal values
            
            asm volatile (
                "fldt %2\n\t"      // Load src value
                "fldt %3\n\t"      // Load dst value
                "fcomi %%st(1)\n\t" // Compare (should set ZF=0 since i != j)
                "fcmovne %%st(1), %%st(0)\n\t"
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
    test_fcmovne();
    return 0;
}
