#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// 打印标志位
static void print_flags(const char* name, int flags) {
    printf("%s: ZF=%d PF=%d CF=%d\n", name, 
           (flags>>6)&1, (flags>>2)&1, (flags>>0)&1);
}

// 测试VUCOMISD指令
static void test_vucomisd(const char* name, double a, double b) {
    int flags;
    
    asm volatile(
        "vucomisd %[b], %[a]\n\t"
        "pushfq\n\t"
        "popq %%rax\n\t"
        "movl %%eax, %[flags]"
        : [flags] "=r"(flags)
        : [a] "x"(a), [b] "xm"(b)
        : "rax", "cc");
    
    printf("\nTest: %s\n", name);
    printf("a=%.15g b=%.15g\n", a, b);
    print_flags("Result flags", flags);
}

int main() {
    // 测试数据
    double normal1 = 1.23456789;
    double normal2 = -9.87654321;
    double zero = 0.0;
    double inf = INFINITY;
    double nan = NAN;
    
    // 正常值比较
    test_vucomisd("Normal > Normal", normal1, normal2);
    test_vucomisd("Normal < Normal", normal2, normal1);
    test_vucomisd("Normal == Normal", normal1, normal1);
    
    // 零值比较
    test_vucomisd("Zero == Zero", zero, zero);
    test_vucomisd("Normal > Zero", normal1, zero);
    test_vucomisd("Normal < Zero", normal2, zero);
    
    // 无穷大比较
    test_vucomisd("Inf == Inf", inf, inf);
    test_vucomisd("Inf > Normal", inf, normal1);
    test_vucomisd("Normal < Inf", normal1, inf);
    
    // NaN比较
    test_vucomisd("NaN vs Normal", nan, normal1);
    test_vucomisd("Normal vs NaN", normal1, nan);
    test_vucomisd("NaN vs NaN", nan, nan);
    test_vucomisd("NaN vs Inf", nan, inf);
    test_vucomisd("Inf vs NaN", inf, nan);
    
    return 0;
}
