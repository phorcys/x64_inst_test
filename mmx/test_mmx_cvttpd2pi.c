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

// CVTTPD2PI测试结构体
typedef struct {
    double input_a;
    double input_b;
    int32_t expected[8]; // 按顺序存储4种舍入模式的预期结果[NEAREST, DOWN, UP, ZERO]
    const char* description;
} CVTTPD2PITestCase;

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

static void print_results(const CVTTPD2PITestCase* tc, const int32_t* actual, RoundingMode mode) {
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
    printf("Input: [%20.10g, %20.10g]\n", tc->input_a, tc->input_b);
    printf("Mode: %-5s Expected: [%12d, %12d]  Actual: [%12d, %12d]\n",
           mode_str[mode],
           tc->expected[mode_index[mode]],
           tc->expected[mode_index[mode] + 1],
           actual[0], actual[1]);
    
    bool match = (actual[0] == tc->expected[mode_index[mode]]) && 
                (actual[1] == tc->expected[mode_index[mode] + 1]);
    printf("Result: %s\n\n", match ? "PASS" : "FAIL");
}

static void test_cvttpd2pi(const CVTTPD2PITestCase* tc) {
    double input[2] ALIGN16 = {tc->input_a, tc->input_b};
    int32_t output[2] ALIGN8 = {0};
    RoundingMode old_mode = get_rounding_mode();

    // 测试所有四种舍入模式
    for (RoundingMode mode = ROUND_NEAREST; mode <= ROUND_TO_ZERO; mode += 0x2000) {
        set_rounding_mode(mode);
        
        // 寄存器版本
        __asm__ __volatile__ (
            "movapd   %[in], %%xmm0\n\t"
            "cvttpd2pi %%xmm0, %%mm0\n\t"
            "movq     %%mm0, %[out]\n\t"
            "emms\n\t"
            : [out] "=m" (*output)
            : [in] "m" (*input)
            : "xmm0", "mm0", "memory"
        );
        print_results(tc, output, mode);

        // 内存版本
        __asm__ __volatile__ (
            "cvttpd2pi %[in], %%mm0\n\t"
            "movq     %%mm0, %[out]\n\t"
            "emms\n\t"
            : [out] "=m" (*output)
            : [in] "m" (*input)
            : "mm0", "memory"
        );
        print_results(tc, output, mode);
    }

    set_rounding_mode(old_mode);
}

int main() {
    const CVTTPD2PITestCase tests[] = {
        {
            .input_a = 1.999,
            .input_b = -1.999,
            .expected = {
                /* CVTTPD2PI总是截断 */ 1, -1,
                                        1, -1,
                                        1, -1,
                                        1, -1
            },
            .description = "Truncation test"
        },
        {
            .input_a = 3.14159,
            .input_b = -2.71828,
            .expected = {
                /* 所有模式相同 */ 3, -2,
                                  3, -2,
                                  3, -2,
                                  3, -2
            },
            .description = "Fractional values"
        },
        {
            .input_a = 2147483647.999,
            .input_b = -2147483648.999,
            .expected = {
                /* 截断到32位边界 */ 2147483647, -2147483648,
                                        2147483647, -2147483648,
                                        2147483647, -2147483648,
                                        2147483647, -2147483648
            },
            .description = "32-bit integer boundary"
        },
        {
            .input_a = INFINITY,
            .input_b = -INFINITY,
            .expected = {
                /* 特殊值处理 */ 0x80000000, 0x80000000,
                                  0x80000000, 0x80000000,
                                  0x80000000, 0x80000000,
                                  0x80000000, 0x80000000
            },
            .description = "Infinity handling"
        },
        {
            .input_a = NAN,
            .input_b = -NAN,
            .expected = {
                /* NaN处理 */ 0x80000000, 0x80000000,
                                0x80000000, 0x80000000,
                                0x80000000, 0x80000000,
                                0x80000000, 0x80000000
            },
            .description = "NaN handling"
        }
    };

    const int num_tests = sizeof(tests)/sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++) {
        printf("\n[Test Case %d/%d]\n", i+1, num_tests);
        test_cvttpd2pi(&tests[i]);
    }

    return 0;
}
