#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <immintrin.h>

#define TEST_DPPS(a_val1, a_val2, a_val3, a_val4, b_val1, b_val2, b_val3, b_val4, imm8, expect1, expect2, expect3, expect4) \
    do { \
        printf("\nTest case: imm8=0x%02X\n", imm8); \
        float a[4] = {a_val1, a_val2, a_val3, a_val4}; \
        float b[4] = {b_val1, b_val2, b_val3, b_val4}; \
        float expect[4] = {expect1, expect2, expect3, expect4}; \
        __m128 a_vec = _mm_loadu_ps(a); \
        __m128 b_vec = _mm_loadu_ps(b); \
        __m128 res; \
        __asm__ volatile ( \
            "movaps %1, %%xmm0\n\t" \
            "movaps %2, %%xmm1\n\t" \
            "dpps $"#imm8", %%xmm1, %%xmm0\n\t" \
            "movaps %%xmm0, %0\n\t" \
            : "=x" (res) \
            : "x" (a_vec), "x" (b_vec) \
            : "xmm0", "xmm1" \
        ); \
        float result[4]; \
        memcpy(result, &res, sizeof(result)); \
        printf("A: [%.2f, %.2f, %.2f, %.2f]\n", a[0], a[1], a[2], a[3]); \
        printf("B: [%.2f, %.2f, %.2f, %.2f]\n", b[0], b[1], b[2], b[3]); \
        printf("Result: [%.2f, %.2f, %.2f, %.2f]\n", result[0], result[1], result[2], result[3]); \
        printf("Expect: [%.2f, %.2f, %.2f, %.2f]\n", expect[0], expect[1], expect[2], expect[3]); \
        int passed = 1; \
        for (int i = 0; i < 4; i++) { \
            if (isinf(result[i]) != isinf(expect[i]) || \
                isnan(result[i]) != isnan(expect[i]) || \
                (!isinf(result[i]) && !isnan(result[i]) && fabs(result[i] - expect[i]) > 1e-6)) { \
                passed = 0; \
                break; \
            } \
        } \
        printf("Test %s\n", passed ? "PASSED" : "FAILED"); \
    } while(0)

void test_dpps() {
    printf("Testing DPPS instruction\n");
    printf("-----------------------\n");
    
    // 测试各种imm8组合
    TEST_DPPS(1.0f, 2.0f, 3.0f, 4.0f, 0.5f, 1.5f, 2.5f, 3.5f, 0xF1, 25.0f, 0.0f, 0.0f, 0.0f);  // 计算所有4个分量点积
    TEST_DPPS(1.0f, 2.0f, 3.0f, 4.0f, 0.5f, 1.5f, 2.5f, 3.5f, 0x11, 0.5f, 0.0f, 0.0f, 0.0f);    // 仅第一个分量
    TEST_DPPS(1.0f, 2.0f, 3.0f, 4.0f, 0.5f, 1.5f, 2.5f, 3.5f, 0x33, 3.5f, 3.5f, 0.0f, 0.0f);    // 前两个分量
    TEST_DPPS(1.0f, 2.0f, 3.0f, 4.0f, 0.5f, 1.5f, 2.5f, 3.5f, 0x77, 11.0f, 11.0f, 11.0f, 0.0f); // 前三个分量
    
    // 测试边界条件
    TEST_DPPS(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0xF1, 0.0f, 0.0f, 0.0f, 0.0f);
    TEST_DPPS(1e30f, 1e30f, 1e30f, 1e30f, 1e30f, 1e30f, 1e30f, 1e30f, 0xF1, INFINITY, 0.0f, 0.0f, 0.0f);
    TEST_DPPS(NAN, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0xF1, NAN, 0.0f, 0.0f, 0.0f);
    TEST_DPPS(1e-30f, 1e-30f, 1e-30f, 1e-30f, 1e-30f, 1e-30f, 1e-30f, 1e-30f, 0xF1, 0.0f, 0.0f, 0.0f, 0.0f);
}

int main() {
    test_dpps();
    return 0;
}
