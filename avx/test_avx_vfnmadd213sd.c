#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

void test_vfnmadd213sd() {
    printf("--- Testing vfnmadd213sd (Fused Negative Multiply-Add of Scalar Double-Precision Floating-Point Values) ---\n");
    
    // 测试数据(32字节对齐)
    double a[4] __attribute__((aligned(32))) = {1.0, 1.0, 1.0, 1.0};
    double b[4] __attribute__((aligned(32))) = {2.0, 2.0, 2.0, 2.0};
    double c[4] __attribute__((aligned(32))) = {3.0, 3.0, 3.0, 3.0};
    double res[4] __attribute__((aligned(32))) = {0};
    
    // 特殊值测试数据
    double special_a[4] = {NAN, INFINITY, -INFINITY, 0.0};
    double special_b[4] = {1.0, 1.0, 1.0, 1.0};
    double special_c[4] = {1.0, INFINITY, -INFINITY, 0.0};

    // 预期结果: -(a * b) + c
    double expected = -(a[0] * b[0]) + c[0];

    // 执行指令
    uint32_t old_mxcsr = get_mxcsr();
    __asm__ __volatile__(
        "vmovapd %1, %%xmm0\n\t"     // a -> xmm0 (src1)
        "vmovapd %2, %%xmm1\n\t"     // b -> xmm1 (src2/dest)
        "vmovapd %3, %%xmm2\n\t"     // c -> xmm2 (src3)
        "vfnmadd213sd %%xmm2, %%xmm0, %%xmm1\n\t"  // xmm1 = -(xmm1*xmm0) + xmm2
        "vmovapd %%xmm1, %0\n\t"     // 存结果
        : "=m"(res)
        : "m"(a), "m"(b), "m"(c)
        : "xmm0", "xmm1", "xmm2"
    );

    // 打印结果和MXCSR状态
    print_mxcsr(get_mxcsr());
    printf("MXCSR changed: 0x%08X -> 0x%08X\n", old_mxcsr, get_mxcsr());
    
    print_double_vec("Input a", a, 1);
    print_double_vec("Input b", b, 1);
    print_double_vec("Input c", c, 1);
    print_double_vec("Result", res, 1);
    printf("Expected: %f\n", expected);

    // 验证结果
    if(!double_equal(res[0], expected, 1e-12)) {
        printf("Mismatch: got %f (0x%016lx), expected %f (0x%016lx)\n", 
              res[0], *(uint64_t*)&res[0], expected, *(uint64_t*)&expected);
    } else {
        printf("Result matches expected value\n");
    }

    // 测试特殊值
    printf("\n[Special values test]\n");
    __asm__ __volatile__(
        "vmovapd %1, %%xmm0\n\t"
        "vmovapd %2, %%xmm1\n\t"
        "vmovapd %3, %%xmm2\n\t"
        "vfnmadd213sd %%xmm2, %%xmm0, %%xmm1\n\t"
        "vmovapd %%xmm1, %0\n\t"
        : "=m"(res)
        : "m"(special_a), "m"(special_b), "m"(special_c)
        : "xmm0", "xmm1", "xmm2"
    );

    print_double_vec_hex("Special inputs a", special_a, 4);
    print_double_vec_hex("Special inputs b", special_b, 4);
    print_double_vec_hex("Special inputs c", special_c, 4);
    print_double_vec_hex("Special results", res, 4);
    print_mxcsr(get_mxcsr());
}

int main() {
    test_vfnmadd213sd();
    return 0;
}
