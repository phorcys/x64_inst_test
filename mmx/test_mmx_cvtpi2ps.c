#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define ALIGN16 __attribute__((aligned(16)))
#define ALIGN8  __attribute__((aligned(8)))

// 舍入模式枚举
typedef enum {
    ROUND_NEAREST = 0x0000,
    ROUND_DOWN    = 0x2000,
    ROUND_UP      = 0x4000,
    ROUND_TO_ZERO = 0x6000
} RoundingMode;

// CVTPI2PS测试结构体
typedef struct {
    int32_t a;
    int32_t b;
    float expected[8]; // 按顺序存储4种舍入模式的预期结果[NEAREST, DOWN, UP, ZERO]
    const char* description;
} CVTPI2PSTestCase;

// MXCSR控制函数
static void set_rounding_mode(RoundingMode mode) {
    uint32_t mxcsr;
    __asm__ __volatile__ ("stmxcsr %0" : "=m"(mxcsr));
    mxcsr = (mxcsr & ~0x6000) | mode;
    __asm__ __volatile__ ("ldmxcsr %0" : : "m"(mxcsr));
}

static RoundingMode get_rounding_mode() {
    uint32_t mxcsr;
    __asm__ __volatile__ ("stmxcsr %0" : "=m"(mxcsr));
    return (RoundingMode)(mxcsr & 0x6000);
}

static bool compare_float(float a, float b) {
    const float epsilon = 1e-6;
    if (a == b) return true;
    float abs_diff = fabsf(a - b);
    float abs_a = fabsf(a);
    float abs_b = fabsf(b);
    float max_val = (abs_a > abs_b) ? abs_a : abs_b;
    return (abs_diff <= epsilon * max_val) || (abs_diff < epsilon);
}

static void print_results(const CVTPI2PSTestCase* tc, const float* actual, RoundingMode mode) {
    const char* mode_str[] = {
        [ROUND_NEAREST] = "Near",
        [ROUND_DOWN]    = "Down",
        [ROUND_UP]      = "Up",
        [ROUND_TO_ZERO] = "Zero"
    };
    const uint32_t mode_index[] = {
        [ROUND_NEAREST] = 0,
        [ROUND_DOWN]    = 2,
        [ROUND_UP]      = 4,
        [ROUND_TO_ZERO] = 6
    };

    printf("Test: %s\n", tc->description);
    printf("Input: [%d, %d]\n", tc->a, tc->b);
    printf("Mode: %-5s Expected: [%15.6f, %15.6f]  Actual: [%15.6f, %15.6f]\n",
           mode_str[mode],
           tc->expected[mode_index[mode]],
           tc->expected[mode_index[mode] + 1],
           actual[0], actual[1]);
    
    bool match = compare_float(actual[0], tc->expected[mode_index[mode]]) && 
                compare_float(actual[1], tc->expected[mode_index[mode] + 1]);
    printf("Result: %s\n\n", match ? "PASS" : "FAIL");
}

static void test_cvtpi2ps(const CVTPI2PSTestCase* tc) {
    int32_t input[2] ALIGN8 = {tc->a, tc->b};
    float output[4] ALIGN16 = {0};
    RoundingMode old_mode = get_rounding_mode();
    int64_t input64;
    memcpy(&input64, input, sizeof(input64));

    // 测试所有四种舍入模式
    for (RoundingMode mode = ROUND_NEAREST; mode <= ROUND_TO_ZERO; mode += 0x2000) {
        set_rounding_mode(mode);
        
        // 寄存器版本
        __asm__ __volatile__ (
            "pxor     %%xmm0, %%xmm0\n\t"
            "movq     %[in], %%mm0\n\t"
            "cvtpi2ps %%mm0, %%xmm0\n\t"
            "movaps   %%xmm0, %[out]\n\t"
            "emms\n\t"
            : [out] "=m" (*output)
            : [in] "m" (input64)
            : "mm0", "xmm0", "memory"
        );
        print_results(tc, output, mode);

        // 内存版本
        __asm__ __volatile__ (
            "pxor     %%xmm0, %%xmm0\n\t"
            "cvtpi2ps %[in], %%xmm0\n\t"
            "movaps   %%xmm0, %[out]\n\t"
            : [out] "=m" (*output)
            : [in] "m" (input64)
            : "xmm0", "memory"
        );
        print_results(tc, output, mode);
    }

    set_rounding_mode(old_mode);
}

int main() {
    const CVTPI2PSTestCase tests[] = {
        {
            .a = 1,
            .b = -1,
            .expected = {
                /* Nearest */ 1.0f, -1.0f,
                /* Down    */ 1.0f, -1.0f,
                /* Up      */ 1.0f, -1.0f,
                /* Zero    */ 1.0f, -1.0f
            },
            .description = "Simple integers"
        },
        // {
        //     .a = 2147483647,
        //     .b = -2147483648,
        //     .expected = {
        //         /* Nearest */ 2147483648.0f, -2147483648.0f,
        //         /* Down    */ 2147483648.0f, -2147483648.0f,
        //         /* Up      */ 2147483648.0f, -2147483648.0f,
        //         /* Zero    */ 2147483648.0f, -2147483648.0f
        //     },
        //     .description = "32-bit integer boundary"
        // },
        {
            .a = 16777216,
            .b = -16777216,
            .expected = {
                /* Nearest */ 16777216.0f, -16777216.0f,
                /* Down    */ 16777216.0f, -16777216.0f,
                /* Up      */ 16777216.0f, -16777216.0f,
                /* Zero    */ 16777216.0f, -16777216.0f
            },
            .description = "Single precision boundary"
        }
    };

    const int num_tests = sizeof(tests)/sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++) {
        printf("\n[Test Case %d/%d]\n", i+1, num_tests);
        test_cvtpi2ps(&tests[i]);
    }

    return 0;
}
