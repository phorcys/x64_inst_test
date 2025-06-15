#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

static void test_vblendvps() {
    printf("--- Testing vblendvps (variable blend packed single-precision floats) ---\n");

    // 测试数据
    float src1[8] ALIGNED(32) = {
        1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f
    };
    float src2[8] ALIGNED(32) = {
        9.9f, 10.10f, 11.11f, 12.12f, 13.13f, 14.14f, 15.15f, 16.16f
    };
    float mask[8] ALIGNED(32) = {
        0.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f
    }; // 掩码: 0表示选src1, -1表示选src2
    float result[8] ALIGNED(32) = {0};

    // 基本测试
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vmovaps %3, %%ymm2\n\t"
        "vblendvps %%ymm2, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2), "m"(mask)
        : "ymm0", "ymm1", "ymm2"
    );
    printf("--- Basic Test ---\n");
    print_float_vec("Expected", (float[]){
        1.1f, 10.10f, 3.3f, 12.12f, 5.5f, 14.14f, 7.7f, 16.16f
    }, 8);
    print_float_vec("Result", result, 8);

    // 验证基本测试结果
    int pass = 1;
    if (result[0] != 1.1f) pass = 0;
    if (result[1] != 10.10f) pass = 0;
    if (result[2] != 3.3f) pass = 0;
    if (result[3] != 12.12f) pass = 0;
    if (result[4] != 5.5f) pass = 0;
    if (result[5] != 14.14f) pass = 0;
    if (result[6] != 7.7f) pass = 0;
    if (result[7] != 16.16f) pass = 0;

    // 边界值测试
    float boundary1[8] ALIGNED(32) = {
        INFINITY, -INFINITY, NAN, 0.0f, -0.0f, 1.0e-38f, 1.0e38f, 2.0f
    };
    float boundary2[8] ALIGNED(32) = {
        0.0f, NAN, -INFINITY, INFINITY, 1.0e38f, 1.0e-38f, -0.0f, 3.0f
    };
    float boundary_mask[8] ALIGNED(32) = {
        -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f
    };
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vmovaps %3, %%ymm2\n\t"
        "vblendvps %%ymm2, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(boundary1), "m"(boundary2), "m"(boundary_mask)
        : "ymm0", "ymm1", "ymm2"
    );

    printf("\n--- Boundary Test ---\n");
    print_float_vec("Expected", (float[]){
        0.0f, -INFINITY, -INFINITY, 0.0f, 1.0e38f, 1.0e-38f, -0.0f, 2.0f
    }, 8);
    print_float_vec("Result", result, 8);

    // 验证边界测试结果
    if (result[0] != 0.0f) pass = 0;
    if (!isinf(result[1]) || result[1] > 0) pass = 0; // 检查是否为-INF
    if (!isinf(result[2]) || result[2] > 0) pass = 0; // 检查是否为-INF
    if (result[3] != 0.0f) pass = 0;
    if (result[4] != boundary2[4]) pass = 0;
    if (result[5] != boundary1[5]) pass = 0;
    if (result[6] != boundary2[6]) pass = 0;
    if (result[7] != boundary1[7]) pass = 0;

    if (pass) {
        printf("\nvblendvps all tests passed\n");
    } else {
        printf("\nvblendvps some tests failed\n");
    }
}

int main() {
    test_vblendvps();
    return 0;
}
