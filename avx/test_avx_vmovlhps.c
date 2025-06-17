#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VMOVLHPS 指令测试
void test_vmovlhps() {
    printf("=== Testing VMOVLHPS ===\n");
    
    // 测试: 将两个寄存器的低64位组合成128位向量
    printf("\nTest: Combine low 64-bits from two registers\n");
    float src1[4] = {1.0f, 2.0f, 3.0f, 4.0f}; // 源寄存器1
    float src2[4] = {5.0f, 6.0f, 7.0f, 8.0f}; // 源寄存器2
    float expected[4] = {1.0f, 2.0f, 5.0f, 6.0f}; // 预期结果
    
    __m128 result;
    __m128 reg1 = _mm_loadu_ps(src1);
    __m128 reg2 = _mm_loadu_ps(src2);
    
    __asm__ __volatile__ (
        "vmovlhps %2, %1, %0" // 执行VMOVLHPS: %0 = [源寄存器1的高64位, 源寄存器2的低64位]
        : "=x" (result)
        : "x" (reg1), "x" (reg2)
    );
    
    float result_arr[4];
    _mm_storeu_ps(result_arr, result);
    
    printf("Source1: [%.1f, %.1f, %.1f, %.1f]\n", 
           src1[0], src1[1], src1[2], src1[3]);
    printf("Source2: [%.1f, %.1f, %.1f, %.1f]\n", 
           src2[0], src2[1], src2[2], src2[3]);
    printf("Result: [%.1f, %.1f, %.1f, %.1f]\n", 
           result_arr[0], result_arr[1], result_arr[2], result_arr[3]);
    printf("Expected: [%.1f, %.1f, %.1f, %.1f] (low 64-bits of both sources)\n", 
           expected[0], expected[1], expected[2], expected[3]);
    
    if(memcmp(result_arr, expected, sizeof(expected)) == 0) {
        printf("Test PASSED\n");
    } else {
        printf("Test FAILED\n");
    }
}

int main() {
    test_vmovlhps();
    return 0;
}
