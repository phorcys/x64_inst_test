#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VMOVLPS 指令测试
void test_vmovlps() {
    printf("=== Testing VMOVLPS ===\n");
    
    // 测试1: 从内存加载低64位到XMM寄存器
    printf("\nTest 1: Load low 64-bit from memory to XMM register\n");
    float src_value[2] = {100.0f, 200.0f}; // 要加载的值
    float src_reg[4] = {1.0f, 2.0f, 3.0f, 4.0f}; // 寄存器初始值
    float expected[4] = {100.0f, 200.0f, 3.0f, 4.0f}; // 预期结果
    
    __m128 result;
    
    __m128 reg = _mm_loadu_ps(src_reg);
    __asm__ __volatile__ (
        "vmovlps %2, %1, %0" // 执行VMOVLPS：将内存值加载到源寄存器的低64位，结果存到目标寄存器
        : "=x" (result)
        : "x" (reg), "m" (*(__m128*)src_value)
    );
    
    float result_arr[4];
    _mm_storeu_ps(result_arr, result);
    
    printf("Register initial: [%.1f, %.1f, %.1f, %.1f]\n", 
           src_reg[0], src_reg[1], src_reg[2], src_reg[3]);
    printf("Memory value: [%.1f, %.1f]\n", src_value[0], src_value[1]);
    printf("Result: [%.1f, %.1f, %.1f, %.1f]\n", 
           result_arr[0], result_arr[1], result_arr[2], result_arr[3]);
    printf("Expected: [%.1f, %.1f, %.1f, %.1f]\n", 
           expected[0], expected[1], expected[2], expected[3]);
    
    if(memcmp(result_arr, expected, sizeof(expected)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 将XMM寄存器的低64位存储到内存
    printf("\nTest 2: Store low 64-bit from XMM register to memory\n");
    float src_vector[4] = {10.0f, 20.0f, 30.0f, 40.0f}; // 源寄存器值
    float mem_dest[2] = {0.0f, 0.0f}; // 目标内存位置
    
    __m128 src = _mm_loadu_ps(src_vector);
    
    __asm__ __volatile__ (
        "vmovlps %%xmm0, %0" // 执行VMOVLPS：将xmm0的低64位存储到内存
        : "=m" (*(__m128*)mem_dest)
        : "x" (src)
    );
    
    printf("Source vector: [%.1f, %.1f, %.1f, %.1f]\n", 
           src_vector[0], src_vector[1], src_vector[2], src_vector[3]);
    printf("Memory dest: [%.1f, %.1f]\n", mem_dest[0], mem_dest[1]);
    printf("Expected: [%.1f, %.1f]\n", src_vector[0], src_vector[1]);
    
    if(mem_dest[0] == src_vector[0] && mem_dest[1] == src_vector[1]) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vmovlps();
    return 0;
}
