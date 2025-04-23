#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <stdalign.h>

// 测试MOVNTDQA指令(从非临时内存加载)
void test_movntdqa() {
    printf("Testing MOVNTDQA instruction\n");
    printf("--------------------------\n");
    
    // 对齐的内存区域
    alignas(16) uint32_t src[4] = {0x12345678, 0x9ABCDEF0, 0x13579BDF, 0x2468ACE0};
    __m128i dst;

    // 使用MOVNTDQA加载
    __asm__ volatile (
        "movntdqa %1, %%xmm0\n\t"
        "movdqa %%xmm0, %0\n\t"
        : "=x" (dst)
        : "m" (*src)
        : "xmm0"
    );

    // 验证结果
    uint32_t result[4];
    memcpy(result, &dst, sizeof(result));
    
    printf("Source: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
           src[0], src[1], src[2], src[3]);
    printf("Result: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
           result[0], result[1], result[2], result[3]);
    printf("Test %s\n", memcmp(src, result, sizeof(src)) ? "FAILED" : "PASSED");
}

int main() {
    test_movntdqa();
    return 0;
}
