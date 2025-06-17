#include "avx.h"
#include <stdio.h>
#include <math.h>

// VMOVHLPS 指令测试
void test_vmovhlps() {
    printf("=== Testing VMOVHLPS ===\n");
    
    // 测试1: 基本功能测试
    printf("\nTest 1: Basic functionality\n");
    float src1[4] = {1.1f, 2.2f, 3.3f, 4.4f};
    float dst[4] = {0};
    float expected[4] = {0}; // 稍后设置期望值
    
    __asm__ __volatile__ (
        "vmovups %1, %%xmm0\n\t"      // 加载src1到xmm0
        "vmovups %2, %%xmm1\n\t"      // 加载src1+2到xmm1
        "vmovhlps %%xmm0, %%xmm1, %%xmm2\n\t" // 修正参数顺序: xmm2 = [xmm1_low, xmm0_high]
        "vmovups %%xmm2, %0"          // 存储结果到dst
        : "=m" (*(float (*)[4])dst)
        : "m" (*(const float (*)[4])src1), "m" (*(const float (*)[2])(src1+2))
        : "xmm0", "xmm1", "xmm2"
    );
    
    // 更新期望值: 结果应为 [src1[2], src1[3], src1[0], src1[1]]
    expected[0] = src1[2];
    expected[1] = src1[3];
    expected[2] = src1[0];
    expected[3] = src1[1];
    
    printf("Src: %.1f, %.1f, %.1f, %.1f\n", src1[0], src1[1], src1[2], src1[3]);
    printf("Dst: %.1f, %.1f, %.1f, %.1f\n", dst[0], dst[1], dst[2], dst[3]);
    
    if(fabs(dst[0] - expected[0]) < 0.001 && 
       fabs(dst[1] - expected[1]) < 0.001 &&
       fabs(dst[2] - expected[2]) < 0.001 && 
       fabs(dst[3] - expected[3]) < 0.001) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
}

int main() {
    test_vmovhlps();
    return 0;
}
