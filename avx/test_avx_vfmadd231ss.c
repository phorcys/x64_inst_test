#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// vfmadd231ss指令测试
void test_vfmadd231ss() {
    printf("--- Testing vfmadd231ss (Fused Multiply-Add of Scalar Single-Precision Floating-Point Values) ---\n");
    
    // 测试数据
    float a = 1.5f;
    float b = 2.5f;
    float c = 0.5f;
    float res = 0;
    
    // 预期结果
    float expected = a * b + c;
    
    // 测试指令
    __asm__ __volatile__(
        "vmovss %1, %%xmm0\n\t"     // 加载a到xmm0
        "vmovss %2, %%xmm1\n\t"     // 加载b到xmm1
        "vmovss %3, %%xmm2\n\t"     // 加载c到xmm2
        "vfmadd231ss %%xmm1, %%xmm0, %%xmm2\n\t"  // xmm2 = xmm0*xmm1 + xmm2
        "vmovss %%xmm2, %0\n\t"     // 存回结果
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
    if(!float_equal(res, expected, 1e-6f)) {
        printf("Mismatch: got %f, expected %f\n", res, expected);
    }
    
    // 测试特殊值
    printf("\n[Special values test]\n");
    float spec_values[][3] = {
        {INFINITY, 1.0f, 1.0f},
        {-INFINITY, 1.0f, 1.0f},
        {NAN, 1.0f, 1.0f},
        {0.0f, 1.0f, 1.0f},
        {1.0e38f, 2.0f, 0.0f}  // 测试溢出
    };
    
    for(size_t i=0; i<sizeof(spec_values)/sizeof(spec_values[0]); i++) {
        float spec_a = spec_values[i][0];
        float spec_b = spec_values[i][1];
        float spec_c = spec_values[i][2];
        float spec_res = 0;
        
        __asm__ __volatile__(
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vmovss %3, %%xmm2\n\t"
            "vfmadd231ss %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovss %%xmm2, %0\n\t"
            : "=m"(spec_res)
            : "m"(spec_a), "m"(spec_b), "m"(spec_c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Test case %zu:\n", i+1);
        print_hex_float_vec("a", &spec_a, 1);
        print_hex_float_vec("b", &spec_b, 1);
        print_hex_float_vec("c", &spec_c, 1);
        print_hex_float_vec("result", &spec_res, 1);
    }
    
    // 测试不同舍入模式
    printf("\n[Rounding modes test]\n");
    uint32_t mxcsr = get_mxcsr();
    for(int i=0; i<4; i++) {  // 测试4种舍入模式
        uint32_t new_mxcsr = (mxcsr & ~0x6000) | (i << 13);
        set_mxcsr(new_mxcsr);
        
        float rnd_a = 1.5f;
        float rnd_b = 1.0f;
        float rnd_c = 0.1f;
        float rnd_res = 0;
        
        __asm__ __volatile__(
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vmovss %3, %%xmm2\n\t"
            "vfmadd231ss %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovss %%xmm2, %0\n\t"
            : "=m"(rnd_res)
            : "m"(rnd_a), "m"(rnd_b), "m"(rnd_c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Rounding mode %d:\n", i);
        print_mxcsr(new_mxcsr);
        printf("Result: %.9g\n", rnd_res);
    }
    set_mxcsr(mxcsr);  // 恢复原始MXCSR
}

int main() {
    test_vfmadd231ss();
    return 0;
}
