#include "avx.h"
#include <stdio.h>
#include <stdint.h>

static void test_vgatherdpd() {
    printf("--- Testing vgatherdpd (gather double-precision floats) ---\n");

    // 测试数据(使用对齐内存)
    double array[8] __attribute__((aligned(32))) = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
    int32_t indices[4] __attribute__((aligned(32))) = {1, 3, 5, 7};
    double result[4] __attribute__((aligned(32))) = {0};
    double expected[4] = {2.2, 4.4, 6.6, 8.8};

    // 调试输出索引值
    printf("Indices values: ");
    for(int i=0; i<4; i++) printf("%d ", indices[i]);
    printf("\n");

    // 执行vgatherdpd指令(正确版)
    __asm__ __volatile__(
        "vmovdqa %1, %%xmm1\n\t"        // 加载索引到xmm1(128位)
        "lea %2, %%rdi\n\t"             // 加载数组基地址
        "vpcmpeqd %%ymm2, %%ymm2, %%ymm2\n\t"  // 设置全1掩码
        "vgatherdpd %%ymm2, (%%rdi,%%xmm1,8), %%ymm0\n\t"  // 使用xmm1作为索引
        "vmovdqa %%ymm0, %0\n\t"        // 存储结果
        : "=m"(result)
        : "m"(indices), "m"(array)
        : "ymm0", "ymm1", "ymm2", "rdi"
    );

    // 调试输出结果值
    printf("Result values: ");
    for(int i=0; i<4; i++) printf("%f ", result[i]);
    printf("\n");

    // 验证结果
    int pass = 1;
    for (int i = 0; i < 4; i++) {
        if (result[i] != expected[i]) {
            printf("Mismatch at position %d: got %f, expected %f\n",
                  i, result[i], expected[i]);
            pass = 0;
        }
    }

    if (pass) {
        printf("vgatherdpd test passed\n");
    } else {
        printf("vgatherdpd test failed\n");
    }
}

int main() {
    test_vgatherdpd();
    return 0;
}
