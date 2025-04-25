#include "x87.h"
#include <stdio.h>
#include <stdint.h>

void test_fdivrp() {
    long double result = 0;
    uint16_t status = 0;
    int stack_depth = 0;
    
    printf("=== Testing FDIVRP ===\n");
    
    struct {
        long double a;
        long double b;
        const char* desc;
    } tests[] = {
        {8.0L, 2.0L, "Normal reverse division"},
        {1.0L, 0.0L, "Divide zero by value"}, 
        {0.0L, 0.0L, "Zero divided by zero"},
        {POS_INF, 2.0L, "Infinity divided"},
        {2.0L, POS_INF, "Divide by infinity"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        // Get initial stack depth
        asm volatile ("fnstsw %0" : "=m" (status));
        stack_depth = (status >> 11) & 0x7;
        
        asm volatile (
            "fldt %2\n\t"      // Load b (divisor)
            "fldt %1\n\t"      // Load a (dividend)
            "fdivrp\n\t"       // st(1) = st(0)/st(1), pop
            "fstpt %0\n\t"     // Store result
            "fnstsw %3\n\t"    // Store status
            : "=m" (result), "=m" (status)
            : "m" (tests[i].a), "m" (tests[i].b)
            : "cc"
        );
        
        // Get new stack depth
        uint16_t new_status = 0;
        asm volatile ("fnstsw %0" : "=m" (new_status));
        int new_depth = (new_status >> 11) & 0x7;
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %Lf / %Lf (reversed)\n", tests[i].a, tests[i].b);
        printf("Result: %Lf\n", result);
        printf("Stack depth before: %d, after: %d\n", stack_depth, new_depth);
        printf("Status Word: 0x%04x\n", status);
        print_x87_status();
    }
}

int main() {
    printf("NOTE: FDIVRP test needs further debugging\n");
    test_fdivrp();
    return 0;
}
