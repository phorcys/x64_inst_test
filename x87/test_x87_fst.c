#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fenv.h>
#include <math.h>
#include <float.h>
#define FE_STACK_FAULT 0x4000
#include "x87.h"

#define TEST_CASE(name, condition) \
    do { \
        printf("[TEST] %-40s %s\n", name, (condition) ? "PASS" : "FAIL"); \
        if (!(condition)) { \
            printf("  [FAIL DETAIL] %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

void test_fst_basic() {
    // 32-bit float tests (existing)
    float f32_values[] = {
        0.0f, -0.0f, 1.0f, -1.0f,
        FLT_MAX, -FLT_MAX, FLT_MIN, -FLT_MIN,
        INFINITY, -INFINITY, NAN, -NAN
    };

    printf("\n==== Testing FST (32-bit float) ====\n");
    for (size_t i = 0; i < sizeof(f32_values)/sizeof(f32_values[0]); i++) {
        float value = f32_values[i];
        float stored_value;
        
        __asm__ volatile (
            "fninit\n\t"
            "flds %1\n\t"
            "fsts %0\n\t"
            : "=m" (stored_value)
            : "m" (value)
            : "st"
        );

        printf("Original: %.15g\n", value);
        printf("Stored:   %.15g\n", stored_value);
        
        if (isnan(value) && isnan(stored_value)) {
            TEST_CASE("NaN value stored correctly", 1);
        } else {
            TEST_CASE("Value stored correctly", value == stored_value);
        }
    }
}

void test_fst_double() {
    // 64-bit double tests
    double d_values[] = {
        0.0, -0.0, 1.0, -1.0,
        DBL_MAX, -DBL_MAX, DBL_MIN, -DBL_MIN,
        INFINITY, -INFINITY, NAN, -NAN,
        3.141592653589793, 2.718281828459045
    };

    printf("\n==== Testing FST (64-bit double) ====\n");
    for (size_t i = 0; i < sizeof(d_values)/sizeof(d_values[0]); i++) {
        double value = d_values[i];
        double stored_value;
        
        printf("\nBefore FST:\n");
        print_x87_status();
        
        __asm__ volatile (
            "fninit\n\t"
            "fldl %1\n\t"
            "fstl %0\n\t"
            : "=m" (stored_value)
            : "m" (value)
            : "st"
        );

        printf("\nAfter FST:\n");
        print_x87_status();
        
        printf("Original: %.15g\n", value);
        printf("Stored:   %.15g\n", stored_value);
        
        if (isnan(value) && isnan(stored_value)) {
            TEST_CASE("NaN value stored correctly", 1);
        } else {
            TEST_CASE("Value stored correctly", value == stored_value);
        }
    }
}

void test_fst_rounding() {
    printf("\n==== Testing FST with Different Rounding Modes ====\n");
    
    double test_values[] = {1.5, -1.5, 2.5, -2.5, 1.25, -1.25};
    const char* mode_names[] = {"Nearest", "Down", "Up", "Truncate"};
    uint16_t modes[] = {0x0000, 0x0400, 0x0800, 0x0C00};
    
    for (int m = 0; m < 4; m++) {
        // Set rounding mode
        uint16_t cw = 0x037F & ~0x0C00;  // Default control word
        cw |= modes[m];  // Set rounding mode
        asm volatile ("fldcw %0" : : "m" (cw));
        
        printf("\nMode: %s (CW=0x%04X)\n", mode_names[m], cw);
        
        for (size_t i = 0; i < sizeof(test_values)/sizeof(test_values[0]); i++) {
            double value = test_values[i];
            double stored;
            
            asm volatile (
                "fninit\n\t"
                "fldl %1\n\t"
                "fstl %0\n\t"
                : "=m" (stored)
                : "m" (value)
                : "st"
            );
            
            printf("Value: %6.2f => Stored: %6.2f\n", value, stored);
        }
    }
}

void test_fst_integer() {
    printf("\n==== Testing FST with Integers ====\n");
    
    // Test 16-bit integers
    int16_t i16_values[] = {0, 1, -1, 32767, -32768};
    for (size_t i = 0; i < sizeof(i16_values)/sizeof(i16_values[0]); i++) {
        int16_t value = i16_values[i];
        int16_t stored;
        
        asm volatile (
            "fninit\n\t"
            "filds %1\n\t"
            "fists %0\n\t"
            : "=m" (stored)
            : "m" (value)
            : "st"
        );
        
        printf("Original: %6d => Stored: %6d\n", value, stored);
        TEST_CASE("16-bit integer stored correctly", value == stored);
    }
    
    // Test 32-bit integers
    int32_t i32_values[] = {0, 1, -1, 2147483647, -2147483648};
    for (size_t i = 0; i < sizeof(i32_values)/sizeof(i32_values[0]); i++) {
        int32_t value = i32_values[i];
        int32_t stored;
        
        asm volatile (
            "fninit\n\t"
            "fildl %1\n\t"
            "fistl %0\n\t"
            : "=m" (stored)
            : "m" (value)
            : "st"
        );
        
        printf("Original: %11d => Stored: %11d\n", value, stored);
        TEST_CASE("32-bit integer stored correctly", value == stored);
    }
}

void test_fst_status() {
    printf("\n==== Testing FST Status Effects ====\n");
    
    struct {
        const char* desc;
        uint16_t cw;
        float value;
        int expected_exceptions;
    } tests[] = {
        {"Normal value, default rounding", 0x037F, 1.2345f, 0},
        {"Denormal value", 0x037F, FLT_MIN/2.0f, FE_UNDERFLOW|FE_INEXACT},
        {"Overflow value", 0x037F, FLT_MAX*2.0f, FE_OVERFLOW|FE_INEXACT},
        {"NaN value", 0x037F, NAN, FE_INVALID}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        feclearexcept(FE_ALL_EXCEPT);
        
        float stored;
        asm volatile (
            "flds %1\n\t"
            "fsts %0\n\t"
            : "=m" (stored)
            : "m" (tests[i].value)
            : "st"
        );
        
        int exceptions = fetestexcept(FE_ALL_EXCEPT);
        printf("Stored value: %.15g\n", stored);
        printf("Exceptions: 0x%X\n", exceptions);
        TEST_CASE("Correct exceptions raised", 
                 (exceptions & tests[i].expected_exceptions) == tests[i].expected_exceptions);
    }
}

void test_fst_modern() {
    printf("\n==== Testing FST with Modern CPU Features ====\n");
    
    // Test with SSE enabled
    printf("\n-- Test 1: With SSE --\n");
    uint32_t mxcsr_before, mxcsr_after;
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_before));
    
    // Generate FPU state
    asm volatile ("fninit");
    asm volatile ("fld1");
    float stored;
    asm volatile ("fsts %0" : "=m" (stored));
    
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_after));
    printf("MXCSR before: 0x%08X\n", mxcsr_before);
    printf("MXCSR after:  0x%08X\n", mxcsr_after);
    TEST_CASE("MXCSR unchanged with FST", mxcsr_before == mxcsr_after);
    
    // Test with AVX enabled
    printf("\n-- Test 2: With AVX --\n");
    uint64_t xcr0_before, xcr0_after;
    asm volatile ("xgetbv" : "=a" (xcr0_before) : "c" (0) : "edx");
    
    asm volatile ("fninit");
    asm volatile ("fld1");
    asm volatile ("fsts %0" : "=m" (stored));
    
    asm volatile ("xgetbv" : "=a" (xcr0_after) : "c" (0) : "edx");
    printf("XCR0 before: 0x%016lX\n", xcr0_before);
    printf("XCR0 after:  0x%016lX\n", xcr0_after);
    TEST_CASE("XCR0 unchanged with FST", xcr0_before == xcr0_after);
}

