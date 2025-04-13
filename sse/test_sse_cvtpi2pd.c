#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define ALIGN16 __attribute__((aligned(16)))
#define ALIGN8  __attribute__((aligned(8)))

typedef struct {
    int32_t a;
    int32_t b;
} TestCase;

static bool compare_double(double a, double b) {
    const double epsilon = 1e-12;
    if (a == b) return true;
    double abs_diff = fabs(a - b);
    double abs_a = fabs(a);
    double abs_b = fabs(b);
    double max_val = (abs_a > abs_b) ? abs_a : abs_b;
    return (abs_diff <= epsilon * max_val) || (abs_diff < epsilon);
}

static void print_results(const char* type, int32_t a, int32_t b,
                         const double* actual, const double* expected) {
    printf("%s Test - Input: [%d, %d]\n", type, a, b);
    printf("  Expected: [%22.15g, %22.15g]\n", expected[0], expected[1]);
    printf("  Actual  : [%22.15g, %22.15g]\n", actual[0], actual[1]);
    
    bool match = compare_double(actual[0], expected[0]) && 
                compare_double(actual[1], expected[1]);
    
    printf("  Result  : %s\n\n", match ? "PASS" : "FAIL");
}

static void test_reg_operand(const TestCase* tc) {
    int32_t input[2] ALIGN8 = {tc->a, tc->b};
    double output[2] ALIGN16 = {0};
    double expected[2] = {(double)tc->a, (double)tc->b};

    __asm__ __volatile__ (
        "pxor     %%xmm0, %%xmm0\n\t"
        "movq     %[in], %%mm0\n\t"
        "cvtpi2pd %%mm0, %%xmm0\n\t"
        "movapd   %%xmm0, %[out]\n\t"
        "emms\n\t"
        : [out] "=m" (*output)
        : [in] "m" (*((const int64_t*)input))
        : "mm0", "xmm0", "memory"
    );

    print_results("Register", tc->a, tc->b, output, expected);
}

static void test_mem_operand(const TestCase* tc) {
    int64_t input ALIGN8 = ((int64_t)tc->b << 32) | (uint32_t)tc->a;
    double output[2] ALIGN16 = {0};
    double expected[2] = {(double)tc->a, (double)tc->b};

    __asm__ __volatile__ (
        "pxor     %%xmm0, %%xmm0\n\t"
        "cvtpi2pd %[in], %%xmm0\n\t"
        "movapd   %%xmm0, %[out]\n\t"
        : [out] "=m" (*output)
        : [in] "m" (input)
        : "xmm0", "memory"
    );

    print_results("Memory  ", tc->a, tc->b, output, expected);
}

int main() {
    const TestCase tests[] = {
        {0, 0},                 // 全零
        {1, -1},                // 简单正负数
        {2147483647, -2147483648}, // 最大正/负32位整数
        {0x12345678, 0x9ABCDEF0}, // 随机测试值
        {65535, -32768},        // 常见边界值
        {0, -0},                // 零的不同表示
        {16777216, -16777216},  // 单精度精确边界
        {0, 0} // 原测试值会导致32位溢出，已替换为安全值
    };

    const int num_tests = sizeof(tests)/sizeof(tests[0]);
    
    for (int i = 0; i < num_tests; i++) {
        printf("\nTest Case %d/%d\n", i+1, num_tests);
        test_reg_operand(&tests[i]);
        test_mem_operand(&tests[i]);
    }

    return 0;
}
