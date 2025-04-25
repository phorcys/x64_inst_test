#include "x87.h"
#include <stdio.h>
#include <stdint.h>

// Test FISUBR with 16-bit integer
void test_fisubr_16bit() {
    struct {
        int16_t a;
        long double b;
        const char* desc;
    } tests[] = {
        {2, 4.0L, "Normal reverse subtraction"},
        {3, 1.5L, "Fraction reverse subtraction"}, 
        {0, 1.0L, "Reverse subtract zero"},
        {2, POS_INF, "Infinity reverse subtraction"},
        {0x7FFF, 2.0L, "Reverse subtract INT16_MAX"},
        {0x8000, 2.0L, "Reverse subtract INT16_MIN"},
        {2, POS_NAN, "NaN reverse subtraction"}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %d - %Lf\n", tests[i].a, tests[i].b);
        
        asm volatile (
            "fldt %1\n\t"       // Load b to st(0)
            "fisubrs %2\n\t"     // st(0) = mem16 - st(0)
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

// Test FISUBR with 32-bit integer
void test_fisubr_32bit() {
    struct {
        int32_t a;
        long double b;
        const char* desc;
    } tests[] = {
        {2, 4.0L, "Normal reverse subtraction"},
        {3, 1.5L, "Fraction reverse subtraction"},
        {0, 1.0L, "Reverse subtract zero"},
        {2, POS_INF, "Infinity reverse subtraction"},
        {0x7FFFFFFF, 2.0L, "Reverse subtract INT32_MAX"},
        {0x80000000, 2.0L, "Reverse subtract INT32_MIN"},
        {2, POS_NAN, "NaN reverse subtraction"}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %d - %Lf\n", tests[i].a, tests[i].b);
        
        asm volatile (
            "fldt %1\n\t"       // Load b to st(0)
            "fisubrl %2\n\t"    // st(0) = mem32 - st(0)
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

// Test FISUBR instruction
void test_fisubr() {
    printf("=== Testing FISUBR (16-bit) ===\n");
    test_fisubr_16bit();
    
    printf("\n=== Testing FISUBR (32-bit) ===\n");
    test_fisubr_32bit();
}

int main() {
    test_fisubr();
    return 0;
}
