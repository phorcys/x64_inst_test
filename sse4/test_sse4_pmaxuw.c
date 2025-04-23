#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试PMAXUW指令(16位无符号整数取最大值)
void test_pmaxuw() {
    printf("Testing PMAXUW instruction\n");
    printf("-------------------------\n");
    
    // 测试数据
    struct TestData {
        uint16_t a[8];
        uint16_t b[8];
        uint16_t expect[8];
    } tests[] = {
        {{1, 2, 3, 4, 5, 6, 7, 8},
         {2, 1, 4, 3, 6, 5, 8, 7},
         {2, 2, 4, 4, 6, 6, 8, 8}},
         
        {{0, 0xFFFF, 0x8000, 0x7FFF, 100, 200, 300, 400},
         {0xFFFF, 0, 0x7FFF, 0x8000, 150, 150, 350, 350},
         {0xFFFF, 0xFFFF, 0x8000, 0x8000, 150, 200, 350, 400}}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest case %zu:\n", i+1);
        
        __m128i a = _mm_loadu_si128((__m128i*)tests[i].a);
        __m128i b = _mm_loadu_si128((__m128i*)tests[i].b);
        __m128i res;
        
        // 使用PMAXUW比较
        __asm__ volatile (
            "movdqu %1, %%xmm0\n\t"
            "movdqu %2, %%xmm1\n\t"
            "pmaxuw %%xmm1, %%xmm0\n\t"
            "movdqu %%xmm0, %0\n\t"
            : "=x" (res)
            : "m" (a), "m" (b)
            : "xmm0", "xmm1"
        );

        // 验证结果
        uint16_t result[8];
        memcpy(result, &res, sizeof(result));
        
        printf("A: [");
        for (int j = 0; j < 8; j++) printf("0x%04X ", tests[i].a[j]);
        printf("]\nB: [");
        for (int j = 0; j < 8; j++) printf("0x%04X ", tests[i].b[j]);
        printf("]\nResult: [");
        for (int j = 0; j < 8; j++) printf("0x%04X ", result[j]);
        printf("]\nExpect: [");
        for (int j = 0; j < 8; j++) printf("0x%04X ", tests[i].expect[j]);
        printf("]\nTest %s\n", 
               memcmp(result, tests[i].expect, sizeof(result)) ? "FAILED" : "PASSED");
    }
}

int main() {
    test_pmaxuw();
    return 0;
}
