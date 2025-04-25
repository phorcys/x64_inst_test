#include "x87.h"
#include <stdio.h>
#include <stdint.h>

// Test FIMUL with 16-bit integer
void test_fimul_16bit() {
    struct {
        int16_t a;
        long double b;
        const char* desc;
    } tests[] = {
        {2, 4.0L, "Normal multiplication"},
        {3, 1.5L, "Fraction multiplication"},
        {0, 1.0L, "Multiply by zero"},
        {2, POS_INF, "Infinity multiplication"},
        {0x7FFF, 2.0L, "Multiply by INT16_MAX"},
        {0x8000, 2.0L, "Multiply by INT16_MIN"},
        {2, POS_NAN, "NaN multiplication"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %d * %Lf\n", tests[i].a, tests[i].b);
        
        asm volatile (
            "fldt %1\n\t"       // Load b to st(0)
            "fimuls %2\n\t"      // st(0) = st(0) * mem16
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

// Test FIMUL with 32-bit integer
void test_fimul_32bit() {
    struct {
        int32_t a;
        long double b;
        const char* desc;
    } tests[] = {
        {2, 4.0L, "Normal multiplication"},
        {3, 1.5L, "Fraction multiplication"},
        {0, 1.0L, "Multiply by zero"},
        {2, POS_INF, "Infinity multiplication"},
        {0x7FFFFFFF, 2.0L, "Multiply by INT32_MAX"},
        {0x80000000, 2.0L, "Multiply by INT32_MIN"},
        {2, POS_NAN, "NaN multiplication"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %d * %Lf\n", tests[i].a, tests[i].b);
        
        asm volatile (
            "fldt %1\n\t"       // Load b to st(0)
            "fimull %2\n\t"      // st(0) = st(0) * mem32
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

// Test FIMUL instruction
void test_fimul() {
    printf("=== Testing FIMUL (16-bit) ===\n");
    test_fimul_16bit();
    
    printf("\n=== Testing FIMUL (32-bit) ===\n");
    test_fimul_32bit();
}

int main() {
    test_fimul();
    return 0;
}
