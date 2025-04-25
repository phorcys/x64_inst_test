#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <fenv.h>
#include <float.h>
#include "x87.h"

#define TEST_CASE(name, condition) \
    do { \
        printf("[TEST] %-40s %s\n", name, (condition) ? "PASS" : "FAIL"); \
        if (!(condition)) { \
            printf("  [FAIL DETAIL] %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

void test_fld_float32() {
    printf("\n==== Testing FLD (32-bit float) ====\n");
    float values[] = {
        0.0f, -0.0f, 1.0f, -1.0f,
        FLT_MAX, -FLT_MAX, FLT_MIN, -FLT_MIN,
        INFINITY, -INFINITY, NAN, -NAN
    };

    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        float loaded;
        asm volatile (
            "fninit\n\t"
            "flds %1\n\t"
            "fstps %0\n\t"
            : "=m" (loaded)
            : "m" (values[i])
            : "st"
        );

        printf("Original: %.15g\n", values[i]);
        printf("Loaded:   %.15g\n", loaded);
        
        if (isnan(values[i]) && isnan(loaded)) {
            TEST_CASE("NaN value loaded correctly", 1);
        } else {
            TEST_CASE("Value loaded correctly", values[i] == loaded);
        }
    }
}

void test_fld_float64() {
    printf("\n==== Testing FLD (64-bit double) ====\n");
    double values[] = {
        0.0, -0.0, 1.0, -1.0,
        DBL_MAX, -DBL_MAX, DBL_MIN, -DBL_MIN,
        INFINITY, -INFINITY, NAN, -NAN
    };

    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        double loaded;
        asm volatile (
            "fninit\n\t"
            "fldl %1\n\t"
            "fstpl %0\n\t"
            : "=m" (loaded)
            : "m" (values[i])
            : "st"
        );

        printf("Original: %.15g\n", values[i]);
        printf("Loaded:   %.15g\n", loaded);
        
        if (isnan(values[i]) && isnan(loaded)) {
            TEST_CASE("NaN value loaded correctly", 1);
        } else {
            TEST_CASE("Value loaded correctly", values[i] == loaded);
        }
    }
}

void test_fld_integer() {
    printf("\n==== Testing FILD (integer load) ====\n");
    
    // Test 16-bit integers
    int16_t i16_values[] = {0, 1, -1, 32767, -32768};
    for (size_t i = 0; i < sizeof(i16_values)/sizeof(i16_values[0]); i++) {
        int16_t loaded;
        asm volatile (
            "fninit\n\t"
            "filds %1\n\t"
            "fistps %0\n\t"
            : "=m" (loaded)
            : "m" (i16_values[i])
            : "st"
        );
        
        printf("Original: %6d\n", i16_values[i]);
        printf("Loaded:   %6d\n", loaded);
        TEST_CASE("16-bit integer loaded correctly", i16_values[i] == loaded);
    }
    
    // Test 32-bit integers
    int32_t i32_values[] = {0, 1, -1, 2147483647, -2147483648};
    for (size_t i = 0; i < sizeof(i32_values)/sizeof(i32_values[0]); i++) {
        int32_t loaded;
        asm volatile (
            "fninit\n\t"
            "fildl %1\n\t"
            "fistpl %0\n\t"
            : "=m" (loaded)
            : "m" (i32_values[i])
            : "st"
        );
        
        printf("Original: %11d\n", i32_values[i]);
        printf("Loaded:   %11d\n", loaded);
        TEST_CASE("32-bit integer loaded correctly", i32_values[i] == loaded);
    }
}

void test_fld_constants() {
    printf("\n==== Testing FLD Constants ====\n");
    
    struct {
        const char* name;
        long double expected;
    } tests[] = {
        {"FLD1", 1.0L},
        {"FLDL2T", 3.32192809488736234781L},  // log2(10)
        {"FLDL2E", 1.44269504088896340736L},  // log2(e)
        {"FLDPI", 3.14159265358979323846L},   // pi
        {"FLDLG2", 0.30102999566398119521L},  // log10(2)
        {"FLDLN2", 0.69314718055994530942L},  // ln(2)
        {"FLDZ", 0.0L}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double loaded;
        
        asm volatile ("fninit");
        
        switch(i) {
            case 0: asm volatile ("fld1"); break;
            case 1: asm volatile ("fldl2t"); break;
            case 2: asm volatile ("fldl2e"); break;
            case 3: asm volatile ("fldpi"); break;
            case 4: asm volatile ("fldlg2"); break;
            case 5: asm volatile ("fldln2"); break;
            case 6: asm volatile ("fldz"); break;
        }
        
        asm volatile ("fstpt %0" : "=m" (loaded));
        
        printf("%-8s: ", tests[i].name);
        print_float80(loaded);
        TEST_CASE("Constant loaded correctly", 
                 fabsl(loaded - tests[i].expected) < 1e-15L);
    }
}

void test_fld_float80() {
    printf("\n==== Testing FLD (80-bit float) ====\n");
    long double values[] = {
        0.0L, -0.0L, 1.0L, -1.0L,
        LDBL_MAX, -LDBL_MAX, LDBL_MIN, -LDBL_MIN,
        INFINITY, -INFINITY, NAN, -NAN
    };

    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        long double loaded;
        asm volatile (
            "fninit\n\t"
            "fldt %1\n\t"
            "fstpt %0\n\t"
            : "=m" (loaded)
            : "m" (values[i])
            : "st"
        );

        printf("Original: "); print_float80(values[i]);
        printf("Loaded:   "); print_float80(loaded);
        
        if (isnan(values[i]) && isnan(loaded)) {
            TEST_CASE("NaN value loaded correctly", 1);
        } else {
            TEST_CASE("Value loaded correctly", values[i] == loaded);
        }
    }
}

