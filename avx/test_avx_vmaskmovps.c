#include "avx.h"
#include <stdio.h>
#include <string.h>

// VMASKMOVPS 指令测试
void test_vmaskmovps() {
    printf("=== Testing VMASKMOVPS ===\n");
    
    // 测试1: 128位版本 - 对齐内存
    printf("\nTest 1: 128-bit aligned memory\n");
    float mem1[4] ALIGNED(16) = {0};
    float expected1[4] = {1.1f, 0.0f, 3.3f, 0.0f}; // 第0和2元素有效
    __m128 src1 = _mm_set_ps(4.4f, 3.3f, 2.2f, 1.1f);
    __m128 mask1 = _mm_castsi128_ps(_mm_set_epi32(0, -1, 0, -1)); // 低位两个元素有效
    
    register void* rdi asm("rdi") = mem1;
    asm volatile(
        "vmaskmovps (%%rdi), %%xmm1, %%xmm0"
        : 
        : "D" (rdi), "x" (mask1), "x" (src1)
        : "memory"
    );
    // 添加寄存器值输出
    print_xmm("XMM0 (src)", _mm_castps_si128(src1));
    print_xmm("XMM1 (mask)", _mm_castps_si128(mask1));
    
    printf("Memory: %.1f, %.1f, %.1f, %.1f\n", 
           mem1[0], mem1[1], mem1[2], mem1[3]);
    
    if(memcmp(mem1, expected1, sizeof(mem1)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 特殊值测试 (NaN, Inf)
    printf("\nTest 2: Special values\n");
    float mem2[4] ALIGNED(16) = {0};
    float nan = 0.0f/0.0f;
    float inf = 1.0f/0.0f;
    __m128 src2 = _mm_set_ps(inf, nan, -inf, 0.0f);
    __m128 mask2 = _mm_castsi128_ps(_mm_set_epi32(-1, -1, -1, 0)); // 高位三个元素有效
    
    register void* rsi asm("rdi") = mem2;
    asm volatile(
        "vmaskmovps (%%rdi), %%xmm1, %%xmm0"
        : 
        : "D" (rsi), "x" (mask2), "x" (src2)
        : "memory"
    );
    // 添加寄存器值输出
    print_xmm("XMM0 (src)", _mm_castps_si128(src2));
    print_xmm("XMM1 (mask)", _mm_castps_si128(mask2));
    
    int nan_ok = isnan(mem2[1]) != 0;
    int inf_ok = (isinf(mem2[0]) && mem2[0] < 0) ? 1 : 0; // 检查负无穷
    int pos_inf_ok = (isinf(mem2[2]) && mem2[2] > 0) ? 1 : 0;
    
    printf("Memory: %f, %f, %f, %f\n", mem2[0], mem2[1], mem2[2], mem2[3]);
    printf("Checks: -inf:%d, nan:%d, +inf:%d\n", inf_ok, nan_ok, pos_inf_ok);
    
    if(inf_ok && nan_ok && pos_inf_ok && mem2[3] == 0.0f) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vmaskmovps();
    return 0;
}
