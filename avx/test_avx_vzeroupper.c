#include <stdio.h>
#include <immintrin.h>
#include "avx.h"

// 测试VZEROUPPER指令
void test_vzeroupper() {
    int errors = 0;
    printf("Testing VZEROUPPER instruction...\n");

    // 使用对齐内存确保正确访问
    float input[8] __attribute__((aligned(32))) = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float result[8] __attribute__((aligned(32)));
    
    asm volatile(
        "vmovdqu %1, %%ymm0\n\t"   // 使用未对齐加载
        "vzeroupper\n\t"           // 执行VZEROUPPER
        "vmovdqu %%ymm0, %0"       // 使用未对齐存储
        : "=m"(result)
        : "m"(input)
        : "ymm0"
    );
    
    // 检查高128位（索引4-7）是否被清零
    for (int i = 4; i < 8; i++) {
        if (result[i] != 0.0f) {
            printf("Error: YMM high 128 bits not zeroed at position %d\n", i);
            printf("Expected: 0.0, Got: %f\n", result[i]);
            errors++;
        }
    }
    
    // 检查低128位（索引0-3）是否保持不变
    for (int i = 0; i < 4; i++) {
        if (result[i] != 1.0f) {
            printf("Error: YMM low 128 bits changed at position %d\n", i);
            printf("Expected: 1.0, Got: %f\n", result[i]);
            errors++;
        }
    }
    
    // 检查XMM寄存器是否不受影响 - 使用对齐内存
    float xmm_input[4] __attribute__((aligned(16))) = {1.0f, 1.0f, 1.0f, 1.0f};
    float xmm_result[4] __attribute__((aligned(16)));
    
    asm volatile(
        "vmovdqu %1, %%xmm1\n\t"   // 加载值到XMM1
        "vmovdqu %%xmm1, %0"       // 存储XMM1到结果数组
        : "=m"(xmm_result)
        : "m"(xmm_input)
        : "xmm1"
    );
    
    for (int i = 0; i < 4; i++) {
        if (xmm_result[i] != 1.0f) {
            printf("Error: XMM register affected at position %d\n", i);
            printf("Expected: 1.0, Got: %f\n", xmm_result[i]);
            errors++;
        }
    }

    // 输出测试结果
    if (errors == 0) {
        printf("All VZEROUPPER tests passed successfully!\n");
    } else {
        printf("VZEROUPPER tests failed: %d errors\n", errors);
    }
}

int main() {
    test_vzeroupper();
    return 0;
}
