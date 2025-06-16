#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// vfmsub231pd指令测试
void test_vfmsub231pd() {
    printf("--- Testing vfmsub231pd (Fused Multiply-Subtract of Packed Double-Precision Floating-Point Values) ---\n");
    
    // 测试数据
    double a[4] ALIGNED(32) = {1.0, 2.0, 3.0, 4.0};
    double b[4] ALIGNED(32) = {5.0, 6.0, 7.0, 8.0}; 
    double c[4] ALIGNED(32) = {0.1, 0.2, 0.3, 0.4};
    double res[4] ALIGNED(32) = {0};
    
    // 预期结果 (vfmsub231pd: res = a*b - c)
    double expected[4] = {
        a[0]*b[0]-c[0],
        a[1]*b[1]-c[1],
        a[2]*b[2]-c[2],
        a[3]*b[3]-c[3]
    };
    
    // 测试128位版本
    printf("\n[128-bit test]\n");
    __asm__ __volatile__(
        "vmovapd %1, %%xmm0\n\t"     // 加载a到xmm0
        "vmovapd %2, %%xmm1\n\t"     // 加载b到xmm1
        "vmovapd %3, %%xmm2\n\t"     // 加载c到xmm2
        "vfmsub231pd %%xmm1, %%xmm0, %%xmm2\n\t"  // xmm2 = xmm0*xmm1 - xmm2
        "vmovapd %%xmm2, %0\n\t"     // 存回结果
        : "=m"(res[0])
        : "m"(a[0]), "m"(b[0]), "m"(c[0])
        : "xmm0", "xmm1", "xmm2"
    );
    
    print_double_vec("Input a", a, 2);
    print_double_vec("Input b", b, 2);
    print_double_vec("Input c", c, 2);
    print_double_vec("Result", res, 2);
    print_double_vec("Expected", expected, 2);
    
    // 验证结果
    for(int i=0; i<2; i++) {
        if(!double_equal(res[i], expected[i], 1e-10)) {
            printf("Mismatch at index %d: got %f, expected %f\n", i, res[i], expected[i]);
        }
    }
    
    // 测试256位版本
    printf("\n[256-bit test]\n");
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"     // 加载a到ymm0
        "vmovapd %2, %%ymm1\n\t"     // 加载b到ymm1
        "vmovapd %3, %%ymm2\n\t"     // 加载c到ymm2
        "vfmsub231pd %%ymm1, %%ymm0, %%ymm2\n\t"  // ymm2 = ymm0*ymm1 - ymm2
        "vmovapd %%ymm2, %0\n\t"     // 存回结果
        : "=m"(res[0])
        : "m"(a[0]), "m"(b[0]), "m"(c[0])
        : "ymm0", "ymm1", "ymm2"
    );
    
    print_double_vec("Input a", a, 4);
    print_double_vec("Input b", b, 4);
    print_double_vec("Input c", c, 4);
    print_double_vec("Result", res, 4);
    print_double_vec("Expected", expected, 4);
    
    // 验证结果
    for(int i=0; i<4; i++) {
        if(!double_equal(res[i], expected[i], 1e-10)) {
            printf("Mismatch at index %d: got %f, expected %f\n", i, res[i], expected[i]);
        }
    }
    
    // 测试特殊值
    printf("\n[Special values test]\n");
    double spec_a[4] ALIGNED(32) = {INFINITY, -INFINITY, NAN, 0.0};
    double spec_b[4] ALIGNED(32) = {1.0, 1.0, 1.0, 1.0};
    double spec_c[4] ALIGNED(32) = {1.0, 1.0, 1.0, 1.0};
    double spec_res[4] ALIGNED(32) = {0};
    
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"
        "vmovapd %2, %%ymm1\n\t"
        "vmovapd %3, %%ymm2\n\t"
        "vfmsub231pd %%ymm1, %%ymm0, %%ymm2\n\t"
        "vmovapd %%ymm2, %0\n\t"
        : "=m"(spec_res[0])
        : "m"(spec_a[0]), "m"(spec_b[0]), "m"(spec_c[0])
        : "ymm0", "ymm1", "ymm2"
    );
    
    print_double_vec_hex("Special a", spec_a, 4);
    print_double_vec_hex("Special b", spec_b, 4);
    print_double_vec_hex("Special c", spec_c, 4);
    print_double_vec_hex("Special result", spec_res, 4);
    
    // 测试不同舍入模式
    printf("\n[Rounding modes test]\n");
    uint32_t mxcsr = get_mxcsr();
    for(int i=0; i<4; i++) {  // 测试4种舍入模式
        uint32_t new_mxcsr = (mxcsr & ~0x6000) | (i << 13);
        set_mxcsr(new_mxcsr);
        
        double rnd_a[4] ALIGNED(32) = {1.5, 2.5, 3.5, 4.5};
        double rnd_b[4] ALIGNED(32) = {1.0, 1.0, 1.0, 1.0};
        double rnd_c[4] ALIGNED(32) = {0.1, 0.1, 0.1, 0.1};
        double rnd_res[4] ALIGNED(32) = {0};
        
        __asm__ __volatile__(
            "vmovapd %1, %%ymm0\n\t"
            "vmovapd %2, %%ymm1\n\t"
            "vmovapd %3, %%ymm2\n\t"
            "vfmsub231pd %%ymm1, %%ymm0, %%ymm2\n\t"
            "vmovapd %%ymm2, %0\n\t"
            : "=m"(rnd_res[0])
            : "m"(rnd_a[0]), "m"(rnd_b[0]), "m"(rnd_c[0])
            : "ymm0", "ymm1", "ymm2"
        );
        
        printf("Rounding mode %d:\n", i);
        print_mxcsr(new_mxcsr);
        print_double_vec("Result", rnd_res, 4);
    }
    set_mxcsr(mxcsr);  // 恢复原始MXCSR
}

int main() {
    test_vfmsub231pd();
    return 0;
}
