#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试PMINUD指令(32位无符号整数取最小值)
void test_pminud() {
    printf("Testing PMINUD instruction\n");
    printf("-------------------------\n");
    
    // 测试数据
    struct TestData {
        uint32_t a[4];
        uint32_t b[4];
        uint32_t expect[4];
    } tests[] = {
        {{1, 2, 3, 4}, {2, 1, 4, 3}, {1, 1, 3, 3}},
        {{0xFFFFFFFF, 0, 0x80000000, 0x7FFFFFFF},
         {0, 0xFFFFFFFF, 0x7FFFFFFF, 0x80000000},
         {0, 0, 0x7FFFFFFF, 0x7FFFFFFF}},
        {{100, 200, 300, 400}, {150, 150, 350, 350}, {100, 150, 300, 350}}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest case %zu:\n", i+1);
        
        __m128i a = _mm_loadu_si128((__m128i*)tests[i].a);
        __m128i b = _mm_loadu_si128((__m128i*)tests[i].b);
        __m128i res;
        
        // 使用PMINUD比较
        __asm__ volatile (
            "movdqu %1, %%xmm0\n\t"
            "movdqu %2, %%xmm1\n\t"
            "pminud %%xmm1, %%xmm0\n\t"
            "movdqu %%xmm0, %0\n\t"
            : "=x" (res)
            : "m" (a), "m" (b)
            : "xmm0", "xmm1"
        );

        // 验证结果
        uint32_t result[4];
        memcpy(result, &res, sizeof(result));
        
        printf("A: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               tests[i].a[0], tests[i].a[1], tests[i].a[2], tests[i].a[3]);
        printf("B: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               tests[i].b[0], tests[i].b[1], tests[i].b[2], tests[i].b[3]);
        printf("Result: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               result[0], result[1], result[2], result[3]);
        printf("Expect: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               tests[i].expect[0], tests[i].expect[1], 
               tests[i].expect[2], tests[i].expect[3]);
        printf("Test %s\n", 
               memcmp(result, tests[i].expect, sizeof(result)) ? "FAILED" : "PASSED");
    }
}

int main() {
    test_pminud();
    return 0;
}
