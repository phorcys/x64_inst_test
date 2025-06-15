#include "avx.h"
#include <stdio.h>
#include <math.h>

static void test_vbroadcastss() {
    printf("--- Testing vbroadcastss (broadcast single-precision float) ---\n");

    // 测试数据
    float src[1] ALIGNED(4) = {3.141592f};
    float boundary[1] ALIGNED(4) = {INFINITY};

    // 正常值测试
    float result[8] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vbroadcastss %1, %%ymm0\n\t"
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src[0])
        : "ymm0"
    );
    printf("--- Normal Test ---\n");
    printf("Source: %f\n", src[0]);
    print_float_vec("Result", result, 8);

    // 验证结果
    int pass = 1;
    for (int i = 0; i < 8; i++) {
        if (!float_equal(result[i], src[0], 0.000001f)) {
            pass = 0;
            break;
        }
    }

    // 边界值测试
    float result_boundary[8] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vbroadcastss %1, %%ymm0\n\t"
        "vmovaps %%ymm0, %0\n\t"
        : "=m"(result_boundary)
        : "m"(boundary[0])
        : "ymm0"
    );
    printf("\n--- Boundary Test ---\n");
    printf("Expected: ");
    for (int i = 0; i < 8; i++) {
        printf("inf ");
    }
    printf("\n");
    print_float_vec("Result", result_boundary, 8);

    // 验证边界测试结果
    for (int i = 0; i < 8; i++) {
        if (!isinf(result_boundary[i])) {
            pass = 0;
            break;
        }
    }

    if (pass) {
        printf("\nvbroadcastss all tests passed\n");
    } else {
        printf("\nvbroadcastss some tests failed\n");
    }
}

int main() {
    test_vbroadcastss();
    return 0;
}
