#include "avx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

static void test_vblendpd() {
    printf("--- Testing vblendpd (blend packed double-precision floats) ---\n");

    // 测试数据
    double src1[4] ALIGNED(32) = {1.1, 2.2, 3.3, 4.4};
    double src2[4] ALIGNED(32) = {5.5, 6.6, 7.7, 8.8};
    double result[4] ALIGNED(32) = {0};

    // 测试数据

    // 测试imm=0x0
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0x0, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 0000 (0x0)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0x1
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0x1, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 0001 (0x1)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0x2
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0x2, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 0010 (0x2)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0x3
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0x3, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 0011 (0x3)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0x4
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0x4, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 0100 (0x4)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0x5
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0x5, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 0101 (0x5)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0x6
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0x6, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 0110 (0x6)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0x7
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0x7, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 0111 (0x7)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0x8
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0x8, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 1000 (0x8)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0x9
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0x9, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 1001 (0x9)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0xA
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0xA, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 1010 (0xA)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0xB
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0xB, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 1011 (0xB)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0xC
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0xC, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 1100 (0xC)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0xD
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0xD, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 1101 (0xD)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0xE
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0xE, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 1110 (0xE)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试imm=0xF
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0xF, %%ymm1, %%ymm0, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1"
    );
    printf("Imm: 1111 (0xF)\n");
    print_double_vec("Result", result, 4);
    printf("\n");

    // 测试边界值
    double boundary1[4] ALIGNED(32) = {INFINITY, -INFINITY, NAN, 0.0};
    double boundary2[4] ALIGNED(32) = {0.0, NAN, -INFINITY, INFINITY};
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0xA, %%ymm1, %%ymm0, %%ymm0\n\t"  // 选择模式1010
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(boundary1), "m"(boundary2)
        : "ymm0", "ymm1"
    );

    printf("--- Boundary Test (Imm: 0xA) ---\n");
    print_double_vec("Expected", (double[]){INFINITY, NAN, NAN, INFINITY}, 4);  // 修正预期值
    print_double_vec("Result", result, 4);

    // 验证边界测试结果
    int pass = 1;
    if (!isinf(result[0])) {
        printf("Mismatch at position 0: expected INF, got %f\n", result[0]);
        pass = 0;
    }
    if (!isnan(result[1])) {
        printf("Mismatch at position 1: expected NAN, got %f\n", result[1]);
        pass = 0;
    }
    if (!isnan(result[2])) {  // 修正为检查NAN
        printf("Mismatch at position 2: expected NAN, got %f\n", result[2]);
        pass = 0;
    }
    if (!isinf(result[3])) {
        printf("Mismatch at position 3: expected INF, got %f\n", result[3]);
        pass = 0;
    }

    // 添加更多边界值测试
    double zero1[4] ALIGNED(32) = {0.0, -0.0, 1.0e-300, 1.0e300};
    double zero2[4] ALIGNED(32) = {-0.0, 0.0, 1.0e300, 1.0e-300};
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vblendpd $0x5, %%ymm1, %%ymm0, %%ymm0\n\t"  // 选择模式0101
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(zero1), "m"(zero2)
        : "ymm0", "ymm1"
    );

    printf("\n--- Zero/Denormal Test (Imm: 0x5) ---\n");
    print_double_vec("Expected", (double[]){0.0, 0.0, 1.0e300, 1.0e-300}, 4);
    print_double_vec("Result", result, 4);

    // 验证零值和非常规数值
    if (result[0] != 0.0) pass = 0;
    if (result[1] != 0.0) pass = 0;
    if (result[2] != zero2[2]) pass = 0;
    if (result[3] != zero1[3]) pass = 0;

    if (pass) {
        printf("vblendpd all tests passed\n");
    } else {
        printf("vblendpd some tests failed\n");
    }
}

int main() {
    test_vblendpd();
    return 0;
}
