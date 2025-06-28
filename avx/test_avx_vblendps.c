#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

static void test_vblendps() {
    printf("--- Testing vblendps (blend packed single-precision floats) ---\n");

    // 测试数据
    float src1[8] ALIGNED(32) = {
        1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f
    };
    float src2[8] ALIGNED(32) = {
        9.9f, 10.10f, 11.11f, 12.12f, 13.13f, 14.14f, 15.15f, 16.16f
    };
    float result[8] ALIGNED(32) = {0};

    // 测试imm=0x00 (全选src1)
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vblendps $0x00, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 00000000 (0x00)\n");
    print_float_vec("Result", result, 8);
    printf("\n");

    // 测试imm=0xFF (全选src2)
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vblendps $0xFF, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 11111111 (0xFF)\n");
    print_float_vec("Result", result, 8);
    printf("\n");

    // 测试imm=0x55 (交替选择)
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vblendps $0x55, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 01010101 (0x55)\n");
    print_float_vec("Result", result, 8);
    print_hex_float_vec("Result", result, 8);
    printf("\n");

    // 测试imm=0xAA (交替选择)
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vblendps $0xAA, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 01010101 (0x55)\n");
    print_float_vec("Result", result, 8);
    print_hex_float_vec("Result", result, 8);
    printf("\n");


    // 测试边界值
    float boundary1[8] ALIGNED(32) = {
        INFINITY, -INFINITY, NAN, 0.0f, -0.0f, 1.0e-38f, 1.0e38f, 2.0f
    };
    float boundary2[8] ALIGNED(32) = {
        0.0f, NAN, -INFINITY, INFINITY, 1.0e38f, 1.0e-38f, -0.0f, 3.0f
    };
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vblendps $0xAA, %%ymm1, %%ymm0, %%ymm0\n\t"  // 选择模式10101010
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(boundary1), "m"(boundary2)
        : "ymm0", "ymm1"
    );

    printf("--- Boundary Test (Imm: 0xAA) ---\n");
    print_float_vec("Expected", (float[]){
        boundary1[0], boundary2[1], boundary1[2], boundary2[3],
        boundary1[4], boundary2[5], boundary1[6], boundary2[7]
    }, 8);
    print_float_vec("Result", result, 8);

    // 验证边界测试结果
    int pass = 1;
    if (!isinf(result[0])) pass = 0;
    if (!isnan(result[1])) pass = 0;
    if (!isnan(result[2])) pass = 0;
    if (!isinf(result[3])) pass = 0;
    if (result[4] != boundary1[4]) pass = 0;
    if (result[5] != boundary2[5]) pass = 0;
    if (result[6] != boundary1[6]) pass = 0;
    if (result[7] != boundary2[7]) pass = 0;

    if (pass) {
        printf("vblendps test passed\n");
    } else {
        printf("vblendps test failed\n");
    }
}

int main() {
    test_vblendps();
    return 0;
}
