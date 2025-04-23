#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <immintrin.h>

#define TEST_DPPD(a_val1, a_val2, b_val1, b_val2, imm8, expect1, expect2) \
    do { \
        printf("\nTest case: imm8=0x%02X\n", imm8); \
        double a[2] = {a_val1, a_val2}; \
        double b[2] = {b_val1, b_val2}; \
        double expect[2] = {expect1, expect2}; \
        __m128d a_vec = _mm_loadu_pd(a); \
        __m128d b_vec = _mm_loadu_pd(b); \
        __m128d res; \
        __asm__ volatile ( \
            "movapd %1, %%xmm0\n\t" \
            "movapd %2, %%xmm1\n\t" \
            "dppd $"#imm8", %%xmm1, %%xmm0\n\t" \
            "movapd %%xmm0, %0\n\t" \
            : "=x" (res) \
            : "x" (a_vec), "x" (b_vec) \
            : "xmm0", "xmm1" \
        ); \
        double result[2]; \
        memcpy(result, &res, sizeof(result)); \
        printf("A: [%.2f, %.2f]\n", a[0], a[1]); \
        printf("B: [%.2f, %.2f]\n", b[0], b[1]); \
        printf("Result: [%.2f, %.2f]\n", result[0], result[1]); \
        printf("Expect: [%.2f, %.2f]\n", expect[0], expect[1]); \
        int passed = 1; \
        for (int i = 0; i < 2; i++) { \
            if (isinf(result[i]) != isinf(expect[i]) || \
                isnan(result[i]) != isnan(expect[i]) || \
                (!isinf(result[i]) && !isnan(result[i]) && fabs(result[i] - expect[i]) > 1e-10)) { \
                passed = 0; \
                break; \
            } \
        } \
        printf("Test %s\n", passed ? "PASSED" : "FAILED"); \
    } while(0)

// 测试DPPD指令(双精度浮点条件点积)
void test_dppd() {
    printf("Testing DPPD instruction\n");
    printf("-----------------------\n");
    
    // 测试各种imm8组合
    TEST_DPPD(1.0, 2.0, 3.0, 4.0, 0x31, 11.0, 0.0);  // 1*3 + 2*4
    TEST_DPPD(1.5, 2.5, 3.5, 4.5, 0x31, 16.50, 0.0);
    TEST_DPPD(-1.0, 2.0, 3.0, -4.0, 0x31, -11.0, 0.0);
    TEST_DPPD(1.0, 2.0, 3.0, 4.0, 0x30, 0.0, 0.0);    // 仅第一个乘积(imm8=0x30不计算任何乘积)
    TEST_DPPD(1.0, 2.0, 3.0, 4.0, 0x21, 8.0, 0.0);    // 仅第二个乘积
    TEST_DPPD(1.0, 2.0, 3.0, 4.0, 0x11, 3.0, 0.0);    // 仅第一个乘积(imm8=0x11计算第一个乘积)
    
    // 测试边界条件
    TEST_DPPD(0.0, 0.0, 0.0, 0.0, 0x31, 0.0, 0.0);
    TEST_DPPD(1e300, 1e300, 1e300, 1e300, 0x31, INFINITY, 0.0);
    TEST_DPPD(1e-300, 1e-300, 1e-300, 1e-300, 0x31, 0.0, 0.0);
}

int main() {
    test_dppd();
    return 0;
}