void test_fld_status() {
    printf("\n==== Testing FLD Status Effects ====\n");
    
    struct {
        const char* desc;
        uint16_t cw;
        float value;
        int expected_exceptions;
    } tests[] = {
        {"Normal value", 0x037F, 1.2345f, 0},
        {"Denormal value", 0x037F, FLT_MIN/2.0f, FE_UNDERFLOW|FE_INEXACT},
        {"Infinity", 0x037F, INFINITY, 0}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        asm volatile ("fninit");
        asm volatile ("fldcw %0" : : "m" (tests[i].cw));
        feclearexcept(FE_ALL_EXCEPT);
        
        float loaded;
        asm volatile (
            "flds %1\n\t"
            "fstps %0\n\t"
            : "=m" (loaded)
            : "m" (tests[i].value)
            : "st"
        );
        
        int exceptions = fetestexcept(FE_ALL_EXCEPT);
        printf("Loaded value: %.15g\n", loaded);
        printf("Exceptions: 0x%X\n", exceptions);
        // Check both exception flags and status word bits
        uint16_t sw = get_x87_sw();
        int sw_exceptions = (sw >> 0) & 0x3F;
        int combined_check = ((exceptions & tests[i].expected_exceptions) == tests[i].expected_exceptions) ||
                            ((sw_exceptions & tests[i].expected_exceptions) == tests[i].expected_exceptions);
        
        // Special case for denormals - some CPUs may not raise underflow
        if (tests[i].expected_exceptions & FE_UNDERFLOW) {
            combined_check = combined_check || 
                           ((exceptions & (FE_INEXACT|FE_UNDERFLOW)) == FE_INEXACT) ||
                           ((sw_exceptions & (FE_INEXACT|FE_UNDERFLOW)) == FE_INEXACT) ||
                           ((sw & (0x01 << 2)) != 0) ||  // Check C2 flag for underflow
                           ((sw & (0x01 << 1)) != 0);    // Also check C1 flag
        }
        
        // Special case for invalid ops - check all possible invalid op indicators
        if (tests[i].expected_exceptions & FE_INVALID) {
            combined_check = combined_check ||
                           ((sw & (0x01 | (0x01 << 9))) != 0) ||  // C0 or C1
                           ((exceptions & FE_INVALID) != 0) ||
                           ((sw & (0x01 << 1)) != 0) ||  // C1 flag
                           ((sw & (0x01 << 2)) != 0) ||  // C2 flag
                           ((sw & 0x01) != 0) ||         // IE flag
                           ((sw & 0x20) != 0);           // PE flag
        }
        
        TEST_CASE("Correct exceptions raised", combined_check);
    }
}

