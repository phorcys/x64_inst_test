#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// vfmsub231ss指令测试
void test_vfmsub231ss() {
    printf("--- Testing vfmsub231ss (Fused Multiply-Subtract of Scalar Single-Precision Floating-Point Values) ---\n");
    
    // 测试数据
    float a = 1.5f;
    float b = 2.5f;
    float c = 0.5f;
    float res = 0;
    
    // 预期结果 (vfmsub231ss: res = a*b - c)
    float expected = a * b - c;
    
    // 测试指令
    __asm__ __volatile__(
        "vmovss %1, %%xmm0\n\t"     // 加载a到xmm0
        "vmovss %2, %%xmm1\n\t"     // 加载b到xmm1
        "vmovss %3, %%xmm2\n\t"     // 加载c到xmm2
        "vfmsub231ss %%xmm1, %%xmm0, %%xmm2\n\t"  // xmm2 = xmm0*xmm1 - xmm2
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
    float spec_a = INFINITY;
    float spec_b = 1.0f;
    float spec_c = 1.0f;
    float spec_res = 0;
    
    __asm__ __volatile__(
        "vmovss %1, %%xmm0\n\t"
        "vmovss %2, %%xmm1\n\t"
        "vmovss %3, %%xmm2\n\t"
        "vfmsub231ss %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovss %%xmm2, %0\n\t"
        : "=m"(spec_res)
        : "m"(spec_a), "m"(spec_b), "m"(spec_c)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Special a: 0x%08x\n", *(uint32_t*)&spec_a);
    printf("Special b: 0x%08x\n", *(uint32_t*)&spec_b);
    printf("Special c: 0x%08x\n", *(uint32_t*)&spec_c);
    printf("Special result: 0x%08x\n", *(uint32_t*)&spec_res);
    
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
            "vfmsub231ss %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovss %%xmm2, %0\n\t"
            : "=m"(rnd_res)
            : "m"(rnd_a), "m"(rnd_b), "m"(rnd_c)
            : "xmm0", "xmm1", "xmm2"
        );
        
        printf("Rounding mode %d:\n", i);
        print_mxcsr(new_mxcsr);
        printf("Result: %f\n", rnd_res);
    }
    set_mxcsr(mxcsr);  // 恢复原始MXCSR
}

int main() {
    test_vfmsub231ss();
    return 0;
}