void test_fst_multithread() {
    printf("\n==== Testing FST in Multithreaded Context ====\n");
    
    // Simulate multithreaded behavior by alternating between different FPU states
    for (int i = 0; i < 3; i++) {
        printf("\nIteration %d\n", i+1);
        
        // State 1: Normal values
        asm volatile ("fninit");
        asm volatile ("fld1");
        asm volatile ("fldln2");
        uint16_t cw_state1 = get_x87_cw();
        
        float value1 = 1.2345f;
        float stored1;
        asm volatile (
            "flds %1\n\t"
            "fsts %0\n\t"
            : "=m" (stored1)
            : "m" (value1)
            : "st"
        );
        
        printf("State 1 stored: %.15g\n", stored1);
        TEST_CASE("State 1 value correct", stored1 == value1);
        TEST_CASE("State 1 control word preserved", get_x87_cw() == cw_state1);
        
        // State 2: Different control word and values
        asm volatile ("fninit");
        uint16_t cw = 0x077F;
        asm volatile ("fldcw %0" : : "m" (cw));  // Round down
        asm volatile ("fldpi");
        asm volatile ("fldz");
        uint16_t cw_state2 = get_x87_cw();
        
        float value2 = 2.3456f;
        float stored2;
        asm volatile (
            "flds %1\n\t"
            "fsts %0\n\t"
            : "=m" (stored2)
            : "m" (value2)
            : "st"
        );
        
        printf("State 2 stored: %.15g\n", stored2);
        TEST_CASE("State 2 value correct", stored2 == value2);
        TEST_CASE("State 2 control word preserved", get_x87_cw() == cw_state2);
    }
}

