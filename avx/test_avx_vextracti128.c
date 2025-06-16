#include "avx.h"
#include <stdio.h>
#include <string.h>

// vextracti128指令测试函数
static void test_vextracti128() {
    printf("--- Testing vextracti128 (extract 128-bit integer data from 256-bit register) ---\n");
    
    // 测试数据
    int32_t src[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int32_t dst_low[4] = {0,0,0,0};  // 提取低128位的结果
    int32_t dst_high[4] = {0,0,0,0}; // 提取高128位的结果
    int32_t expected_low[4] = {1, 2, 3, 4};  // 预期结果
    int32_t expected_high[4] = {5, 6, 7, 8}; // 预期结果
    
    memset(&dst_low, 0, 16);
    memset(&dst_high, 0, 16);
    // 测试提取低128位(立即数0)
    __asm__ __volatile__(
        "vmovdqu %1, %%ymm0\n\t"
        "vextracti128 $0, %%ymm0, %0\n\t"
        : "=m"(dst_low)
        : "m"(src)
        : "ymm0"
    );
    
    printf("vextracti128 low 128-bits tests.\n");
    print_int32_vec("src     :", src, 8);
    print_int32_vec("dst low :", dst_low, 4);
    print_int32_vec("dst high:", dst_high, 4);
    
    memset(&dst_low, 0, 16);
    memset(&dst_high, 0, 16);
    // 测试提取高128位(立即数1)
    __asm__ __volatile__(
        "vmovdqu %1, %%ymm0\n\t"
        "vextracti128 $1, %%ymm0, %0\n\t"
        : "=m"(dst_high)
        : "m"(src)
        : "ymm0"
    );
    
    printf("vextracti128 low 128-bits tests.\n");
    print_int32_vec("src     :", src, 8);
    print_int32_vec("dst low :", dst_low, 4);
    print_int32_vec("dst high:", dst_high, 4);
}

int main() {
    test_vextracti128();
    return 0;
}
