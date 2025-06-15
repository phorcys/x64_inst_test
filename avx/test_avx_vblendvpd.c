#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

static void test_vblendvpd() {
    printf("--- Testing vblendvpd (variable blend packed double-precision floats) ---\n");

    // 测试数据
    double src1[4] ALIGNED(32) = {1.1, 2.2, 3.3, 4.4};
    double src2[4] ALIGNED(32) = {5.5, 6.6, 7.7, 8.8};
    double mask[4] ALIGNED(32) = {0.0, -1.0, 0.0, -1.0}; // 掩码: 0表示选src1, -1表示选src2
    double result[4] ALIGNED(32) = {0};

    // 基本测试
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vmovapd %3, %%ymm2\n\t"
        "vblendvpd %%ymm2, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2), "m"(mask)
        : "ymm0", "ymm1", "ymm2"
    );
    printf("--- Basic Test ---\n");
    print_double_vec("Expected", (double[]){1.1, 6.6, 3.3, 8.8}, 4);
    print_double_vec("Result", result, 4);

    // 验证基本测试结果
    int pass = 1;
    if (result[0] != 1.1) pass = 0;
    if (result[1] != 6.6) pass = 0;
    if (result[2] != 3.3) pass = 0;
    if (result[3] != 8.8) pass = 0;

    // 边界值测试
    double boundary1[4] ALIGNED(32) = {INFINITY, -INFINITY, NAN, 0.0};
    double boundary2[4] ALIGNED(32) = {0.0, NAN, -INFINITY, INFINITY};
    double boundary_mask[4] ALIGNED(32) = {-1.0, 0.0, -1.0, 0.0};
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vmovapd %3, %%ymm2\n\t"
        "vblendvpd %%ymm2, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(boundary1), "m"(boundary2), "m"(boundary_mask)
        : "ymm0", "ymm1", "ymm2"
    );

    printf("\n--- Boundary Test ---\n");
    print_double_vec("Expected", (double[]){0.0, -INFINITY, -INFINITY, 0.0}, 4);
    print_double_vec("Result", result, 4);

    // 验证边界测试结果
    if (result[0] != 0.0) pass = 0;
    if (!isinf(result[1]) || result[1] > 0) pass = 0; // 检查是否为-INF
    if (!isinf(result[2]) || result[2] > 0) pass = 0; // 检查是否为-INF
    if (result[3] != 0.0) pass = 0;

    if (pass) {
        printf("\nvblendvpd all tests passed\n");
    } else {
        printf("\nvblendvpd some tests failed\n");
    }
}

int main() {
    test_vblendvpd();
    return 0;
}
