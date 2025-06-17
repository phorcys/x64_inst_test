#include "avx.h"
#include <stdio.h>
#include <string.h>

// VMOVDDUP 指令测试
void test_vmovddup() {
    printf("=== Testing VMOVDDUP ===\n");
    
    // 测试1: 128位版本
    printf("\nTest 1: 128-bit version\n");
    double src1[2] = {1.1, 2.2};
    double dst1[2] = {0};
    double expected1[2] = {1.1, 1.1}; // 复制低位元素
    
    asm volatile(
        "vmovddup %1, %%xmm0\n\t"
        "movupd %%xmm0, %0"
        : "=m" (*(double (*)[2])dst1)
        : "m" (*(double (*)[2])src1)
        : "xmm0"
    );
    
    printf("Src: %.1f, %.1f\n", src1[0], src1[1]);
    printf("Dst: %.1f, %.1f\n", dst1[0], dst1[1]);
    
    if(dst1[0] == expected1[0] && dst1[1] == expected1[1]) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 256位版本
    printf("\nTest 2: 256-bit version\n");
    double src2[4] = {1.1, 2.2, 3.3, 4.4};
    double dst2[4] = {0};
    double expected2[4] = {1.1, 1.1, 3.3, 3.3}; // 复制每个64位元素
    
    asm volatile(
        "vmovddup %1, %%ymm0\n\t"
        "vmovupd %%ymm0, %0"
        : "=m" (*(double (*)[4])dst2)
        : "m" (*(double (*)[4])src2)
        : "ymm0"
    );
    
    printf("Src: %.1f, %.1f, %.1f, %.1f\n", 
           src2[0], src2[1], src2[2], src2[3]);
    printf("Dst: %.1f, %.1f, %.1f, %.1f\n", 
           dst2[0], dst2[1], dst2[2], dst2[3]);
    
    if(memcmp(dst2, expected2, sizeof(dst2)) == 0) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
    
    // 测试3: 特殊值测试 (NaN)
    printf("\nTest 3: Special values (NaN)\n");
    double nan = 0.0/0.0;
    double src3[2] = {nan, 0.0}; // NaN, 0
    double dst3[2] = {0};
    
    asm volatile(
        "vmovddup %1, %%xmm0\n\t"
        "movupd %%xmm0, %0"
        : "=m" (*(double (*)[2])dst3)
        : "m" (*(double (*)[2])src3)
        : "xmm0"
    );
    
    int nan_ok0 = isnan(dst3[0]);
    int nan_ok1 = isnan(dst3[1]);
    
    printf("Dst: %f, %f\n", dst3[0], dst3[1]);
    printf("Checks: [0] is nan:%d, [1] is nan:%d\n", nan_ok0, nan_ok1);
    
    if(nan_ok0 && nan_ok1) {
        printf("Test 3 PASSED\n");
    } else {
        printf("Test 3 FAILED\n");
    }
}

int main() {
    test_vmovddup();
    return 0;
}
