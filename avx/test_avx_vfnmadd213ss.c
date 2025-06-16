#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

void test_vfnmadd213ss() {
    printf("--- Testing vfnmadd213ss (Fused Negative Multiply-Add of Scalar Single-Precision Floating-Point Values) ---\n");
    
    // 测试数据(32字节对齐)
    float a[4] __attribute__((aligned(32))) = {1.0f, 1.0f, 1.0f, 1.0f};
    float b[4] __attribute__((aligned(32))) = {2.0f, 2.0f, 2.0f, 2.0f};
    float c[4] __attribute__((aligned(32))) = {3.0f, 3.0f, 3.0f, 3.0f};
    float res[4] __attribute__((aligned(32))) = {0};
    
    // 特殊值测试数据
    float special_a[4] = {NAN, INFINITY, -INFINITY, 0.0f};
    float special_b[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float special_c[4] = {1.0f, INFINITY, -INFINITY, 0.0f};

    // 预期结果: -(a * b) + c
    float expected = -(a[0] * b[0]) + c[0];

    // 执行指令
    uint32_t old_mxcsr = get_mxcsr();
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"     // a -> xmm0 (src1)
        "vmovaps %2, %%xmm1\n\t"     // b -> xmm1 (src2/dest)
        "vmovaps %3, %%xmm2\n\t"     // c -> xmm2 (src3)
        "vfnmadd213ss %%xmm2, %%xmm0, %%xmm1\n\t"  // xmm1 = -(xmm1*xmm0) + xmm2
        "vmovaps %%xmm1, %0\n\t"     // 存结果
        : "=m"(res)
        : "m"(a), "m"(b), "m"(c)
        : "xmm0", "xmm1", "xmm2"
    );

    // 打印结果和MXCSR状态
    print_mxcsr(get_mxcsr());
    printf("MXCSR changed: 0x%08X -> 0x%08X\n", old_mxcsr, get_mxcsr());
    
    print_float_vec("Input a", a, 1);
    print_float_vec("Input b", b, 1);
    print_float_vec("Input c", c, 1);
    print_float_vec("Result", res, 1);
    printf("Expected: %f\n", expected);

    // 验证结果
    if(!float_equal(res[0], expected, 1e-6f)) {
        printf("Mismatch: got %f (0x%08x), expected %f (0x%08x)\n", 
              res[0], *(uint32_t*)&res[0], expected, *(uint32_t*)&expected);
    } else {
        printf("Result matches expected value\n");
    }

    // 测试特殊值
    printf("\n[Special values test]\n");
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vmovaps %3, %%xmm2\n\t"
        "vfnmadd213ss %%xmm2, %%xmm0, %%xmm1\n\t"
        "vmovaps %%xmm1, %0\n\t"
        : "=m"(res)
        : "m"(special_a), "m"(special_b), "m"(special_c)
        : "xmm0", "xmm1", "xmm2"
    );

    print_hex_float_vec("Special inputs a", special_a, 4);
    print_hex_float_vec("Special inputs b", special_b, 4);
    print_hex_float_vec("Special inputs c", special_c, 4);
    print_hex_float_vec("Special results", res, 4);
    print_mxcsr(get_mxcsr());
}

int main() {
    test_vfnmadd213ss();
    return 0;
}
