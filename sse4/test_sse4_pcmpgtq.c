#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 测试PCMPGTQ指令(64位有符号整数比较)
void test_pcmpgtq() {
    printf("Testing PCMPGTQ instruction\n");
    printf("--------------------------\n");
    
    // 测试数据
    struct TestData {
        int64_t a[2];
        int64_t b[2];
        int64_t expect[2];  // 预期结果: -1(大于)或0(不大于)
    } tests[] = {
        {{10, -5}, {5, -3}, {-1, 0}},    // 10>5, -5>-3
        {{-1, 0}, {0, 0}, {0, 0}},       // -1>0, 0>0
        {{0x7FFFFFFFFFFFFFFF, 0x8000000000000000}, 
         {0, -1}, 
         {-1, 0}}  // 边界值测试
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest case %zu:\n", i+1);
        
        __m128i a = _mm_loadu_si128((__m128i*)tests[i].a);
        __m128i b = _mm_loadu_si128((__m128i*)tests[i].b);
        __m128i res;
        
        // 使用PCMPGTQ比较
        __asm__ volatile (
            "movdqu %1, %%xmm0\n\t"
            "movdqu %2, %%xmm1\n\t"
            "pcmpgtq %%xmm1, %%xmm0\n\t"
            "movdqu %%xmm0, %0\n\t"
            : "=x" (res)
            : "m" (a), "m" (b)
            : "xmm0", "xmm1"
        );

        // 验证结果
        int64_t result[2];
        memcpy(result, &res, sizeof(result));
        
        printf("A: [%ld, %ld]\n", tests[i].a[0], tests[i].a[1]);
        printf("B: [%ld, %ld]\n", tests[i].b[0], tests[i].b[1]);
        printf("Result: [0x%016lX, 0x%016lX]\n", result[0], result[1]);
        printf("Expect: [0x%016lX, 0x%016lX]\n", 
               tests[i].expect[0], tests[i].expect[1]);
        printf("Test %s\n", 
               memcmp(result, tests[i].expect, sizeof(result)) ? "FAILED" : "PASSED");
    }
}

int main() {
    test_pcmpgtq();
    return 0;
}
