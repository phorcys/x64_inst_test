#include <stdio.h>
#include <stdint.h>
#include <math.h>
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
    int32_t expected[4][2]; // Expected results for each rounding mode
    const char *description;
} CVTTPS2PITestCase;

static void set_rounding_mode(RoundingMode mode) {
    uint32_t mxcsr;
    __asm__ __volatile__ ("stmxcsr %0" : "=m"(mxcsr));
    mxcsr = (mxcsr & ~0x6000) | mode;
    __asm__ __volatile__ ("ldmxcsr %0" : : "m"(mxcsr));
}

static void print_results(const CVTTPS2PITestCase* tc, const int32_t* actual, RoundingMode mode) {
    const char* mode_str[] = {"NEAREST", "DOWN", "UP", "TO_ZERO"};
    printf("Test: %s [%s]\n", tc->description, mode_str[mode >> 13]);
    printf("Input: [%f, %f]\n", tc->input[0], tc->input[1]);
    printf("Expected: [%d, %d]\n", tc->expected[mode >> 13][0], tc->expected[mode >> 13][1]);
    printf("Actual: [%d, %d]\n", actual[0], actual[1]);
    printf("Result: %s\n\n", 
          (actual[0] == tc->expected[mode >> 13][0] && 
           actual[1] == tc->expected[mode >> 13][1]) ? "PASS" : "FAIL");
}

static void test_cvttps2pi(const CVTTPS2PITestCase* tc, RoundingMode mode) {
    set_rounding_mode(mode);
    
    __m128 input = _mm_set_ps(0, 0, tc->input[1], tc->input[0]);
    int32_t output[2];
    
    // 寄存器版本
    __m64 res;
    asm volatile (
        "cvttps2pi %1, %0\n\t"
        : "=y" (res)
        : "x" (input)
    );
    output[0] = ((int32_t*)&res)[0];
    output[1] = ((int32_t*)&res)[1];
    print_results(tc, output, mode);
    _mm_empty();

    // 内存版本
    float mem_input[2] = {tc->input[0], tc->input[1]};
    asm volatile (
        "cvttps2pi (%1), %0\n\t"
        : "=y" (res)
        : "r" (mem_input)
    );
    output[0] = ((int32_t*)&res)[0];
    output[1] = ((int32_t*)&res)[1];
    print_results(tc, output, mode);
    _mm_empty();
}

int main() {
    const CVTTPS2PITestCase tests[] = {
        {
            .input = {1.5f, -1.5f},
            .expected = {
                {1, -1},    // NEAREST (should truncate)
                {1, -1},     // DOWN
                {1, -1},    // UP
                {1, -1}      // TO_ZERO
            },
            .description = "Mid-point truncation test"
        },
        {
            .input = {3.14159f, -2.71828f},
            .expected = {
                {3, -2},     // NEAREST
                {3, -2},     // DOWN
                {3, -2},     // UP
                {3, -2}      // TO_ZERO
            },
            .description = "Fractional values"
        },
        {
            .input = {2147483647.5f, -2147483648.5f},
            .expected = {
                {0x80000000, 0x80000000},
                {0x80000000, 0x80000000},
                {0x80000000, 0x80000000},
                {0x80000000, 0x80000000}
            },
            .description = "32-bit boundary test (saturation)"
        },
        {
            .input = {NAN, -INFINITY},
            .expected = {
                {0x80000000, 0x80000000},
                {0x80000000, 0x80000000},
                {0x80000000, 0x80000000},
                {0x80000000, 0x80000000}
            },
            .description = "NaN and infinity test"
        }
    };

    const int num_tests = sizeof(tests)/sizeof(tests[0]);
    const RoundingMode modes[] = {ROUND_NEAREST, ROUND_DOWN, ROUND_UP, ROUND_TO_ZERO};

    for (int i = 0; i < num_tests; i++) {
        printf("\n[Test Case %d/%d]\n", i+1, num_tests);
        for (int j = 0; j < 4; j++) {
            test_cvttps2pi(&tests[i], modes[j]);
        }
    }

    return 0;
}
