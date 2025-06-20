#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// 打印标志位
static void print_flags(const char* name, int flags) {
    printf("%s: ZF=%d PF=%d CF=%d\n", name, 
           (flags>>6)&1, (flags>>2)&1, (flags>>0)&1);
}

// 测试VUCOMISS指令
static void test_vucomiss(const char* name, float a, float b) {
    int flags;
    
    asm volatile(
        "vucomiss %[b], %[a]\n\t"
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
    float normal1 = 1.23456789f;
    float normal2 = -9.87654321f;
    float zero = 0.0f;
    float inf = INFINITY;
    float nan = NAN;
    
    // 正常值比较
    test_vucomiss("Normal > Normal", normal1, normal2);
    test_vucomiss("Normal < Normal", normal2, normal1);
    test_vucomiss("Normal == Normal", normal1, normal1);
    
    // 零值比较
    test_vucomiss("Zero == Zero", zero, zero);
    test_vucomiss("Normal > Zero", normal1, zero);
    test_vucomiss("Normal < Zero", normal2, zero);
    
    // 无穷大比较
    test_vucomiss("Inf == Inf", inf, inf);
    test_vucomiss("Inf > Normal", inf, normal1);
    test_vucomiss("Normal < Inf", normal1, inf);
    
    // NaN比较
    test_vucomiss("NaN vs Normal", nan, normal1);
    test_vucomiss("Normal vs NaN", normal1, nan);
    test_vucomiss("NaN vs NaN", nan, nan);
    test_vucomiss("NaN vs Inf", nan, inf);
    test_vucomiss("Inf vs NaN", inf, nan);
    
    return 0;
}
