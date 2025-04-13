#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

#define ALIGN16 __attribute__((aligned(16)))
#define ALIGN8  __attribute__((aligned(8)))

typedef enum {
    ROUND_NEAREST = 0x0000,
    ROUND_DOWN    = 0x2000,
    ROUND_UP      = 0x4000,
    ROUND_TO_ZERO = 0x6000
} RoundingMode;

typedef struct {
    float input[2];
    int32_t expected[2];
    const char *description;
} CVTPS2PITestCase;

static void set_rounding_mode(RoundingMode mode) {
    uint32_t mxcsr;
    __asm__ __volatile__ ("stmxcsr %0" : "=m"(mxcsr));
    mxcsr = (mxcsr & ~0x6000) | mode;
    __asm__ __volatile__ ("ldmxcsr %0" : : "m"(mxcsr));
}

static void test_cvtps2pi(const CVTPS2PITestCase* tc) {
    __m128 input = _mm_set_ps(0, 0, tc->input[1], tc->input[0]);
    int32_t output[2];
    
    __m64 res;
    asm volatile (
        "cvtps2pi %1, %0\n\t"
        : "=y" (res)
        : "x" (input)
    );
    output[0] = ((int32_t*)&res)[0];
    output[1] = ((int32_t*)&res)[1];
    
    printf("Test: %s\n", tc->description);
    printf("Input: [%f, %f]\n", tc->input[0], tc->input[1]);
    printf("Expected: [%d, %d]\n", tc->expected[0], tc->expected[1]);
    printf("Actual: [%d, %d]\n", output[0], output[1]);
    printf("Result: %s\n\n", 
          (output[0] == tc->expected[0] && output[1] == tc->expected[1]) ? "PASS" : "FAIL");
    
    _mm_empty();
}

int main() {
    const CVTPS2PITestCase tests[] = {
        {
            .input = {1.5f, -1.5f},
            .expected = {2, -2},
            .description = "Mid-point rounding test"
        },
        {
            .input = {2147483648.0f, -2147483648.0f},
            .expected = {0x80000000, 0x80000000},
            .description = "32-bit boundary test"
        },
        {
            .input = {3.14159f, -2.71828f},
            .expected = {3, -3},
            .description = "Fractional values"
        }
    };

    const int num_tests = sizeof(tests)/sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++) {
        printf("\n[Test Case %d/%d]\n", i+1, num_tests);
        test_cvtps2pi(&tests[i]);
    }

    return 0;
}
