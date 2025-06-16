#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

void test_vfnmadd213ps() {
    printf("--- Testing vfnmadd213ps (Fused Negative Multiply-Add of Packed Single-Precision Floating-Point Values) ---\n");
    
    // 测试数据(32字节对齐)
    float a[8] __attribute__((aligned(32))) = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float b[8] __attribute__((aligned(32))) = {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f};
    float c[8] __attribute__((aligned(32))) = {3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f};
    float res[8] __attribute__((aligned(32))) = {0};
    
    // 特殊值测试数据
    float special_a[8] = {NAN, INFINITY, -INFINITY, 0.0f, -0.0f, FLT_MIN, FLT_MAX, 1.0f};
    float special_b[8] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 2.0f, 0.0f};
    float special_c[8] = {1.0f, INFINITY, -INFINITY, 0.0f, -0.0f, FLT_MIN, FLT_MAX, 1.0f};

    // 测试128位版本
    printf("\n[128-bit version test]\n");
    // 预期结果: -(a * b) + c
    float expected[4] = {
        -(a[0] * b[0]) + c[0],
        -(a[1] * b[1]) + c[1],
        -(a[2] * b[2]) + c[2],
        -(a[3] * b[3]) + c[3]
    };

    // 执行指令
    uint32_t old_mxcsr = get_mxcsr();
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"     // a -> xmm0 (src1)
        "vmovaps %2, %%xmm1\n\t"     // b -> xmm1 (src2/dest)
        "vmovaps %3, %%xmm2\n\t"     // c -> xmm2 (src3)
        "vfnmadd213ps %%xmm2, %%xmm0, %%xmm1\n\t"  // xmm1 = -(xmm1*xmm0) + xmm2
        "vmovaps %%xmm1, %0\n\t"     // 存结果
        : "=m"(res)
        : "m"(a), "m"(b), "m"(c)
        : "xmm0", "xmm1", "xmm2"
    );

    // 打印结果和MXCSR状态
    print_mxcsr(get_mxcsr());
    printf("MXCSR changed: 0x%08X -> 0x%08X\n", old_mxcsr, get_mxcsr());
    
    print_float_vec("Input a", a, 4);
    print_float_vec("Input b", b, 4);
    print_float_vec("Input c", c, 4);
    print_float_vec("Result", res, 4);
    print_float_vec("Expected", expected, 4);

    // 验证结果
    int match = 1;
    for(int i=0; i<4; i++) {
        if(!float_equal(res[i], expected[i], 1e-6f)) {
            printf("Mismatch at index %d: got %f (0x%08X), expected %f (0x%08X)\n", 
                  i, res[i], *(uint32_t*)&res[i], expected[i], *(uint32_t*)&expected[i]);
            match = 0;
        }
    }
    if(match) {
        printf("All elements match\n");
    }

    // 测试256位版本
    printf("\n[256-bit version test]\n");
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"     // a -> ymm0 (src1)
        "vmovaps %2, %%ymm1\n\t"     // b -> ymm1 (src2/dest)
        "vmovaps %3, %%ymm2\n\t"     // c -> ymm2 (src3)
        "vfnmadd213ps %%ymm2, %%ymm0, %%ymm1\n\t"  // ymm1 = -(ymm1*ymm0) + ymm2
        "vmovaps %%ymm1, %0\n\t"     // 存结果
        : "=m"(res)
        : "m"(a), "m"(b), "m"(c)
        : "ymm0", "ymm1", "ymm2"
    );

    // 打印256位结果
    print_vector256("256-bit result", _mm256_load_ps(res));

    // 测试特殊值
    printf("\n[Special values test]\n");
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vmovaps %3, %%xmm2\n\t"
        "vfnmadd213ps %%xmm2, %%xmm0, %%xmm1\n\t"
        "vmovaps %%xmm1, %0\n\t"
        : "=m"(res)
        : "m"(special_a), "m"(special_b), "m"(special_c)
        : "xmm0", "xmm1", "xmm2"
    );

    print_hex_float_vec("Special inputs a", special_a, 8);
    print_hex_float_vec("Special inputs b", special_b, 8);
    print_hex_float_vec("Special inputs c", special_c, 8);
    print_hex_float_vec("Special results", res, 8);
    print_mxcsr(get_mxcsr());
}

int main() {
    test_vfnmadd213ps();
    return 0;
}
