#include "x87.h"
#include <stdio.h>
#include <stdint.h>

// Declare assembly function
extern void ficomp_test(long double a, int32_t b);

// Test cases for FICOMP (compare ST(0) with memory integer and pop)
static const struct {
    long double st0;
    int32_t mem_int;
} test_cases[] = {
    {0.0L, 0},          // 0.0 vs 0
    {1.0L, 1},          // 1.0 vs 1  
    {-1.0L, -1},        // -1.0 vs -1
    {1.5L, 1},          // 1.5 vs 1
    {1.0L, 2},          // 1.0 vs 2
    {100.0L, 100},      // 100.0 vs 100
    {1e10L, 1000000000},// large float vs large int
    {1e-10L, 0},        // small float vs 0
    {POS_INF, INT32_MAX}, // +INF vs max int
    {NEG_INF, INT32_MIN}, // -INF vs min int
    {POS_NAN, 0},       // NAN vs 0
};

int main() {
    uint16_t cw = 0x037f;  // Control word with all exceptions masked
    
    printf("Testing FICOMP (ST(0) vs memory integer with pop):\n");
    
    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); ++i) {
        long double a = test_cases[i].st0;
        int32_t b = test_cases[i].mem_int;
        
        // Initialize x87 state
        asm volatile ("fninit\n\t"
                      "fldcw %0" : : "m" (cw));
        
        printf("\nTest case %zu:\n", i+1);
        printf("ST(0): "); print_float80(a);
        printf("Mem: %d (0x%08x)\n", b, b);
        
        // Call assembly function to perform FICOMP
        ficomp_test(a, b);
        
        // Print x87 status after comparison
        print_x87_status();
        
        // Verify stack was popped by checking tag word
        uint16_t tag;
        asm volatile ("fnstsw %0" : "=m" (tag));
        if ((tag & 0x4000) == 0) {  // Check if ST(0) is empty
            printf("Stack properly popped - ST(0) is empty\n");
        } else {
            printf("ERROR: Stack not properly popped!\n");
        }
    }
    
    return 0;
}
