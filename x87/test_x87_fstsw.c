#include <stdio.h>
#include <stdint.h>
#include <fenv.h>
#include "x87.h"

#define TEST_CASE(name, condition) \
    do { \
        printf("[TEST] %-40s %s\n", name, (condition) ? "PASS" : "FAIL"); \
        if (!(condition)) { \
            printf("  [FAIL DETAIL] %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

void compare_fstsw_fnstsw() {
    // 原有实现...
}

void test_status_word_save() {
    // 原有实现... 
}

void test_condition_codes() {
    // 原有实现...
}

void test_stack_pointer() {
    // 原有实现...
}

void test_exception_state() {
    // 原有实现...
}

void test_fstsw_status() {
    // 原有实现...
}

void test_fstsw_modern() {
    // 原有实现...
}

void test_fstsw_boundary() {
    printf("\n==== Testing FSTSW Boundary Conditions ====\n");
    
    struct {
        const char* desc;
        uint16_t cw;
        int expected_exceptions;
    } tests[] = {
        {"Minimum control word (all unmasked)", 0x0000, FE_DIVBYZERO},
        {"Single bit set (IE unmasked)", 0x0001, FE_DIVBYZERO},
        {"Max value - 1", 0x7FFF, FE_DIVBYZERO},
        {"Maximum control word", 0xFFFF, FE_DIVBYZERO},
        {"Alternating bits", 0x5555, FE_DIVBYZERO},
        {"Inverse bits", 0xAAAA, FE_DIVBYZERO}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s (CW=0x%04X)\n", i+1, tests[i].desc, tests[i].cw);
        
        feclearexcept(FE_ALL_EXCEPT);
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        
        float zero = 0.0f;
        float one = 1.0f;
        uint16_t sw;
        asm volatile (
            "flds %1\n\t"
            "fdivs %0\n\t"
            "fnstsw %2"
            : 
            : "m" (zero), "m" (one), "m" (sw)
            : "memory"
        );
        
        int exceptions = fetestexcept(FE_ALL_EXCEPT);
        printf("Status Word: 0x%04X\n", sw);
        printf("Exceptions: 0x%X\n", exceptions);
        
        TEST_CASE("Exception flag set", (sw & 0x04) != 0);
        // Status word bits 15-14 may vary by implementation
        printf("Status word details: C3=%d C2=%d C1=%d C0=%d TOP=%d\n",
              (sw>>14)&1, (sw>>10)&1, (sw>>9)&1, (sw>>8)&1, (sw>>11)&7);
        
        // For unmasked exceptions, check if expected flags are set
        if(tests[i].cw & 0x3F) { // If any exceptions unmasked
            TEST_CASE("Correct exceptions raised",
                    (exceptions & tests[i].expected_exceptions) != 0);
        }
    }
}

int main() {
    printf("=== Testing FSTSW/FNSTSW instructions ===\n");
    
    compare_fstsw_fnstsw();
    test_status_word_save();
    test_condition_codes();
    test_stack_pointer();
    test_exception_state();
    test_fstsw_boundary();
    test_fstsw_modern();
    test_fstsw_status();
    
    printf("\n=== FSTSW/FNSTSW tests completed ===\n");
    return 0;
}
