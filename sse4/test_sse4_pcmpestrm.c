#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

int main() {
    printf("Testing PCMPESTRM instruction\n");
    
    // 测试数据
    uint8_t a[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    uint8_t b[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    int len = 16;
    uint64_t result;

    // 使用指针直接加载数据
    __m128i xmm0 = _mm_loadu_si128((__m128i*)a);
    __m128i xmm1 = _mm_loadu_si128((__m128i*)b);
    
    // 简化内联汇编
    __asm__ __volatile__ (
        "mov %2, %%eax\n\t"         // 设置长度到eax
        "mov %2, %%edx\n\t"         // 设置长度到edx
        "pcmpestrm %3, %1, %0\n\t"  // 执行比较
        : "+x" (xmm0)
        : "x" (xmm1), "r" (len), "i" (0x00)
        : "eax", "edx"
    );
    
    result = _mm_extract_epi64(xmm0, 0);

    printf("Result: 0x%016lX\n", result);
    printf("Expected: 0x000000000000FFFF (all equal)\n");
    printf("Test %s\n", (result == 0xFFFF) ? "PASSED" : "FAILED");

    return 0;
}
