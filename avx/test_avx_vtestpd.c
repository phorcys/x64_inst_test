#include <stdio.h>
#include <stdint.h>
#include "avx.h"

// 打印标志寄存器
static void print_flags(const char* name, uint64_t flags) {
    printf("%s: CF=%lu ZF=%lu\n", name, 
           (flags >> 0) & 1,  // CF
           (flags >> 6) & 1); // ZF
}

// vtestpd 指令测试
static int test_vtestpd() {
    printf("--- Testing vtestpd ---\n");
    
    // 测试数据
    ALIGNED(32) double src1[4] = {1.0, 0.0, -2.5, __builtin_nan("")};
    ALIGNED(32) double src2[4] = {1.0, __builtin_inf(), -2.5, 0.0};
    ALIGNED(32) double src3[4] = {0.0, 0.0, 0.0, 0.0};
    ALIGNED(32) double src4[4] = {__builtin_inf(), __builtin_inf(), __builtin_inf(), __builtin_inf()};
    
    int total_errors = 0;
    
    // 256位版本测试
    printf("\nTesting 256-bit vtestpd:\n");
    
    // 测试1: 正常值
    uint64_t flags1_before, flags1_after;
    __asm__ __volatile__("pushfq\n\tpop %0" : "=r" (flags1_before) :: "memory");
    __asm__ __volatile__(
        "vmovapd %0, %%ymm0\n\t"
        "vmovapd %1, %%ymm1\n\t"
        "vtestpd %%ymm1, %%ymm0\n\t"
        : 
        : "m" (*src1), "m" (*src2)
        : "ymm0", "ymm1", "cc"
    );
    __asm__ __volatile__("pushfq\n\tpop %0" : "=r" (flags1_after) :: "memory");
    print_flags("  Before", flags1_before);
    print_flags("  After", flags1_after);
    
    // 测试2: 全零测试
    uint64_t flags2_before, flags2_after;
    __asm__ __volatile__("pushfq\n\tpop %0" : "=r" (flags2_before) :: "memory");
    __asm__ __volatile__(
        "vmovapd %0, %%ymm0\n\t"
        "vmovapd %1, %%ymm1\n\t"
        "vtestpd %%ymm1, %%ymm0\n\t"
        : 
        : "m" (*src3), "m" (*src4)
        : "ymm0", "ymm1", "cc"
    );
    __asm__ __volatile__("pushfq\n\tpop %0" : "=r" (flags2_after) :: "memory");
    print_flags("  Zero test Before", flags2_before);
    print_flags("  Zero test After", flags2_after);
    
    // 128位版本测试
    printf("\nTesting 128-bit vtestpd:\n");
    
    // 测试1: 正常值
    uint64_t flags3_before, flags3_after;
    __asm__ __volatile__("pushfq\n\tpop %0" : "=r" (flags3_before) :: "memory");
    __asm__ __volatile__(
        "vmovapd %0, %%xmm0\n\t"
        "vmovapd %1, %%xmm1\n\t"
        "vtestpd %%xmm1, %%xmm0\n\t"
        : 
        : "m" (*src1), "m" (*src2)
        : "xmm0", "xmm1", "cc"
    );
    __asm__ __volatile__("pushfq\n\tpop %0" : "=r" (flags3_after) :: "memory");
    print_flags("  Before", flags3_before);
    print_flags("  After", flags3_after);
    
    // 测试2: 全零测试
    uint64_t flags4_before, flags4_after;
    __asm__ __volatile__("pushfq\n\tpop %0" : "=r" (flags4_before) :: "memory");
    __asm__ __volatile__(
        "vmovapd %0, %%xmm0\n\t"
        "vmovapd %1, %%xmm1\n\t"
        "vtestpd %%xmm1, %%xmm0\n\t"
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
        printf("\nAll vtestpd tests passed!\n");
    } else {
        printf("\nSome vtestpd tests failed!\n");
        total_errors++;
    }
    
    return total_errors;
}

int main() {
    int errors = test_vtestpd();
    return errors;
}