void test_fst_boundary() {
    printf("\n==== Testing FST Boundary Values ====\n");
    
    struct {
        const char* desc;
        float value;
        int expected_exception;
        uint16_t cw;
        uint16_t expected_sw_mask;
    } tests[] = {
        {"Minimum denormal (default rounding)", FLT_MIN/2.0f, FE_UNDERFLOW, 0x037F, 0xFFFF},
        {"Maximum denormal (round down)", FLT_MIN-FLT_MIN/2.0f, FE_UNDERFLOW, 0x077F, 0xFFFF},
        {"Minimum normal (round up)", FLT_MIN, 0, 0x0B7F, 0xFFFF},
        {"Maximum normal (truncate)", FLT_MAX, 0, 0x0F7F, 0xFFFF},
        {"Just below overflow", FLT_MAX*0.999f, 0, 0x037F, 0xFFFF},
        {"Just above overflow", FLT_MAX*1.001f, FE_OVERFLOW, 0x037F, 0xFFFF},
        {"Positive infinity", INFINITY, 0, 0x037F, 0xFFFF},
        {"Negative infinity", -INFINITY, 0, 0x037F, 0xFFFF},
        {"Quiet NaN", NAN, FE_INVALID, 0x037F, 0xFFFF},
        {"Signaling NaN", -NAN, FE_INVALID, 0x037F, 0xFFFF},
        {"Denormal with DAZ", FLT_MIN/2.0f, FE_UNDERFLOW, 0x037F | 0x0040, 0xFFFF},
        {"Stack underflow", 1.0f, FE_STACK_FAULT, 0x037F, 0x4000},
        {"Precision loss", 1.0f/3.0f, FE_INEXACT, 0x037F, 0xFFFF}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        feclearexcept(FE_ALL_EXCEPT);
        
        uint16_t cw_before = get_x87_cw();
        uint16_t tw_before = get_x87_tw();
        
        float stored;
        asm volatile (
            "flds %1\n\t"
            "fsts %0\n\t"
            : "=m" (stored)
            : "m" (tests[i].value)
            : "st"
        );
        
        uint16_t cw_after = get_x87_cw();
        uint16_t sw_after = get_x87_sw();
        uint16_t tw_after = get_x87_tw();
        int exceptions = fetestexcept(FE_ALL_EXCEPT);
        
        printf("Original: %.15g\n", tests[i].value);
        printf("Stored:   %.15g\n", stored);
        printf("Exceptions: 0x%X\n", exceptions);
        printf("Control Word: 0x%04X\n", tests[i].cw);
        printf("Status Word: 0x%04X\n", sw_after);
        
        TEST_CASE("Control word unchanged", 
                 (cw_before & 0x0F00) == (cw_after & 0x0F00));
        TEST_CASE("Status word matches expected", 
                 (sw_after & tests[i].expected_sw_mask) == tests[i].expected_sw_mask);
        TEST_CASE("Tag word unchanged", tw_before == tw_after);
        
        if (isnan(tests[i].value)) {
            TEST_CASE("NaN value stored correctly", isnan(stored));
        } else {
            TEST_CASE("Value stored correctly", tests[i].value == stored);
        }
        TEST_CASE("Correct exception raised", 
                 (exceptions & tests[i].expected_exception) == tests[i].expected_exception);
    }
}

