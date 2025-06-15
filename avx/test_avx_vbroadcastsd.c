#include "avx.h"
#include <stdio.h>
#include <math.h>

static void test_vbroadcastsd() {
    printf("--- Testing vbroadcastsd (broadcast double-precision float) ---\n");

    // 测试数据
    double src[1] ALIGNED(8) = {3.141592653589793};
    double boundary[1] ALIGNED(8) = {INFINITY};

    // 正常值测试
    double result[4] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vbroadcastsd %1, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(src[0])
        : "ymm0"
    );
    printf("--- Normal Test ---\n");
    printf("Source: %f\n", src[0]);
    print_double_vec("Result", result, 4);

    // 验证结果
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (!double_equal(result[i], src[0], 0.000001)) {
            pass = 0;
            break;
        }
    }

    // 边界值测试
    double result_boundary[4] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vbroadcastsd %1, %%ymm0\n\t"
        "vmovapd %%ymm0, %0\n\t"
        : "=m"(result_boundary)
        : "m"(boundary[0])
        : "ymm0"
    );
    printf("\n--- Boundary Test ---\n");
    printf("Expected: ");
    for (int i = 0; i < 4; i++) {
        printf("inf ");
    }
    printf("\n");
    print_double_vec("Result", result_boundary, 4);

    // 验证边界测试结果
    for (int i = 0; i < 4; i++) {
        if (!isinf(result_boundary[i])) {
            pass = 0;
            break;
        }
    }

    if (pass) {
        printf("\nvbroadcastsd all tests passed\n");
    } else {
        printf("\nvbroadcastsd some tests failed\n");
    }
}

int main() {
    test_vbroadcastsd();
    return 0;
}
