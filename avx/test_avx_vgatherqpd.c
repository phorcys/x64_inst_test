#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

static void test_vgatherqpd() {
    printf("--- Testing vgatherqpd (gather quad-precision floats with 64-bit indices) ---\n");

    // 测试数据(使用对齐内存)
    double array[8] ALIGNED(32) = {
        1.1, 2.2, 3.3, 4.4, 
        INFINITY, -INFINITY, NAN, 0.0
    };
    int64_t indices[4] ALIGNED(32) = {1, 3, 4, 7};
    double result[4] ALIGNED(32) = {0};
    double expected[4] = {2.2, 4.4, INFINITY, 0.0};

    // 打印输入数据
    printf("Input array: ");
    for(int i=0; i<8; i++) printf("%f ", array[i]);
    printf("\nIndices: ");
    for(int i=0; i<4; i++) printf("%ld ", indices[i]);
    printf("\n");

    // 执行vgatherqpd指令
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm1\n\t"        // 加载64位索引到ymm1
        "lea %2, %%rdi\n\t"             // 加载数组基地址
        "vpcmpeqd %%ymm2, %%ymm2, %%ymm2\n\t"  // 设置全1掩码
        "vgatherqpd %%ymm2, (%%rdi,%%ymm1,8), %%ymm0\n\t"  // 使用ymm1作为64位索引(每个元素8字节)
        "vmovdqa %%ymm0, %0\n\t"        // 存储结果
        : "=m"(result)
        : "m"(indices), "m"(array)
        : "ymm0", "ymm1", "ymm2", "rdi"
    );

    // 打印结果
    printf("Gathered result: ");
    for(int i=0; i<4; i++) printf("%f ", result[i]);
    printf("\n");

    // 测试2: 部分掩码
    double result_partial[4] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm1\n\t"
        "lea %2, %%rdi\n\t"
        "vpxor %%ymm2, %%ymm2, %%ymm2\n\t"  // 全0掩码
        "vpcmpeqd %%ymm3, %%ymm3, %%ymm3\n\t"  
        "vpsllq $63, %%ymm3, %%ymm3\n\t"    // 设置偶数位掩码
        "vgatherqpd %%ymm3, (%%rdi,%%ymm1,8), %%ymm0\n\t"
        "vmovdqa %%ymm0, %0\n\t"
        : "=m"(result_partial)
        : "m"(indices), "m"(array)
        : "ymm0", "ymm1", "ymm2", "ymm3", "rdi"
    );

    // 测试3: 非对齐内存访问
    double unaligned_array[9]; // 故意不对齐
    for(int i=0; i<9; i++) unaligned_array[i] = array[i%8];
    double result_unaligned[4] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm1\n\t"
        "lea %2, %%rdi\n\t"
        "vpcmpeqd %%ymm2, %%ymm2, %%ymm2\n\t"
        "vgatherqpd %%ymm2, (%%rdi,%%ymm1,8), %%ymm0\n\t"
        "vmovdqa %%ymm0, %0\n\t"
        : "=m"(result_unaligned)
        : "m"(indices), "m"(unaligned_array)
        : "ymm0", "ymm1", "ymm2", "rdi"
    );

    // 验证结果
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (i == 2) { // 处理INFINITY
            if (!isinf(result[i])) {
                printf("Mismatch at position %d: expected INF, got %f\n", i, result[i]);
                pass = 0;
            }
        } else if (i == 1 && isnan(result[i])) { // 处理NAN
            if (!isnan(result[i])) {
                printf("Mismatch at position %d: expected NAN, got %f\n", i, result[i]);
                pass = 0;
            }
        } else if (result[i] != expected[i]) {
            printf("Mismatch at position %d: got %f, expected %f\n",
                  i, result[i], expected[i]);
            pass = 0;
        }
    }

    if (pass) {
        printf("vgatherqpd test passed\n");
    } else {
        printf("vgatherqpd test failed\n");
    }
}

int main() {
    test_vgatherqpd();
    return 0;
}
