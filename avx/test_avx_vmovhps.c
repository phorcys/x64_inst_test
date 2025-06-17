#include "avx.h"
#include <stdio.h>
#include <math.h>

// VMOVHPS 指令测试
void test_vmovhps() {
    printf("=== Testing VMOVHPS ===\n");
    
    // 测试1: 从内存加载到寄存器高64位
    printf("\nTest 1: Load high 64-bit from memory\n");
    float src1[4] = {1.1f, 2.2f, 3.3f, 4.4f};
    float mem_value[2] = {5.5f, 6.6f};
    float dst1[4] = {0};
    float expected1[4] = {1.1f, 2.2f, 5.5f, 6.6f}; // 低64位不变，高64位更新
    
    __asm__ __volatile__ (
        "vmovups %1, %%xmm0\n\t"      // 加载src1到xmm0
        "vmovhps %2, %%xmm0, %%xmm0\n\t" // 将内存值加载到xmm0高64位
        "vmovups %%xmm0, %0"          // 存储结果到dst1
        : "=m" (*(float (*)[4])dst1)
        : "m" (*(const float (*)[4])src1), "m" (*(const float (*)[2])mem_value)
        : "xmm0"
    );
    
    printf("Src: %.1f, %.1f, %.1f, %.1f\n", src1[0], src1[1], src1[2], src1[3]);
    printf("Mem: %.1f, %.1f\n", mem_value[0], mem_value[1]);
    printf("Dst: %.1f, %.1f, %.1f, %.1f\n", dst1[0], dst1[1], dst1[2], dst1[3]);
    
    if(fabsf(dst1[0] - expected1[0]) < 0.001f && 
       fabsf(dst1[1] - expected1[1]) < 0.001f &&
       fabsf(dst1[2] - expected1[2]) < 0.001f && 
       fabsf(dst1[3] - expected1[3]) < 0.001f) {
        printf("Test 1 PASSED\n");
    } else {
        printf("Test 1 FAILED\n");
    }
    
    // 测试2: 将寄存器高64位存储到内存
    printf("\nTest 2: Store high 64-bit to memory\n");
    float src2[4] = {7.7f, 8.8f, 9.9f, 10.10f};
    float mem_dst[2] = {0};
    float expected_mem[2] = {9.9f, 10.10f}; // 高64位的值
    
    __asm__ __volatile__ (
        "vmovups %1, %%xmm0\n\t"      // 加载src2到xmm0
        "vmovhps %%xmm0, %0"          // 将xmm0高64位存储到内存
        : "=m" (*(float (*)[2])mem_dst)
        : "m" (*(const float (*)[4])src2)
        : "xmm0"
    );
    
    printf("Src: %.1f, %.1f, %.1f, %.1f\n", src2[0], src2[1], src2[2], src2[3]);
    printf("Mem: %.1f, %.1f\n", mem_dst[0], mem_dst[1]);
    
    if(fabsf(mem_dst[0] - expected_mem[0]) < 0.001f && 
       fabsf(mem_dst[1] - expected_mem[1]) < 0.001f) {
        printf("Test 2 PASSED\n");
    } else {
        printf("Test 2 FAILED\n");
    }
}

int main() {
    test_vmovhps();
    return 0;
}
