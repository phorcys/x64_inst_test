#include "avx.h"
#include <stdio.h>
#include <string.h>

// 简化的VMASKMOVPD测试
void test_vmaskmovpd() {
    printf("=== Simplified VMASKMOVPD Test ===\n");
    
    // 只测试128位版本
    double mem[2] ALIGNED(16) = {0};
    double expected[2] = {1.1, 0.0};
    __m128d src = _mm_set_pd(2.2, 1.1);
    __m128d mask = _mm_castsi128_pd(_mm_set_epi64x(0, -1)); // 低位掩码有效
    
    // 使用显式寄存器绑定
    asm volatile(
        "vmaskmovpd %%xmm1, %%xmm0, (%0)"  // 添加额外的%转义
        : 
        : "r" (mem), "x" (mask), "x" (src)
        : "memory"
    );
    
    printf("Memory: %.1f, %.1f\n", mem[0], mem[1]);
    
    // 检查结果并打印详细输出
    printf("Expected: %.1f, %.1f\n", expected[0], expected[1]);
    printf("Actual:   %.1f, %.1f\n", mem[0], mem[1]);
    
    if(mem[0] == expected[0] && mem[1] == expected[1]) {
        printf("Test PASSED\n");
    } else {
        printf("Test FAILED\n");
    }
    
    // 输出寄存器值以便调试
    print_xmm("XMM0 (src)", _mm_castpd_si128(src));
    print_xmm("XMM1 (mask)", _mm_castpd_si128(mask));
}

int main() {
    test_vmaskmovpd();
    return 0;
}
