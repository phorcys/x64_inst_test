#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "avx.h"

// 打印标志寄存器
static void print_flags(const char* name, uint64_t flags) {
    printf("%s: CF=%lu ZF=%lu\n", name, 
           (flags >> 0) & 1,  // CF
           (flags >> 6) & 1); // ZF
}

// vtestps 指令测试
static int test_vtestps() {
    printf("--- Testing vtestps ---\n");
    
    // 测试数据
    ALIGNED(32) float src1[8] = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f
    };
    
    ALIGNED(32) float src2[8] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f
    };
    
    ALIGNED(32) float src3[8] = {0};
    ALIGNED(32) float src4[8] = {__builtin_inff()};
    
    int total_errors = 0;
    
    // 256位版本测试
    printf("\nTesting 256-bit vtestps:\n");
    
    // 测试1: 正常值
    uint64_t flags1_before, flags1_after;
    __asm__ __volatile__("pushq 0\n\t popfq\n\tpushfq\n\tpop %0" : "=r" (flags1_before) :: "memory");
    __asm__ __volatile__(
        "vmovaps %0, %%ymm0\n\t"
        "vmovaps %1, %%ymm1\n\t"
        "vtestps %%ymm1, %%ymm0\n\t"
        : 
        : "m" (*src1), "m" (*src2)
        : "ymm0", "ymm1", "cc"
    );
    __asm__ __volatile__("pushfq\n\tpop %0" : "=r" (flags1_after) :: "memory");
    print_flags("  Before", flags1_before);
    print_flags("  After", flags1_after);
    
    // 测试2: 全零测试
    uint64_t flags2_before, flags2_after;
    __asm__ __volatile__("pushq 0\n\t popfq\n\tpushfq\n\tpop %0" : "=r" (flags2_before) :: "memory");
    __asm__ __volatile__(
        "vmovaps %0, %%ymm0\n\t"
        "vmovaps %1, %%ymm1\n\t"
        "vtestps %%ymm1, %%ymm0\n\t"
        : 
        : "m" (*src3), "m" (*src4)
        : "ymm0", "ymm1", "cc"
    );
    __asm__ __volatile__("pushfq\n\tpop %0" : "=r" (flags2_after) :: "memory");
    print_flags("  Zero test Before", flags2_before);
    print_flags("  Zero test After", flags2_after);
    
    // 128位版本测试
    printf("\nTesting 128-bit vtestps:\n");
    
    // 测试1: 正常值
    uint64_t flags3_before, flags3_after;
    __asm__ __volatile__("pushq 0\n\t popfq\n\tpushfq\n\tpop %0" : "=r" (flags3_before) :: "memory");
    __asm__ __volatile__(
        "vmovaps %0, %%xmm0\n\t"
        "vmovaps %1, %%xmm1\n\t"
        "vtestps %%xmm1, %%xmm0\n\t"
        : 
        : "m" (*src1), "m" (*src2)
        : "xmm0", "xmm1", "cc"
    );
    __asm__ __volatile__("pushfq\n\tpop %0" : "=r" (flags3_after) :: "memory");
    print_flags("  Before", flags3_before);
    print_flags("  After", flags3_after);
    
    // 测试2: 全零测试
    uint64_t flags4_before, flags4_after;
    __asm__ __volatile__("pushq 0\n\t popfq\n\tpushfq\n\tpop %0" : "=r" (flags4_before) :: "memory");
    __asm__ __volatile__(
        "vmovaps %0, %%xmm0\n\t"
        "vmovaps %1, %%xmm1\n\t"
        "vtestps %%xmm1, %%xmm0\n\t"
        : 
        : "m" (*src3), "m" (*src4)
        : "xmm0", "xmm1", "cc"
    );
    __asm__ __volatile__("pushfq\n\tpop %0" : "=r" (flags4_after) :: "memory");
    print_flags("  Zero test Before", flags4_before);
    print_flags("  Zero test After", flags4_after);
    
    // 检查结果
    if ((flags1_before != flags1_after) && (flags2_before != flags2_after) &&
        (flags3_before != flags3_after) && (flags4_before != flags4_after)) {
        printf("\nAll vtestps tests passed!\n");
    } else {
        printf("\nSome vtestps tests failed!\n");
        total_errors++;
    }
    
    return total_errors;
}

int main() {
    int errors = test_vtestps();
    return errors;
}
