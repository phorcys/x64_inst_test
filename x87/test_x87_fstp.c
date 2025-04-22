#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fenv.h>
#include <math.h>
#include <float.h>

#define TEST_CASE(name, condition) \
    do { \
        printf("[TEST] %-40s %s\n", name, (condition) ? "PASS" : "FAIL"); \
        if (!(condition)) { \
            printf("  [FAIL DETAIL] %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

void print_float(const char* name, float value) {
    printf("%s: ", name);
    if (isnan(value)) {
        printf("%sNaN\n", signbit(value) ? "-" : "+");
    } else if (isinf(value)) {
        printf("%sINF\n", signbit(value) ? "-" : "+");
    } else {
        printf("%.15g\n", value);
    }
}

void test_fstp() {
    float f32_values[] = {
        0.0f, -0.0f,
        1.0f, -1.0f,
        123.456f, -123.456f,
        FLT_MAX, -FLT_MAX,
        FLT_MIN, -FLT_MIN,
        INFINITY, -INFINITY,
        NAN, -NAN
    };

    printf("\n==== Testing FSTP (float) ====\n");
    for (size_t i = 0; i < sizeof(f32_values)/sizeof(f32_values[0]); i++) {
        float value = f32_values[i];
        float stored_value;
        
        __asm__ volatile (
            "fninit\n\t"     // Initialize FPU
            "flds %1\n\t"    // Load value
            "fstps %0\n\t"   // Store value and pop
            : "=m" (stored_value)
            : "m" (value)
            : "st"
        );

        print_float("Original", value);
        print_float("Stored", stored_value);
        
        if (isnan(value) && isnan(stored_value)) {
            TEST_CASE("NaN value stored correctly", 1);
        } else {
            TEST_CASE("Value stored correctly", value == stored_value);
        }
    }

    // Test stack behavior (FSTP should pop)
    printf("\n==== Testing FSTP stack behavior ====\n");
    uint16_t fpu_status;
    
    // Get initial FPU state
    __asm__ volatile ("fninit\n\t");
    __asm__ volatile ("fld1\n\t");  // Push value to stack
    __asm__ volatile ("fnstsw %0" : "=m" (fpu_status));
    int initial_top = (fpu_status >> 11) & 0x7;
    
    float temp = 1.0f;
    __asm__ volatile (
        "fstps %0\n\t"   // Store value and pop
        : "=m" (temp)
        : : "st");
    
    // Get final FPU state
    __asm__ volatile ("fnstsw %0" : "=m" (fpu_status));
    int new_top = (fpu_status >> 11) & 0x7;
    
    printf("Initial TOP: %d, New TOP: %d\n", initial_top, new_top);
    printf("FPU Status Word: 0x%04x\n", fpu_status);
    
    // Check stack pointer was incremented (popped)
    TEST_CASE("Stack pointer incremented (popped)", 
             new_top == ((initial_top + 1) % 8));
}

int main() {
    fesetround(FE_TONEAREST);
    feclearexcept(FE_ALL_EXCEPT);

    test_fstp();

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
