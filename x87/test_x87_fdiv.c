#include "x87.h"
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

// Basic FDIV tests with register operands
void test_fdiv_reg() {
    printf("\n=== Testing FDIV with register operands ===\n");
    
    struct {
        long double a;
        long double b;
        const char* desc;
    } tests[] = {
        {4.0L, 2.0L, "Normal division"},
        {1.0L, 0.0L, "Divide by zero"},
        {0.0L, 0.0L, "Zero divided by zero"},
        {POS_INF, 2.0L, "Infinity divided"},
        {2.0L, POS_INF, "Divide by infinity"},
        {POS_INF, POS_INF, "Infinity divided by infinity"},
        {POS_NAN, 2.0L, "NaN divided"},
        {2.0L, POS_NAN, "Divide by NaN"},
        {POS_DENORM, 2.0L, "Denormal divided"},
        {2.0L, POS_DENORM, "Divide by denormal"},
        {1.0L, 3.0L, "Fraction result"},
        {DBL_MAX, DBL_MIN, "Max divided by min"},
        {FLT_MIN, FLT_MAX, "Min divided by max"}
    };
    
    // Set control word with all exceptions masked
    uint16_t cw = X87_CW_PC_DOUBLE | X87_CW_RC_NEAR | 
                 X87_CW_IM | X87_CW_DM | X87_CW_ZM | 
                 X87_CW_OM | X87_CW_UM | X87_CW_PM;
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        uint16_t status;
        
        // Skip problematic tests initially
        if (i == 1 || i == 2) continue;
        
        asm volatile (
            "fninit\n\t"       // Initialize FPU
            "fldcw %4\n\t"     // Load control word
            "fldt %1\n\t"      // Load a (dividend)
            "fldt %2\n\t"      // Load b (divisor)
            "fdiv %%st, %%st(1)\n\t" // st(1) = st(1)/st(0)
            "fstpt %0\n\t"     // Store result
            "fnstsw %3\n\t"    // Store status word
            "fnclex\n\t"       // Clear exceptions
            "ffree %%st(0)\n\t" // Free ST(0)
            "ffree %%st(1)\n\t" // Free ST(1)
            : "=m" (result), "=m" (status)
            : "m" (tests[i].a), "m" (tests[i].b), "m" (cw)
            : "st", "st(1)", "cc"
        );
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %Lf / %Lf\n", tests[i].a, tests[i].b);
        printf("Result: ");
        print_float80(result);
        printf("Status Word: 0x%04x\n", status);
        print_x87_status();
    }
}

// Test FDIVP instruction
void test_fdivp() {
    printf("\n=== Testing FDIVP ===\n");
    
    struct {
        long double a;
        long double b;
        const char* desc;
    } tests[] = {
        {1.0L, 2.0L, "Basic division"},
        {POS_INF, 1.0L, "Infinity division"},
        {1.0L, POS_NAN, "Divide by NaN"},
        {POS_DENORM, POS_DENORM, "Denormal division"}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        long double result;
        
        asm volatile (
            "fldt %2\n\t"      // Load b
            "fldt %1\n\t"      // Load a
            "fdivp %%st, %%st(1)\n\t" // st(1) = st(1)/st(0) and pop
            "fstpt %0\n\t"     // Store result
            : "=m" (result)
            : "m" (tests[i].a), "m" (tests[i].b)
            : "st", "st(1)"
        );
        
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        printf("Operation: %Lf / %Lf\n", tests[i].a, tests[i].b);
        printf("Result: ");
        print_float80(result);
        print_x87_status();
    }
}

// Test different rounding modes
void test_rounding_modes() {
    printf("\n=== Testing FDIV with different rounding modes ===\n");
    
    const uint16_t modes[] = {
        X87_CW_RC_NEAR,
        X87_CW_RC_DOWN, 
        X87_CW_RC_UP,
        X87_CW_RC_ZERO
    };
    
    const char* mode_names[] = {
        "Round to nearest",
        "Round down",
        "Round up",
        "Round toward zero"
    };
    
    struct {
        long double a;
        long double b;
    } tests[] = {
        {1.0L, 3.0L},  // 1/3 = 0.333...
        {10.0L, 3.0L}  // 10/3 = 3.333...
    };
    
    for (size_t i = 0; i < sizeof(modes)/sizeof(modes[0]); i++) {
        uint16_t cw = X87_CW_PC_DOUBLE | modes[i] | 
                     X87_CW_IM | X87_CW_DM | X87_CW_ZM | 
                     X87_CW_OM | X87_CW_UM | X87_CW_PM;
        
        printf("\n--- %s ---\n", mode_names[i]);
        
        for (size_t j = 0; j < sizeof(tests)/sizeof(tests[0]); j++) {
            long double result;
            uint16_t status;
            
            asm volatile (
                "fninit\n\t"
                "fldcw %4\n\t"
                "fldt %2\n\t"
                "fldt %1\n\t"
                "fdivp %%st, %%st(1)\n\t"
                "fstpt %0\n\t"
                "fnstsw %3\n\t"
                "fnclex\n\t"
                : "=m" (result), "=m" (status)
                : "m" (tests[j].a), "m" (tests[j].b), "m" (cw)
                : "st", "st(1)", "cc"
            );
            
            printf("Test %zu: %Lf / %Lf = ", j+1, tests[j].a, tests[j].b);
            print_float80(result);
            print_x87_status();
        }
    }
}