void test_fst_vs_fstp() {
    printf("\n==== Testing FST vs FSTP ====\n");
    
    double value = 3.141592653589793;
    double stored_fst, stored_fstp;
    
    // Test FST
    asm volatile (
        "fninit\n\t"
        "fldl %1\n\t"
        "fstl %0\n\t"
        : "=m" (stored_fst)
        : "m" (value)
        : "st"
    );
    
    uint16_t sw_after_fst = get_x87_sw();
    int top_after_fst = (sw_after_fst >> 11) & 0x7;
    
    // Test FSTP
    asm volatile (
        "fninit\n\t"
        "fldl %1\n\t"
        "fstpl %0\n\t"
        : "=m" (stored_fstp)
        : "m" (value)
        : "st"
    );
    
    uint16_t sw_after_fstp = get_x87_sw();
    int top_after_fstp = (sw_after_fstp >> 11) & 0x7;
    
    printf("After FST: TOP=%d\n", top_after_fst);
    printf("After FSTP: TOP=%d\n", top_after_fstp);
    
    TEST_CASE("FST preserves stack", top_after_fst == 7);
    TEST_CASE("FSTP modifies stack", top_after_fstp == 0);
    TEST_CASE("Both store same value", stored_fst == stored_fstp);
}

void test_fst_combinations() {
    printf("\n==== Testing FST Combinations ====\n");
    
    struct {
        const char* desc;
        uint16_t cw;
        float value;
    } tests[] = {
        {"Normal value, default rounding", 0x037F, 1.2345f},
        {"Normal value, round down", 0x077F, 1.2345f},
        {"Normal value, round up", 0x0B7F, 1.2345f},
        {"Normal value, truncate", 0x0F7F, 1.2345f},
        {"Denormal value", 0x037F, FLT_MIN/2.0f},
        {"Large value", 0x037F, FLT_MAX*0.999f},
        {"Small value", 0x037F, FLT_MIN*2.0f}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        
        float stored;
        asm volatile (
            "flds %1\n\t"
            "fsts %0\n\t"
            : "=m" (stored)
            : "m" (tests[i].value)
            : "st"
        );
        
        printf("Original: %.15g\n", tests[i].value);
        printf("Stored:   %.15g\n", stored);
        TEST_CASE("Value stored correctly", tests[i].value == stored);
    }
}

void test_fst_exceptions() {
    printf("\n==== Testing FST Exceptions ====\n");
    
    struct {
        const char* desc;
        float value;
        int expected_exception;
    } tests[] = {
        {"Overflow", FLT_MAX * 2.0f, FE_OVERFLOW},
        {"Underflow", FLT_MIN / 2.0f, FE_UNDERFLOW},
        {"Inexact", 1.0f/3.0f, FE_INEXACT},
        {"Invalid (NaN)", NAN, FE_INVALID}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        feclearexcept(FE_ALL_EXCEPT);
        float stored;
        
        asm volatile (
            "fninit\n\t"
            "flds %1\n\t"
            "fsts %0\n\t"
            : "=m" (stored)
            : "m" (tests[i].value)
            : "st"
        );
        
        int exceptions = fetestexcept(FE_ALL_EXCEPT);
        printf("Stored value: %.15g\n", stored);
        printf("Exceptions: 0x%X\n", exceptions);
        TEST_CASE("Correct exception raised", 
                 (exceptions & tests[i].expected_exception) != 0);
    }
}

