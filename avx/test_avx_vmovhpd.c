#include "avx.h"
#include <stdio.h>
#include <math.h>

// VMOVHPD 指令测试
void test_vmovhpd() {
    printf("=== Testing VMOVHPD ===\n");
    
    // 测试1: 从内存加载到寄存器高64位
    printf("\nTest 1: Load high 64-bit from memory\n");
    double src1[2] = {1.1, 2.2};
    double mem_value = 3.3;
    double dst1[2] = {0};
    double expected1[2] = {1.1, 3.3}; // 低位不变，高位更新
    
    __asm__ __volatile__ (
        "vmovupd %1, %%xmm0\n\t"      // 加载src1到xmm0
        "vmovhpd %2, %%xmm0, %%xmm0\n\t" // 将内存值加载到xmm0高64位
        "vmovupd %%xmm0, %0"          // 存储结果到dst1
        : "=m" (*(double (*)[2])dst1)
        : "m" (*(const double (*)[2])src1), "m" (mem_value)
        : "xmm0"
    );
    
    printf("Src: %.1f, %.1f\n", src1[0], src1[1]);
    printf("Mem: %.1f\n", mem_value);
    printf("Dst: %.1f, %.1f\n", dst1[0], dst1[1]);
    
    if(fabs(dst1[0] - expected1[0]) < 0.001 && 
       fabs(dst1[1] - expected1[1]) < 0.001) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 将寄存器高64位存储到内存
    printf("\nTest 2: Store high 64-bit to memory\n");
    double src2[2] = {4.4, 5.5};
    double mem_dst = 0;
    double expected_mem = 5.5; // 高64位的值
    
    __asm__ __volatile__ (
        "vmovupd %1, %%xmm0\n\t"      // 加载src2到xmm0
        "vmovhpd %%xmm0, %0"          // 将xmm0高64位存储到内存
        : "=m" (mem_dst)
        : "m" (*(const double (*)[2])src2)
        : "xmm0"
    );
    
    printf("Src: %.1f, %.1f\n", src2[0], src2[1]);
    printf("Mem: %.1f\n", mem_dst);
    
    if(fabs(mem_dst - expected_mem) < 0.001) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vmovhpd();
    return 0;
}