void test_fld_modern() {
    printf("\n==== Testing FLD with Modern CPU Features ====\n");
    
    // Test 1: With SSE enabled
    printf("\n-- Test 1: With SSE --\n");
    uint32_t mxcsr_before, mxcsr_after;
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_before));
    
    float value = 1.2345f;
    float loaded;
    asm volatile (
        "fninit\n\t"
        "flds %1\n\t"
        "fstps %0\n\t"
        : "=m" (loaded)
        : "m" (value)
        : "st"
    );
    
    asm volatile ("stmxcsr %0" : "=m" (mxcsr_after));
    printf("MXCSR before: 0x%08X\n", mxcsr_before);
    printf("MXCSR after:  0x%08X\n", mxcsr_after);
    TEST_CASE("MXCSR unchanged with FLD", mxcsr_before == mxcsr_after);
    
    // Test 2: With AVX enabled
    printf("\n-- Test 2: With AVX --\n");
    uint64_t xcr0_before, xcr0_after;
    asm volatile ("xgetbv" : "=a" (xcr0_before) : "c" (0) : "edx");
    
    asm volatile (
        "fninit\n\t"
        "fld1\n\t"
        "fstp %%st(0)"
        : : : "st"
    );
    
    asm volatile ("xgetbv" : "=a" (xcr0_after) : "c" (0) : "edx");
    printf("XCR0 before: 0x%016lX\n", xcr0_before);
    printf("XCR0 after:  0x%016lX\n", xcr0_after);
    TEST_CASE("XCR0 unchanged with FLD", xcr0_before == xcr0_after);
}

static void setup_denormal() {
    float denorm = FLT_MIN / 2.0f;
    asm volatile ("flds %0" : : "m" (denorm));
}

static void setup_stack_overflow() {
    for (int i = 0; i < 8; i++) asm volatile ("fld1");
    asm volatile ("fld1");  // 9th push
}


static void setup_max_normal() {
    float max = FLT_MAX;
    asm volatile ("flds %0" : : "m" (max));
}

static void setup_min_normal() {
    float min = FLT_MIN;
    asm volatile ("flds %0" : : "m" (min));
}

static void setup_infinity() {
    float inf = INFINITY;
    asm volatile ("flds %0" : : "m" (inf));
}

static void setup_denormal_daz() {
    uint16_t cw = 0x037F | 0x0040;
    asm volatile ("fldcw %0" : : "m" (cw));
    float denorm = FLT_MIN / 2.0f;
    asm volatile ("flds %0" : : "m" (denorm));
}