void test_fst_extended() {
    // 80-bit float tests
    long double ld_values[] = {
        0.0L, -0.0L, 1.0L, -1.0L,
        LDBL_MAX, -LDBL_MAX, LDBL_MIN, -LDBL_MIN,
        INFINITY, -INFINITY, NAN, -NAN,
        3.1415926535897932385L, 2.7182818284590452354L
    };

    printf("\n==== Testing FST (80-bit float) ====\n");
    for (size_t i = 0; i < sizeof(ld_values)/sizeof(ld_values[0]); i++) {
        long double value = ld_values[i];
        long double stored_value;
        
        printf("\nBefore FST:\n");
        print_x87_status();
        
        __asm__ volatile (
            "fninit\n\t"
            "fldt %1\n\t"
            "fstpt %0\n\t"
            : "=m" (stored_value)
            : "m" (value)
            : "st"
        );

        printf("\nAfter FST:\n");
        print_x87_status();
        
        printf("Original: "); print_float80(value);
        printf("Stored:   "); print_float80(stored_value);
        
        if (isnan(value) && isnan(stored_value)) {
            TEST_CASE("NaN value stored correctly", 1);
        } else {
            TEST_CASE("Value stored correctly", value == stored_value);
        }
    }

    // Register-to-register tests
    printf("\n==== Testing FST ST(i) ====\n");
    long double st0_val = 1.23456789L;
    long double st1_val = 2.34567890L;
    
    __asm__ volatile ("fninit\n\t");
    __asm__ volatile ("fldt %0" : : "m" (st0_val));
    __asm__ volatile ("fldt %0" : : "m" (st1_val));
    
    printf("Initial stack:\n");
    print_x87_status();
    
    // Store ST0 to ST1
    __asm__ volatile ("fst %%st(1)" : : : "st");
    
    printf("\nAfter FST ST(1):\n");
    print_x87_status();
    
    long double new_st0, new_st1;
    __asm__ volatile ("fstpt %0" : "=m" (new_st0));
    __asm__ volatile ("fstpt %0" : "=m" (new_st1));
    
    printf("ST0: "); print_float80(new_st0);
    printf("ST1: "); print_float80(new_st1);
    
    TEST_CASE("ST1 updated correctly", new_st1 == st1_val);
    TEST_CASE("ST0 unchanged", new_st0 == st1_val);
}

int main() {
    fesetround(FE_TONEAREST);
    feclearexcept(FE_ALL_EXCEPT);

    test_fst_basic();
    test_fst_double();
    test_fst_rounding();
    test_fst_integer();
    test_fst_boundary();
    test_fst_modern();
    test_fst_multithread();
    test_fst_status();
    test_fst_combinations();
    test_fst_exceptions();
    test_fst_vs_fstp();
    test_fst_extended();

    int exceptions = fetestexcept(FE_ALL_EXCEPT);
    if (exceptions) {
        printf("\n[WARNING] Floating-point exceptions occurred: ");
        if (exceptions & FE_INVALID) printf("FE_INVALID ");
        if (exceptions & FE_DIVBYZERO) printf("FE_DIVBYZERO ");
        if (exceptions & FE_OVERFLOW) printf("FE_OVERFLOW ");
        if (exceptions & FE_UNDERFLOW) printf("FE_UNDERFLOW ");
        if (exceptions & FE_INEXACT) printf("FE_INEXACT ");
        printf("\n");
    }

    return 0;
}
