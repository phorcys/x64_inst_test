#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试PMINSB指令(8位有符号整数取最小值)
void test_pminsb() {
    printf("Testing PMINSB instruction\n");
    printf("-------------------------\n");
    
    // 测试数据
    struct TestData {
        int8_t a[16];
        int8_t b[16];
        int8_t expect[16];
    } tests[] = {
        {{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},
         {2,1,4,3,6,5,8,7,10,9,12,11,14,13,16,15},
         {1,1,3,3,5,5,7,7,9,9,11,11,13,13,15,15}},
         
        {{-128,127,0,-1,100,-100,50,-50,25,-25,12,-12,6,-6,3,-3},
         {127,-128,1,0,-100,100,-50,50,-25,25,-12,12,-6,6,-3,3},
         {-128,-128,0,-1,-100,-100,-50,-50,-25,-25,-12,-12,-6,-6,-3,-3}}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest case %zu:\n", i+1);
        
        __m128i a = _mm_loadu_si128((__m128i*)tests[i].a);
        __m128i b = _mm_loadu_si128((__m128i*)tests[i].b);
        __m128i res;
        
        // 使用PMINSB比较
        __asm__ volatile (
            "movdqu %1, %%xmm0\n\t"
            "movdqu %2, %%xmm1\n\t"
            "pminsb %%xmm1, %%xmm0\n\t"
            "movdqu %%xmm0, %0\n\t"
            : "=x" (res)
            : "m" (a), "m" (b)
            : "xmm0", "xmm1"
        );

        // 验证结果
        int8_t result[16];
        memcpy(result, &res, sizeof(result));
        
        printf("A: [");
        for (int j = 0; j < 16; j++) printf("%d ", tests[i].a[j]);
        printf("]\nB: [");
        for (int j = 0; j < 16; j++) printf("%d ", tests[i].b[j]);
        printf("]\nResult: [");
        for (int j = 0; j < 16; j++) printf("%d ", result[j]);
        printf("]\nExpect: [");
        for (int j = 0; j < 16; j++) printf("%d ", tests[i].expect[j]);
        printf("]\nTest %s\n", 
               memcmp(result, tests[i].expect, sizeof(result)) ? "FAILED" : "PASSED");
    }
}

int main() {
    test_pminsb();
    return 0;
}
