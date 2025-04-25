#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Test FIDIVR with 16-bit integer
void test_fidivr_16bit() {
    struct {
        int16_t a;
        long double b;
        const char* desc;
    } tests[] = {
        {2, 4.0L, "Normal division"},
        {3, 1.0L, "Fraction division"},
        {0, 1.0L, "Divide by zero"},
        {2, POS_INF, "Infinity divided"},
        {0x7FFF, 2.0L, "Divide by INT16_MAX"},
        {0x8000, 2.0L, "Divide by INT16_MIN"},
        {2, POS_NAN, "NaN divided"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %d / %Lf\n", tests[i].a, tests[i].b);
        
        asm volatile (
            "filds %2\n\t"      // Load integer to st(0)
            "fldt %1\n\t"       // Load float to st(0), push integer to st(1)
            "fidivr %2\n\t"     // st(0) = mem16/st(0) = a/b
            "fstpt %0\n\t"      // Store result
            : "=m" (result)
            : "m" (tests[i].b), "m" (tests[i].a)
            : "st", "st(1)"
        );
        
        printf("Result: ");
        print_float80(result);
        print_x87_status();
    }
}

// Test FIDIVR with 32-bit integer
void test_fidivr_32bit() {
    struct {
        int32_t a;
        long double b;
        const char* desc;
    } tests[] = {
        {2, 4.0L, "Normal division"},
        {3, 1.0L, "Fraction division"},
        {0, 1.0L, "Divide by zero"},
        {2, POS_INF, "Infinity divided"},
        {0x7FFFFFFF, 2.0L, "Divide by INT32_MAX"},
        {0x80000000, 2.0L, "Divide by INT32_MIN"},
        {2, POS_NAN, "NaN divided"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %d / %Lf\n", tests[i].a, tests[i].b);
        
        asm volatile (
            "fildl %2\n\t"      // Load integer to st(0)
            "fldt %1\n\t"       // Load float to st(0), push integer to st(1)
            "fidivr %2\n\t"     // st(0) = mem32/st(0) = a/b
            "fstpt %0\n\t"      // Store result
            : "=m" (result)
            : "m" (tests[i].b), "m" (tests[i].a)
            : "st", "st(1)"
        );
        
        printf("Result: ");
        print_float80(result);
        print_x87_status();
    }
}

// Test FIDIVR instruction
void test_fidivr() {
    printf("=== Testing FIDIVR (16-bit) ===\n");
    test_fidivr_16bit();
    
    printf("\n=== Testing FIDIVR (32-bit) ===\n");
    test_fidivr_32bit();
}

int main() {
    test_fidivr();
    return 0;
}
