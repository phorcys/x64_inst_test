#include "x87.h"
#include <stdio.h>
#include <stdint.h>

// Test FISUB with 16-bit integer
void test_fisub_16bit() {
    struct {
        int16_t a;
        long double b;
        const char* desc;
    } tests[] = {
        {2, 4.0L, "Normal subtraction"},
        {3, 1.5L, "Fraction subtraction"}, 
        {0, 1.0L, "Subtract zero"},
        {2, POS_INF, "Infinity subtraction"},
        {0x7FFF, 2.0L, "Subtract INT16_MAX"},
        {0x8000, 2.0L, "Subtract INT16_MIN"},
        {2, POS_NAN, "NaN subtraction"}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %Lf - %d\n", tests[i].b, tests[i].a);
        
        asm volatile (
            "fldt %1\n\t"       // Load b to st(0)
            "fisubs %2\n\t"      // st(0) = st(0) - mem16
            "fstpt %0\n\t"       // Store result
            : "=m" (result)
            : "m" (tests[i].b), "m" (tests[i].a)
            : "st"
        );

        printf("Result: ");
        print_float80(result);
        print_x87_status();
    }
}

// Test FISUB with 32-bit integer
void test_fisub_32bit() {
    struct {
        int32_t a;
        long double b;
        const char* desc;
    } tests[] = {
        {2, 4.0L, "Normal subtraction"},
        {3, 1.5L, "Fraction subtraction"},
        {0, 1.0L, "Subtract zero"},
        {2, POS_INF, "Infinity subtraction"},
        {0x7FFFFFFF, 2.0L, "Subtract INT32_MAX"},
        {0x80000000, 2.0L, "Subtract INT32_MIN"},
        {2, POS_NAN, "NaN subtraction"}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %Lf - %d\n", tests[i].b, tests[i].a);
        
        asm volatile (
            "fldt %1\n\t"       // Load b to st(0)
            "fisubl %2\n\t"      // st(0) = st(0) - mem32
            "fstpt %0\n\t"       // Store result
            : "=m" (result)
            : "m" (tests[i].b), "m" (tests[i].a)
            : "st"
        );

        printf("Result: ");
        print_float80(result);
        print_x87_status();
    }
}

// Test FISUB instruction
void test_fisub() {
    printf("=== Testing FISUB (16-bit) ===\n");
    test_fisub_16bit();
    
    printf("\n=== Testing FISUB (32-bit) ===\n");
    test_fisub_32bit();
}

int main() {
    test_fisub();
    return 0;
}
