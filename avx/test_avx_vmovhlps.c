#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VMOVHLPS 指令测试
void test_vmovhlps() {
    printf("=== Testing VMOVHLPS ===\n");
    
    // 测试: 将高位64位从源寄存器移动到目标寄存器
    printf("\nTest: Move high 64-bits from source to destination\n");
    float src1[4] = {1.0f, 2.0f, 3.0f, 4.0f}; // 源寄存器1
    float src2[4] = {5.0f, 6.0f, 7.0f, 8.0f}; // 源寄存器2
    float expected[4] = {7.0f, 8.0f, 3.0f, 4.0f}; // 预期结果
    
    __m128 result;
    __m128 reg1 = _mm_loadu_ps(src1);
    __m128 reg2 = _mm_loadu_ps(src2);
    
    __asm__ __volatile__ (
        "vmovhlps %2, %1, %0" // 执行VMOVHLPS: %0 = 源寄存器2的高64位 + 源寄存器1的高64位
        : "=x" (result)
        : "x" (reg1), "x" (reg2)
    );
    
    float result_arr[4];
    _mm_storeu_ps(result_arr, result);
    
    printf("Source1: [%.1f, %.1f, %.1f, %.1f]\n", src1[0], src1[1], src1[2], src1[3]);
    printf("Source2: [%.1f, %.1f, %.1f, %.1f]\n", src2[0], src2[1], src2[2], src2[3]);
    printf("Result: [%.1f, %.1f, %.1f, %.1f]\n", result_arr[0], result_arr[1], result_arr[2], result_arr[3]);
    printf("Expected: [%.1f, %.1f, %.1f, %.1f]\n", expected[0], expected[1], expected[2], expected[3]);
    
    if(memcmp(result_arr, expected, sizeof(expected)) == 0) {
        printf("Test PASSED\n");
    } else {
        printf("Test FAILED\n");
    }
}

int main() {
    test_vmovhlps();
    return 0;
}
