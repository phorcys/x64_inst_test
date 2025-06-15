#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

static void test_vgatherdps() {
    printf("--- Testing vgatherdps (gather single-precision floats) ---\n");

    // 测试数据(使用对齐内存)
    float array[16] ALIGNED(32) = {
        1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f,
        INFINITY, -INFINITY, NAN, 0.0f, -0.0f, 1.0e-38f, 1.0e38f, 2.0f
    };
    int32_t indices[8] ALIGNED(32) = {1, 3, 5, 7, 8, 9, 10, 15};
    float result[8] ALIGNED(32) = {0};
    float expected[8] = {2.2f, 4.4f, 6.6f, 8.8f, INFINITY, -INFINITY, NAN, 2.0f};

    // 打印输入数据
    print_float_vec("Input array", array, 16);
    printf("Indices: ");
    for(int i=0; i<8; i++) printf("%d ", indices[i]);
    printf("\n");

    // 测试1: 全1掩码
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm1\n\t"
        "lea %2, %%rdi\n\t"
        "vpcmpeqd %%ymm2, %%ymm2, %%ymm2\n\t"  // 全1掩码
        "vgatherdps %%ymm2, (%%rdi,%%ymm1,4), %%ymm0\n\t"
        "vmovdqa %%ymm0, %0\n\t"
        : "=m"(result)
        : "m"(indices), "m"(array)
        : "ymm0", "ymm1", "ymm2", "rdi"
    );

    // 测试2: 部分掩码
    float result_partial[8] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm1\n\t"
        "lea %2, %%rdi\n\t"
        "vpxor %%ymm2, %%ymm2, %%ymm2\n\t"  // 全0掩码
        "vpcmpeqd %%ymm3, %%ymm3, %%ymm3\n\t"  
        "vpslld $31, %%ymm3, %%ymm3\n\t"    // 设置偶数位掩码
        "vgatherdps %%ymm3, (%%rdi,%%ymm1,4), %%ymm0\n\t"
        "vmovdqa %%ymm0, %0\n\t"
        : "=m"(result_partial)
        : "m"(indices), "m"(array)
        : "ymm0", "ymm1", "ymm2", "ymm3", "rdi"
    );

    // 测试3: 非对齐内存访问
    float unaligned_array[17]; // 故意不对齐
    for(int i=0; i<17; i++) unaligned_array[i] = array[i%16];
    float result_unaligned[8] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm1\n\t"
        "lea %2, %%rdi\n\t"
        "vpcmpeqd %%ymm2, %%ymm2, %%ymm2\n\t"
        "vgatherdps %%ymm2, (%%rdi,%%ymm1,4), %%ymm0\n\t"
        "vmovdqa %%ymm0, %0\n\t"
        : "=m"(result_unaligned)
        : "m"(indices), "m"(unaligned_array)
        : "ymm0", "ymm1", "ymm2", "rdi"
    );

    // 打印结果
    print_float_vec("Gathered result", result, 8);

    // 验证结果
    int pass = 1;
    for (int i = 0; i < 8; i++) {
        if (i == 6) { // 处理NaN的特殊情况
            if (!isnan(result[i])) {
                printf("Mismatch at position %d: expected NaN, got %f\n", i, result[i]);
                pass = 0;
            }
        } else if (result[i] != expected[i]) {
            printf("Mismatch at position %d: got %f, expected %f\n",
                  i, result[i], expected[i]);
            pass = 0;
        }
    }

    if (pass) {
        printf("vgatherdps test passed\n");
    } else {
        printf("vgatherdps test failed\n");
    }
}

int main() {
    test_vgatherdps();
    return 0;
}