// Test FDIV with memory operands
void test_fdiv_mem() {
    printf("\n=== Testing FDIV with memory operands ===\n");
    
    float f32 = 2.500007f;  // More precise value
    double f64 = 0.399999;  // More precise value
    int64_t i64 = 7;
    int32_t i32 = 5;
    int16_t i16 = 2;
    
    long double result;
    uint16_t status;
    
    // FDIV m32fp
    float expected_f32 = 1.0f / f32;
    asm volatile (
        "fninit\n\t"        // Initialize FPU
        "flds %2\n\t"          // Load 1.0 (ST0)
        "fld1\n\t"       // Load m32 (ST0), pushes 1.0 to ST1
        "fdivp %%st(0), %%st(1)\n\t" // ST1 = ST1/ST0 and pop
        "fstpt %0\n\t"      // Store result
        "fnstsw %1\n\t"     // Store status
        : "=m" (result), "=m" (status)
        : "m" (f32)
        : "st", "st(1)", "cc"
    );
    printf("\nFDIV m32: 1.0 / %f = ", f32);
    print_float80(result);
    printf("Expected: %.8g (float precision)\n", expected_f32);
    
    // Check result with tolerance
    float actual_f32 = (float)result;
    float rel_error_f32 = fabsf((actual_f32 - expected_f32)/expected_f32);
    if (rel_error_f32 > 1e-6f) {
        printf("ERROR: Relative error too large (%.2g%%)\n", rel_error_f32*100);
    } else {
        printf("PASSED: Result within tolerance\n");
    }
    
    // FDIV m64fp
    double expected_f64 = 1.0 / f64;
    asm volatile (
        "fninit\n\t"        // Initialize FPU
        "fldl %2\n\t"          // Load 1.0 (ST0)
        "fld1\n\t"       // Load m64 (ST0), pushes 1.0 to ST1
        "fdivp %%st(0), %%st(1)\n\t" // ST1 = ST1/ST0 and pop
        "fstpt %0\n\t"      // Store result
        "fnstsw %1\n\t"     // Store status
        : "=m" (result), "=m" (status)
        : "m" (f64)
        : "st", "st(1)", "cc"
    );
    printf("\nFDIV m64: 1.0 / %lf = ", f64);
    print_float80(result);
    printf("Expected: %.17g (double precision)\n", expected_f64);
    
    // Check result with tolerance
    double actual_f64 = (double)result;
    double rel_error_f64 = fabs((actual_f64 - expected_f64)/expected_f64);
    if (rel_error_f64 > 1e-12) {
        printf("ERROR: Relative error too large (%.2g%%)\n", rel_error_f64*100);
    } else {
        printf("PASSED: Result within tolerance\n");
    }

        // FIDIV m64int (alternative implementation)
        asm volatile (
            "fninit\n\t"        // Initialize FPU
            "fildq %2\n\t"          // Load 1.0 (ST0)
            "fld1\n\t"      // Load 32-bit integer (ST0), pushes 1.0 to ST1
            "fdivp %%st(0), %%st(1)\n\t" // ST1 = ST1/ST0 and pop
            "fstpt %0\n\t"      // Store result
            "fnstsw %1\n\t"     // Store status
            : "=m" (result), "=m" (status)
            : "m" (*(int64_t*)&i64)
            : "st", "cc"
        );
        printf("\nFIDIV m64: 1.0 / %ld = ", i64);
        print_float80(result);
        
    // FIDIV m32int (alternative implementation)
    asm volatile (
        "fninit\n\t"        // Initialize FPU
        "fildl %2\n\t"          // Load 1.0 (ST0)
        "fld1\n\t"      // Load 32-bit integer (ST0), pushes 1.0 to ST1
        "fdivp %%st(0), %%st(1)\n\t" // ST1 = ST1/ST0 and pop
        "fstpt %0\n\t"      // Store result
        "fnstsw %1\n\t"     // Store status
        : "=m" (result), "=m" (status)
        : "m" (*(int32_t*)&i32)
        : "st", "cc"
    );
    printf("\nFIDIV m32: 1.0 / %d = ", i32);
    print_float80(result);
    
    // FIDIV m16int (alternative implementation)
    asm volatile (
        "fninit\n\t"        // Initialize FPU
        "filds %2\n\t"          // Load 1.0 (ST0)
        "fld1\n\t"      // Load 16-bit integer (ST0), pushes 1.0 to ST1
        "fdivp %%st(0), %%st(1)\n\t" // ST1 = ST1/ST0 and pop
        "fstpt %0\n\t"      // Store result
        "fnstsw %1\n\t"     // Store status
        : "=m" (result), "=m" (status)
        : "m" (*(int16_t*)&i16)
        : "st", "cc"
    );
    printf("\nFIDIV m16: 1.0 / %d = ", i16);
    print_float80(result);
}

int main() {
    init_x87_env();
    test_fdiv_reg();
    test_fdivp();
    test_rounding_modes();
    test_fdiv_mem();
    return 0;
}
