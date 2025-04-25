#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Test FIDIV with 16-bit integer
void test_fidiv_16bit() {
    struct {
        long double a;
        int16_t b;
        const char* desc;
    } tests[] = {
        {4.0L, 2, "Normal division"},
        {1.0L, 3, "Fraction division"},
        {1.0L, 0, "Divide by zero"},
        {POS_INF, 2, "Infinity divided"},
        {2.0L, 0x7FFF, "Divide by INT16_MAX"},
        {2.0L, 0x8000, "Divide by INT16_MIN"},
        {POS_NAN, 2, "NaN divided"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %Lf / %d\n", tests[i].a, tests[i].b);
        
        asm volatile (
            "filds %2\n\t"      // Load 16-bit integer b
            "fldt %1\n\t"       // Load a (dividend)
            "fidiv %2\n\t"      // st(0) = st(0)/mem16
            "fstpt %0\n\t"      // Store result
            : "=m" (result)
            : "m" (tests[i].a), "m" (tests[i].b)
            : "st", "st(1)"
        );
        
        printf("Result: ");
        print_float80(result);
        print_x87_status();
    }
}

// Test FIDIV with 32-bit integer
void test_fidiv_32bit() {
    struct {
        long double a;
        int32_t b;
        const char* desc;
    } tests[] = {
        {4.0L, 2, "Normal division"},
        {1.0L, 3, "Fraction division"},
        {1.0L, 0, "Divide by zero"},
        {POS_INF, 2, "Infinity divided"},
        {2.0L, 0x7FFFFFFF, "Divide by INT32_MAX"},
        {2.0L, 0x80000000, "Divide by INT32_MIN"},
        {POS_NAN, 2, "NaN divided"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %Lf / %d\n", tests[i].a, tests[i].b);
        
        asm volatile (
            "fildl %2\n\t"      // Load 32-bit integer b
            "fldt %1\n\t"       // Load a (dividend)
            "fidiv %2\n\t"      // st(0) = st(0)/mem32
            "fstpt %0\n\t"      // Store result
            : "=m" (result)
            : "m" (tests[i].a), "m" (tests[i].b)
            : "st", "st(1)"
        );
        
        printf("Result: ");
        print_float80(result);
        print_x87_status();
    }
}

// Test FIDIV instruction
void test_fidiv() {
    printf("=== Testing FIDIV (16-bit) ===\n");
    test_fidiv_16bit();
    
    printf("\n=== Testing FIDIV (32-bit) ===\n");
    test_fidiv_32bit();
}

int main() {
    test_fidiv();
    return 0;
}
