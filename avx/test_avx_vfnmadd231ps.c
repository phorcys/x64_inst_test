#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// vfnmadd231ps指令测试
void test_vfnmadd231ps() {
    printf("--- Testing vfnmadd231ps (Fused Negative Multiply-Add of Packed Single-Precision Floating-Point Values) ---\n");
    
    // 测试数据(32字节对齐)
    float a[8] ALIGNED(32) = {1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f, 8.5f};
    float b[8] ALIGNED(32) = {0.5f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f};
    float c[8] ALIGNED(32) = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float res[8] ALIGNED(32) = {0};
    
    // 预期结果 (vfnmadd231ps: res = -(a*b) + c)
    float expected[8] = {
        -(a[0]*b[0]) + c[0],
        -(a[1]*b[1]) + c[1],
        -(a[2]*b[2]) + c[2],
        -(a[3]*b[3]) + c[3],
        -(a[4]*b[4]) + c[4],
        -(a[5]*b[5]) + c[5],
        -(a[6]*b[6]) + c[6],
        -(a[7]*b[7]) + c[7]
    };
    
    // 测试256位版本
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"     // 加载a到ymm0
        "vmovaps %2, %%ymm1\n\t"     // 加载b到ymm1
        "vmovaps %3, %%ymm2\n\t"     // 加载c到ymm2
        "vfnmadd231ps %%ymm1, %%ymm0, %%ymm2\n\t"  // ymm2 = -(ymm0*ymm1) + ymm2
        "vmovaps %%ymm2, %0\n\t"     // 存回结果
        : "=m"(res[0])
        : "m"(a[0]), "m"(b[0]), "m"(c[0])
        : "ymm0", "ymm1", "ymm2"
    );
    
    printf("[256-bit test]\n");
    print_float_vec("Input a", a, 8);
    print_float_vec("Input b", b, 8);
    print_float_vec("Input c", c, 8);
    print_float_vec("Result", res, 8);
    print_float_vec("Expected", expected, 8);
    
    // 验证结果
    for(int i=0; i<8; i++) {
        if(!float_equal(res[i], expected[i], 1e-5f)) {
            printf("Mismatch at index %d: got %f, expected %f\n", i, res[i], expected[i]);
        }
    }
    
    // 测试128位版本
    float res128[4] ALIGNED(16) = {0};
    float expected128[4] = {
        -(a[0]*b[0]) + c[0],
        -(a[1]*b[1]) + c[1],
        -(a[2]*b[2]) + c[2],
        -(a[3]*b[3]) + c[3]
    };
    
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"
        "vmovaps %2, %%xmm1\n\t"
        "vmovaps %3, %%xmm2\n\t"
        "vfnmadd231ps %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovaps %%xmm2, %0\n\t"
        : "=m"(res128[0])
        : "m"(a[0]), "m"(b[0]), "m"(c[0])
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("\n[128-bit test]\n");
    print_float_vec("Result", res128, 4);
    print_float_vec("Expected", expected128, 4);
    
    // 测试特殊值
    printf("\n[Special values test]\n");
    float spec_a[8] ALIGNED(32) = {INFINITY, -INFINITY, NAN, 0.0f, 1.0f, -1.0f, 2.0f, -2.0f};
    float spec_b[8] ALIGNED(32) = {1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -0.0f, -0.0f};
    float spec_c[8] ALIGNED(32) = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float spec_res[8] ALIGNED(32) = {0};
    
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"
        "vmovaps %2, %%ymm1\n\t"
        "vmovaps %3, %%ymm2\n\t"
        "vfnmadd231ps %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovaps %%ymm2, %0\n\t"
        : "=m"(spec_res[0])
        : "m"(spec_a[0]), "m"(spec_b[0]), "m"(spec_c[0])
        : "ymm0", "ymm1", "ymm2"
    );
    
    print_hex_float_vec("Special a", spec_a, 8);
    print_hex_float_vec("Special b", spec_b, 8);
    print_hex_float_vec("Special c", spec_c, 8);
    print_hex_float_vec("Special result", spec_res, 8);
}

int main() {
    test_vfnmadd231ps();
    return 0;
}
