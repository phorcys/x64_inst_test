#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <immintrin.h>

#define ALIGN16 __attribute__((aligned(16)))
#define ALIGN8  __attribute__((aligned(8)))

typedef struct {
    float input[2];
    int32_t expected[2];
    const char *description;
} CVTTPS2PITestCase;

static void print_results(const CVTTPS2PITestCase* tc, const int32_t* actual) {
    printf("Test: %s\n", tc->description);
    printf("Input: [%f, %f]\n", tc->input[0], tc->input[1]);
    printf("Expected: [%d, %d]\n", tc->expected[0], tc->expected[1]);
    printf("Actual: [%d, %d]\n", actual[0], actual[1]);
    printf("Result: %s\n\n", 
          (actual[0] == tc->expected[0] && actual[1] == tc->expected[1]) ? "PASS" : "FAIL");
}

static void test_cvttps2pi(const CVTTPS2PITestCase* tc) {
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
    print_results(tc, output);
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
    print_results(tc, output);
    _mm_empty();
}

int main() {
    const CVTTPS2PITestCase tests[] = {
        {
            .input = {1.5f, -1.5f},
            .expected = {1, -1}, // 截断结果
            .description = "Truncation test"
        },
        {
            .input = {3.14159f, -2.71828f},
            .expected = {3, -2},
            .description = "Fractional values"
        },
        {
            .input = {2147483647.5f, -2147483648.5f},
            .expected = {2147483647, -2147483648},
            .description = "32-bit boundary"
        }
    };

    const int num_tests = sizeof(tests)/sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++) {
        printf("\n[Test Case %d/%d]\n", i+1, num_tests);
        test_cvttps2pi(&tests[i]);
    }

    return 0;
}
