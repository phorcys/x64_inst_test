#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// vfmsub231sd指令测试
void test_vfmsub231sd() {
    printf("--- Testing vfmsub231sd (Fused Multiply-Subtract of Scalar Double-Precision Floating-Point Values) ---\n");
    
    // 测试数据
    double a = 1.5;
    double b = 2.5;
    double c = 0.5;
    double res = 0;
    
    // 预期结果 (vfmsub231sd: res = a*b - c)
    double expected = a * b - c;
    
    // 测试指令
    __asm__ __volatile__(
        "vmovsd %1, %%xmm0\n\t"     // 加载a到xmm0
        "vmovsd %2, %%xmm1\n\t"     // 加载b到xmm1
        "vmovsd %3, %%xmm2\n\t"     // 加载c到xmm2
        "vfmsub231sd %%xmm1, %%xmm0, %%xmm2\n\t"  // xmm2 = xmm0*xmm1 - xmm2
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
    if(!double_equal(res, expected, 1e-10)) {
        printf("Mismatch: got %f, expected %f\n", res, expected);
    }
    
    // 测试特殊值
    printf("\n[Special values test]\n");
    double spec_a = INFINITY;
    double spec_b = 1.0;
    double spec_c = 1.0;
    double spec_res = 0;
    
    __asm__ __volatile__(
        "vmovsd %1, %%xmm0\n\t"
        "vmovsd %2, %%xmm1\n\t"
        "vmovsd %3, %%xmm2\n\t"
        "vfmsub231sd %%xmm1, %%xmm0, %%xmm2\n\t"
        "vmovsd %%xmm2, %0\n\t"
        : "=m"(spec_res)
        : "m"(spec_a), "m"(spec_b), "m"(spec_c)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("Special a: 0x%016lx\n", *(uint64_t*)&spec_a);
    printf("Special b: 0x%016lx\n", *(uint64_t*)&spec_b);
    printf("Special c: 0x%016lx\n", *(uint64_t*)&spec_c);
    printf("Special result: 0x%016lx\n", *(uint64_t*)&spec_res);
    
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
            "vfmsub231sd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovsd %%xmm2, %0\n\t"
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
    test_vfmsub231sd();
    return 0;
}
