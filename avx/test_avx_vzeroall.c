#include <stdio.h>
#include <immintrin.h>
#include "avx.h"

// 测试VZEROALL指令
void test_vzeroall() {
    int errors = 0;
    printf("Testing VZEROALL instruction...\n");

    // 使用对齐内存确保正确访问
    float input[8] __attribute__((aligned(32))) = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float result[8] __attribute__((aligned(32)));
    
    // 测试YMM0寄存器
    asm volatile(
        "vmovdqu %1, %%ymm0\n\t"   // 加载值到YMM0
        "vzeroall\n\t"             // 执行VZEROALL
        "vmovdqu %%ymm0, %0"       // 存储YMM0到结果数组
        : "=m"(result)
        : "m"(input)
        : "ymm0"
    );
    
    // 检查整个YMM寄存器是否被清零
    for (int i = 0; i < 8; i++) {
        if (result[i] != 0.0f) {
            printf("Error: YMM0 not zeroed at position %d\n", i);
            printf("Expected: 0.0, Got: %f\n", result[i]);
            errors++;
        }
    }

    // 测试YMM1寄存器
    asm volatile(
        "vmovdqu %1, %%ymm1\n\t"   // 加载值到YMM1
        "vzeroall\n\t"             // 执行VZEROALL
        "vmovdqu %%ymm1, %0"       // 存储YMM1到结果数组
        : "=m"(result)
        : "m"(input)
        : "ymm1"
    );
    
    // 检查整个YMM寄存器是否被清零
    for (int i = 0; i < 8; i++) {
        if (result[i] != 0.0f) {
            printf("Error: YMM1 not zeroed at position %d\n", i);
            printf("Expected: 0.0, Got: %f\n", result[i]);
            errors++;
        }
    }

    // 输出测试结果
    if (errors == 0) {
        printf("All VZEROALL tests passed successfully!\n");
    } else {
        printf("VZEROALL tests failed: %d errors\n", errors);
    }
}

int main() {
    test_vzeroall();
    return 0;
}