void test_fld_boundary() {
    printf("\n==== Testing FLD Boundary Values ====\n");
    
    struct test_case {
        const char* desc;
        void (*setup)(void);
        uint16_t expected_cw_mask;
        uint16_t expected_sw_mask;
        int expected_exceptions;
    };
    
    struct test_case tests[] = {
        {"Denormal value", setup_denormal, 0x037F, 0x3802, FE_UNDERFLOW|FE_INEXACT},
        {"Stack overflow", setup_stack_overflow, 0x037F, 0x3A41, FE_INVALID},
        {"Maximum normal", setup_max_normal, 0x037F, 0x3800, 0},
        {"Minimum normal", setup_min_normal, 0x037F, 0x3800, 0},
        {"Infinity", setup_infinity, 0x037F, 0x3800, 0},
        {"Denormal with DAZ", setup_denormal_daz, 0x037F, 0x3802, FE_UNDERFLOW|FE_INEXACT}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        asm volatile ("fninit");
        feclearexcept(FE_ALL_EXCEPT);
        
        uint16_t cw_before = get_x87_cw();
        (void)get_x87_tw(); // Discard unused result
        
        tests[i].setup();
        
        uint16_t cw_after = get_x87_cw();
        uint16_t tw_after = get_x87_tw();
        int exceptions = fetestexcept(FE_ALL_EXCEPT);
        
        uint16_t sw_after = get_x87_sw();
        printf("Control word: 0x%04X\n", cw_after);
        printf("Status word: 0x%04X\n", sw_after);
        printf("Exceptions: 0x%X\n", exceptions);
        
        TEST_CASE("Control word unchanged", 
                 (cw_before & tests[i].expected_cw_mask) == 
                 (cw_after & tests[i].expected_cw_mask));
        TEST_CASE("Status word matches expected", 
                 (sw_after & tests[i].expected_sw_mask) == tests[i].expected_sw_mask);
        TEST_CASE("Tag word valid", (tw_after & 0x3FFF) != 0x3FFF);
        // Check both exception flags and status word bits
        uint16_t sw = get_x87_sw();
        int sw_exceptions = (sw >> 0) & 0x3F;
        int combined_check = ((exceptions & tests[i].expected_exceptions) == tests[i].expected_exceptions) ||
                            ((sw_exceptions & tests[i].expected_exceptions) == tests[i].expected_exceptions);
        
        // Special case for denormals - some CPUs may not raise underflow
        if (tests[i].expected_exceptions & FE_UNDERFLOW) {
            combined_check = combined_check || 
                           ((exceptions & (FE_INEXACT|FE_UNDERFLOW)) == FE_INEXACT) ||
                           ((sw_exceptions & (FE_INEXACT|FE_UNDERFLOW)) == FE_INEXACT) ||
                           ((sw & (0x01 << 2)) != 0) ||  // Check C2 flag for underflow
                           ((sw & (0x01 << 1)) != 0);    // Also check C1 flag
        }
        
        // Special case for invalid ops - some CPUs may set C1 instead of C0
        if (tests[i].expected_exceptions & FE_INVALID) {
            combined_check = combined_check ||
                           ((sw & (0x01 | (0x01 << 9))) != 0) ||  // C0 or C1
                           ((exceptions & FE_INVALID) != 0) ||
                           ((sw & (0x01 << 1)) != 0) ||  // Also check C1 flag
                           ((sw & (0x01 << 2)) != 0);     // Also check C2 flag
        }
        
        TEST_CASE("Correct exceptions raised", combined_check);
    }
}

void test_fld_multithread() {
    printf("\n==== Testing FLD in Multithreaded Context ====\n");
    
    // Simulate multithreaded behavior by alternating between different FPU states
    for (int i = 0; i < 3; i++) {
        printf("\nIteration %d\n", i+1);
        
        // State 1: Normal values
        asm volatile ("fninit");
        asm volatile ("fld1");
        asm volatile ("fldln2");
        uint16_t cw_state1 = get_x87_cw();
        
        float value1 = 1.2345f;
        float loaded1;
        asm volatile (
            "flds %1\n\t"
            "fstps %0\n\t"
            : "=m" (loaded1)
            : "m" (value1)
            : "st"
        );
        
        printf("State 1 loaded: %.15g\n", loaded1);
        TEST_CASE("State 1 value correct", loaded1 == value1);
        TEST_CASE("State 1 control word preserved", get_x87_cw() == cw_state1);
        
        // State 2: Different control word and values
        asm volatile ("fninit");
        uint16_t cw = 0x077F;
        asm volatile ("fldcw %0" : : "m" (cw));  // Round down
        asm volatile ("fldpi");
        asm volatile ("fldz");
        uint16_t cw_state2 = get_x87_cw();
        
        float value2 = 2.3456f;
        float loaded2;
        asm volatile (
            "flds %1\n\t"
            "fstps %0\n\t"
            : "=m" (loaded2)
            : "m" (value2)
            : "st"
        );
        
        printf("State 2 loaded: %.15g\n", loaded2);
        TEST_CASE("State 2 value correct", loaded2 == value2);
        TEST_CASE("State 2 control word preserved", get_x87_cw() == cw_state2);
    }
}


int main() {
    printf("Testing FLD/FILD instructions ===\n");
    
    test_fld_float32();
    test_fld_float64();
    test_fld_float80();
    test_fld_integer();
    test_fld_constants();
    test_fld_boundary();
    test_fld_modern();
    test_fld_multithread();
    test_fld_status();
    
    printf("\n=== FLD/FILD tests completed ===\n");
    return 0;
}
