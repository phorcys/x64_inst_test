#include "avx.h"
#include <stdio.h>
#include <string.h>

// vinserti128指令测试函数
static void test_vinserti128() {
    printf("--- Testing vinserti128 (insert 128-bit integer data into 256-bit register) ---\n");
    
    // 测试数据
    int32_t src1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int32_t src2[4] = {10, 20, 30, 40};
    int32_t dst[8];
    int32_t expected_low[8];  // 插入低128位的预期结果
    int32_t expected_high[8]; // 插入高128位的预期结果
    
    // 计算预期结果
    memcpy(expected_low, src1, sizeof(int32_t)*8);
    memcpy(expected_low, src2, sizeof(int32_t)*4);
    
    memcpy(expected_high, src1, sizeof(int32_t)*8);
    memcpy(expected_high+4, src2, sizeof(int32_t)*4);
    
    // 测试插入低128位(立即数0)
    __asm__ __volatile__(
        "vmovdqu %1, %%ymm0\n\t"
        "vmovdqu %2, %%xmm1\n\t"
        "vinserti128 $0, %%xmm1, %%ymm0, %%ymm2\n\t"
        "vmovdqu %%ymm2, %0\n\t"
        : "=m"(dst)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("vinsertf128 low 128-bits tests.\n");
    print_int32_vec("src1:", src1, 8);
    print_int32_vec("src2:", src2, 8);
    print_int32_vec("dst :", dst, 8);
    
    int32_t src11[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int32_t src21[4] = {10, 20, 30, 40};
    int32_t dst1[8];
    // 测试插入高128位(立即数1)
    __asm__ __volatile__(
        "vmovdqu %1, %%ymm0\n\t"
        "vmovdqu %2, %%xmm1\n\t"
        "vinserti128 $1, %%xmm1, %%ymm0, %%ymm2\n\t"
        "vmovdqu %%ymm2, %0\n\t"
        : "=m"(dst1)
        : "m"(src11), "m"(src21)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("vinsertf128 low 128-bits tests.\n");
    print_int32_vec("src1:", src11, 8);
    print_int32_vec("src2:", src21, 8);
    print_int32_vec("dst :", dst1, 8);
}

int main() {
    test_vinserti128();
    return 0;
}
