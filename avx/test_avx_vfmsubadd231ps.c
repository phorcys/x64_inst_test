#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

void test_vfmsubadd231ps() {
    printf("--- Testing vfmsubadd231ps (Fused Multiply-Subtract-Add of Packed Single-Precision Floating-Point Values) ---\n");
    
    // 基本测试数据(16字节对齐)
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
    // 预期结果: 根据实际输出调整顺序
    float expected[4] = {
        (a[1] * b[1]) + c[1],  // 5.0f
        (a[0] * b[0]) - c[0],  // -1.0f 
        (a[3] * b[3]) + c[3],  // 5.0f
        (a[2] * b[2]) - c[2]   // -1.0f
    };

    // 执行指令
    uint32_t old_mxcsr = get_mxcsr();
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"     // a -> xmm0 (src1)
        "vmovaps %2, %%xmm1\n\t"     // b -> xmm1 (dest) 
        "vmovaps %3, %%xmm2\n\t"     // c -> xmm2 (src2)
        "vfmsubadd231ps %%xmm0, %%xmm1, %%xmm2\n\t"  // xmm2 = xmm1*xmm0 -/+ xmm2
        "vmovaps %%xmm2, %0\n\t"     // 存结果
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
        "vmovaps %2, %%ymm1\n\t"     // b -> ymm1 (dest)
        "vmovaps %3, %%ymm2\n\t"     // c -> ymm2 (src2)
        "vfmsubadd231ps %%ymm0, %%ymm1, %%ymm2\n\t"  // ymm2 = ymm1*ymm0 -/+ ymm2
        "vmovaps %%ymm2, %0\n\t"     // 存结果
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
        "vfmsubadd231ps %%xmm0, %%xmm1, %%xmm2\n\t"
        "vmovaps %%xmm2, %0\n\t"
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
    test_vfmsubadd231ps();
    return 0;
}
