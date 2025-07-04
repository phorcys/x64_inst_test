#include "avx.h"
#include <stdio.h>
#include <string.h>

// vinsertf128指令测试函数
static void test_vinsertf128() {
    printf("--- Testing vinsertf128 (insert 128-bit float data into 256-bit register) ---\n");
    
    // 测试数据
    float src1[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float src2[4] = {10.0f, 20.0f, 30.0f, 40.0f};
    float dst[8];
    float expected_low[8];  // 插入低128位的预期结果
    float expected_high[8]; // 插入高128位的预期结果
    
    // 计算预期结果
    memcpy(expected_low, src1, sizeof(float)*8);
    memcpy(expected_low, src2, sizeof(float)*4);
    
    memcpy(expected_high, src1, sizeof(float)*8);
    memcpy(expected_high+4, src2, sizeof(float)*4);
    
    // 测试插入低128位(立即数0)
    __asm__ __volatile__(
        "vmovups %1, %%ymm0\n\t"
        "vmovups %2, %%xmm1\n\t"
        "vinsertf128 $0, %%xmm1, %%ymm0, %%ymm2\n\t"
        "vmovups %%ymm2, %0\n\t"
        : "=m"(dst)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("vinsertf128 low 128-bits tests.\n");
    print_float_vec("src1:", src1, 8);
    print_float_vec("src2:", src2, 8);
    print_float_vec("dst :", dst, 8);
    
    // 测试插入高128位(立即数1)
    __asm__ __volatile__(
        "vmovups %1, %%ymm0\n\t"
        "vmovups %2, %%xmm1\n\t"
        "vinsertf128 $1, %%xmm1, %%ymm0, %%ymm2\n\t"
        "vmovups %%ymm2, %0\n\t"
        : "=m"(dst)
        : "m"(src1), "m"(src2)
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("vinsertf128 high 128-bits tests.\n");
    print_float_vec("src1:", src1, 8);
    print_float_vec("src2:", src2, 8);
    print_float_vec("dst :", dst, 8);

}

int main() {
    test_vinsertf128();
    return 0;
}
