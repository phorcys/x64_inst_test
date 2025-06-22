#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// VDIVSS指令测试函数
static void test_vdivss() {
    printf("=== Testing VDIVSS ===\n");

    // 测试用例数组
    struct TestCase {
        float a;
        float b;
        float expected;
        const char* desc;
    } test_cases[] = {
        {10.0f, 2.0f, 5.0f, "10.0 / 2.0 = 5.0"},
        {1.0f, 4.0f, 0.25f, "1.0 / 4.0 = 0.25"},
        {0.0f, 1.0f, 0.0f, "0.0 / 1.0 = 0.0"},
        {-10.0f, 2.0f, -5.0f, "-10.0 / 2.0 = -5.0"},
        {1.0f, 0.0f, INFINITY, "1.0 / 0.0 = INF"},
        {0.0f, 0.0f, NAN, "0.0 / 0.0 = NAN"},
        {INFINITY, 2.0f, INFINITY, "INF / 2.0 = INF"},
        {1.0f, INFINITY, 0.0f, "1.0 / INF = 0.0"},
        {NAN, 2.0f, NAN, "NAN / 2.0 = NAN"},
        {1.0f, NAN, NAN, "1.0 / NAN = NAN"}
    };

    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    int passed = 0;

    for (int i = 0; i < num_tests; i++) {
        float a = test_cases[i].a;
        float b = test_cases[i].b;
        float expected = test_cases[i].expected;
        float result;

        // 使用内联汇编执行VDIVSS指令
        __asm__ __volatile__ (
            "vdivss %[b], %[a], %[res]\n\t"
            : [res] "=x" (result)
            : [a] "x" (a),
              [b] "x" (b)
        );

        // 比较结果
        int is_equal;
        if (isnan(expected)) {
            is_equal = isnan(result);
        } else if (isinf(expected)) {
            is_equal = isinf(result) && (signbit(expected) == signbit(result));
        } else {
            is_equal = float_equal(result, expected, 0.0001f);
        }

        // 打印测试结果
        printf("Test %d: %s\n", i+1, test_cases[i].desc);
        printf("  Operands: a=%f, b=%f\n", a, b);
        printf("  Expected: %f (0x%08X)\n", expected, *(uint32_t*)&expected);
        printf("  Result:   %f (0x%08X)\n", result, *(uint32_t*)&result);
        printf("  %s\n", is_equal ? "PASS" : "FAIL");

        if (is_equal) passed++;
    }

    printf("\nSummary: %d/%d tests passed\n\n", passed, num_tests);
}

// 主函数
int main() {
    test_vdivss();
    return 0;
}
