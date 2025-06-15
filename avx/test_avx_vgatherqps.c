#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

static void test_vgatherqps() {
    printf("--- Testing vgatherqps (gather single-precision floats with 64-bit indices) ---\n");

    // 测试数据(使用对齐内存)
    float array[16] ALIGNED(32) = {
        1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f, 7.7f, 8.8f,
        INFINITY, -INFINITY, NAN, 0.0f, -0.0f, 1.0e-38f, 1.0e38f, 2.0f
    };
    int64_t indices[4] ALIGNED(32) = {1, 3, 8, 15};
    float result[4] ALIGNED(32) = {0};
    float expected[4] = {2.2f, 4.4f, INFINITY, 2.0f};

    // 打印输入数据
    print_float_vec("Input array", array, 16);
    printf("Indices: ");
    for(int i=0; i<4; i++) printf("%ld ", indices[i]);
    printf("\n");

    // 执行vgatherqps指令
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm1\n\t"        // 加载64位索引到ymm1
        "lea %2, %%rdi\n\t"             // 加载数组基地址
        "vpcmpeqd %%xmm2, %%xmm2, %%xmm2\n\t"  // 设置全1掩码(xmm)
        "vgatherqps %%xmm2, (%%rdi,%%ymm1,4), %%xmm0\n\t"  // 使用ymm1作为64位索引(每个元素4字节)
        "vmovdqa %%xmm0, %0\n\t"        // 存储结果到xmm0
        : "=m"(result)
        : "m"(indices), "m"(array)
        : "xmm0", "ymm1", "ymm2", "rdi"
    );

    // 打印结果
    print_float_vec("Gathered result", result, 4);

    // 测试2: 部分掩码
    float result_partial[4] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm1\n\t"
        "lea %2, %%rdi\n\t"
        "vpxor %%xmm2, %%xmm2, %%xmm2\n\t"  // 全0掩码
        "vpcmpeqd %%xmm3, %%xmm3, %%xmm3\n\t"  
        "vpslld $31, %%xmm3, %%xmm3\n\t"    // 设置偶数位掩码
        "vgatherqps %%xmm3, (%%rdi,%%ymm1,4), %%xmm0\n\t"
        "vmovdqa %%xmm0, %0\n\t"
        : "=m"(result_partial)
        : "m"(indices), "m"(array)
        : "xmm0", "ymm1", "xmm2", "xmm3", "rdi"
    );

    // 测试3: 非对齐内存访问
    float unaligned_array[17]; // 故意不对齐
    for(int i=0; i<17; i++) unaligned_array[i] = array[i%16];
    float result_unaligned[4] ALIGNED(32) = {0};
    __asm__ __volatile__(
        "vmovdqa %1, %%ymm1\n\t"
        "lea %2, %%rdi\n\t"
        "vpcmpeqd %%xmm2, %%xmm2, %%xmm2\n\t"
        "vgatherqps %%xmm2, (%%rdi,%%ymm1,4), %%xmm0\n\t"
        "vmovdqa %%xmm0, %0\n\t"
        : "=m"(result_unaligned)
        : "m"(indices), "m"(unaligned_array)
        : "xmm0", "ymm1", "xmm2", "rdi"
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
        printf("vgatherqps test passed\n");
    } else {
        printf("vgatherqps test failed\n");
    }
}

int main() {
    test_vgatherqps();
    return 0;
}
