#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <immintrin.h>

// VMOVQ 指令测试
void test_vmovq() {
    printf("=== Testing VMOVQ ===\n");
    
    // 测试1: 从内存加载64位整数到XMM寄存器
    printf("\nTest 1: Load 64-bit integer from memory to XMM register\n");
    long long src_value = 0x1234567890ABCDEFLL; // 64位整数值
    long long expected = 0x1234567890ABCDEFLL; // 预期结果
    
    __m128i result;
    
    __asm__ __volatile__ (
        "vmovq %1, %0" // 执行VMOVQ：将内存值加载到XMM寄存器
        : "=x" (result)
        : "m" (src_value)
    );
    
    long long result_value;
    _mm_storeu_si128((__m128i*)&result_value, result);
    
    printf("Source: 0x%016llX\n", src_value);
    printf("Result: 0x%016llX\n", result_value);
    printf("Expected: 0x%016llX\n", expected);
    
    if(result_value == expected) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 将XMM寄存器的低64位存储到内存
    printf("\nTest 2: Store low 64-bit from XMM register to memory\n");
    long long mem_dest = 0; // 目标内存位置
    __m128i src = _mm_set_epi64x(0, 0x1122334455667788LL); // 源寄存器值
    
    __asm__ __volatile__ (
        "vmovq %1, %0" // 执行VMOVQ：将xmm寄存器的低64位存储到内存
        : "=m" (mem_dest)
        : "x" (src)
    );
    
    printf("Source vector: 0x%016llX\n", 0x1122334455667788LL);
    printf("Memory dest: 0x%016llX\n", mem_dest);
    printf("Expected: 0x%016llX\n", 0x1122334455667788LL);
    
    if(mem_dest == 0x1122334455667788LL) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vmovq();
    return 0;
}
