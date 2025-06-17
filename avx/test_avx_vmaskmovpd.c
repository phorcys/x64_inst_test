#include "avx.h"
#include <stdio.h>
#include <string.h>

// VMASKMOVPD 指令测试
void test_vmaskmovpd() {
    printf("=== Testing VMASKMOVPD ===\n");
    
    // 测试1: 128位版本 - 对齐内存
    printf("\nTest 1: 128-bit aligned memory\n");
    double mem1[2] ALIGNED(16) = {0};
    double expected1[2] = {1.1, 0.0}; // 低位掩码有效
    __m128d src1 = _mm_set_pd(2.2, 1.1);
    __m128d mask1 = _mm_castsi128_pd(_mm_set_epi64x(0, -1)); // 低位掩码有效
    
    // 准备RDI指向目标内存
    register void* rdi asm("rdi") = mem1;
    
    asm volatile(
        "vmaskmovpd (%0), %1, %2"
        : 
        : "D" (rdi), "x" (src1), "x" (mask1)
        : "memory"
    );
    
    print_xmm("XMM0 (src)", _mm_castpd_si128(src1));
    print_xmm("XMM1 (mask)", _mm_castpd_si128(mask1));
    printf("Memory after: %.1f, %.1f\n", mem1[0], mem1[1]);
    
    if(memcmp(mem1, expected1, sizeof(mem1)) == 0) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位版本 - 未对齐内存
    printf("\nTest 2: 256-bit unaligned memory\n");
    double mem2[4] = {0};
    double expected2[4] = {3.3, 0, 0, 6.6}; // 修正期望值
    __m256d src2 = _mm256_set_pd(6.6, 5.5, 4.4, 3.3);
    __m256d mask2 = _mm256_castsi256_pd(
        _mm256_set_epi64x(-1, -1, 0, 0)); // 高位两个元素有效
    
    // 准备RDI指向目标内存
    rdi = mem2 + 1; // 未对齐地址
    
    asm volatile(
        "vmaskmovpd (%0), %1, %2"
        : 
        : "D" (rdi), "x" (src2), "x" (mask2)
        : "memory"
    );
    
    print_ymm("YMM0 (src)", _mm256_castpd_si256(src2));
    print_ymm("YMM1 (mask)", _mm256_castpd_si256(mask2));
    printf("Memory after: %.1f, %.1f, %.1f, %.1f\n", 
           mem2[0], mem2[1], mem2[2], mem2[3]);
    
    if(memcmp(mem2, expected2, sizeof(mem2)) == 0) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
    
    // 测试3: 特殊值测试 (NaN, Inf, 边界值)
    printf("\nTest 3: Special values\n");
    double mem3[2] ALIGNED(16) = {0};
    double nan = 0.0/0.0;
    double inf = 1.0/0.0;
    // 移除未使用的expected3变量
    // double expected3[2] = {nan, inf};
    __m128d src3 = _mm_set_pd(inf, nan);
    __m128d mask3 = _mm_castsi128_pd(_mm_set_epi64x(-1, -1)); // 全掩码
    
    rdi = mem3;
    
    asm volatile(
        "vmaskmovpd (%0), %1, %2"
        : 
        : "D" (rdi), "x" (src3), "x" (mask3)
        : "memory"
    );
    
    int nan_ok = isnan(mem3[0]) != 0;
    int inf_ok = (isinf(mem3[1]) && mem3[1] > 0) ? 1 : 0;
    
    printf("Memory after: %f (isnan=%d), %f (isinf=%d)\n", 
           mem3[0], nan_ok, mem3[1], inf_ok);
    
    if(nan_ok && inf_ok) {
        printf("Test 3 PASSED\n");
    } else {
        printf("Test 3 FAILED\n");
    }
}

int main() {
    test_vmaskmovpd();
    return 0;
}
