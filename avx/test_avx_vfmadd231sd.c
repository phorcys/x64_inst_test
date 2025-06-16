#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// vfmadd231sd指令测试
void test_vfmadd231sd() {
    printf("--- Testing vfmadd231sd (Fused Multiply-Add of Scalar Double-Precision Floating-Point Values) ---\n");
    
    // 测试数据
    double a = 1.5;
    double b = 2.5;
    double c = 0.5;
    double res = 0;
    
    // 预期结果
    double expected = a * b + c;
    
    // 测试指令
    __asm__ __volatile__(
        "vmovsd %1, %%xmm0\n\t"     // 加载a到xmm0
        "vmovsd %2, %%xmm1\n\t"     // 加载b到xmm1
        "vmovsd %3, %%xmm2\n\t"     // 加载c到xmm2
        "vfmadd231sd %%xmm1, %%xmm0, %%xmm2\n\t"  // xmm2 = xmm0*xmm1 + xmm2
        "vmovsd %%xmm2, %0\n\t"     // 存回结果
        : "=m"(res)
        : "m"(a), "m"(b), "m"(c)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Input a: %f\n", a);
    printf("Input b: %f\n", b);
    printf("Input c: %f\n", c);
    printf("Result: %f\n", res);
    printf("Expected: %f\n", expected);
    
    // 验证结果
    if(!double_equal(res, expected, 1e-12)) {
        printf("Mismatch: got %f, expected %f\n", res, expected);
    }
    
    // 测试特殊值
    printf("\n[Special values test]\n");
    double spec_values[][3] = {
        {INFINITY, 1.0, 1.0},
        {-INFINITY, 1.0, 1.0},
        {NAN, 1.0, 1.0},
        {0.0, 1.0, 1.0},
        {1.0e308, 2.0, 0.0}  // 测试溢出
    };
    
    for(size_t i=0; i<sizeof(spec_values)/sizeof(spec_values[0]); i++) {
        double spec_a = spec_values[i][0];
        double spec_b = spec_values[i][1];
        double spec_c = spec_values[i][2];
        double spec_res = 0;
        
        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vmovsd %3, %%xmm2\n\t"
            "vfmadd231sd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovsd %%xmm2, %0\n\t"
            : "=m"(spec_res)
            : "m"(spec_a), "m"(spec_b), "m"(spec_c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Test case %zu:\n", i+1);
        print_double_vec_hex("a", &spec_a, 1);
        print_double_vec_hex("b", &spec_b, 1);
        print_double_vec_hex("c", &spec_c, 1);
        print_double_vec_hex("result", &spec_res, 1);
    }
    
    // 测试不同舍入模式
    printf("\n[Rounding modes test]\n");
    uint32_t mxcsr = get_mxcsr();
    for(int i=0; i<4; i++) {  // 测试4种舍入模式
        uint32_t new_mxcsr = (mxcsr & ~0x6000) | (i << 13);
        set_mxcsr(new_mxcsr);
        
        double rnd_a = 1.5;
        double rnd_b = 1.0;
        double rnd_c = 0.1;
        double rnd_res = 0;
        
        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vmovsd %3, %%xmm2\n\t"
            "vfmadd231sd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovsd %%xmm2, %0\n\t"
            : "=m"(rnd_res)
            : "m"(rnd_a), "m"(rnd_b), "m"(rnd_c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Rounding mode %d:\n", i);
        print_mxcsr(new_mxcsr);
        printf("Result: %.17g\n", rnd_res);
    }
    set_mxcsr(mxcsr);  // 恢复原始MXCSR
}

int main() {
    test_vfmadd231sd();
    return 0;
}
