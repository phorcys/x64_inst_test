#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VMOVLPD 指令测试
void test_vmovlpd() {
    printf("=== Testing VMOVLPD ===\n");
    
    // 测试1: 从内存加载低64位到XMM寄存器
    printf("\nTest 1: Load low 64-bit from memory to XMM register\n");
    double src_value = 123.456; // 要加载的值
    double src_reg[2] = {1.0, 2.0}; // 寄存器初始值
    double expected[2] = {123.456, 2.0}; // 预期结果
    
    __m128d result;
    
    __m128d reg = _mm_load_pd(src_reg);
    __asm__ __volatile__ (
        "vmovlpd %2, %1, %0" // 执行VMOVLPD：将内存值加载到源寄存器的低64位，结果存到目标寄存器
        : "=x" (result)
        : "x" (reg), "m" (src_value)
    );
    
    double result_arr[2];
    _mm_store_pd(result_arr, result);
    
    printf("Register initial: [%.3f, %.3f]\n", src_reg[0], src_reg[1]);
    printf("Memory value: %.3f\n", src_value);
    printf("Result: [%.3f, %.3f]\n", result_arr[0], result_arr[1]);
    printf("Expected: [%.3f, %.3f]\n", expected[0], expected[1]);
    
    if(result_arr[0] == expected[0] && result_arr[1] == expected[1]) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 将XMM寄存器的低64位存储到内存
    printf("\nTest 2: Store low 64-bit from XMM register to memory\n");
    double src_vector[2] = {10.0, 20.0}; // 源寄存器值
    double mem_dest = 0.0; // 目标内存位置
    
    __m128d src = _mm_load_pd(src_vector);
    
    __asm__ __volatile__ (
        "vmovlpd %%xmm0, %0" // 执行VMOVLPD：将xmm0的低64位存储到内存
        : "=m" (mem_dest)
        : "x" (src)
    );
    
    printf("Source vector: [%.1f, %.1f]\n", src_vector[0], src_vector[1]);
    printf("Memory dest: %.1f\n", mem_dest);
    printf("Expected: %.1f\n", src_vector[0]);
    
    if(mem_dest == src_vector[0]) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vmovlpd();
    return 0;
}
