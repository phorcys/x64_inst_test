#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// vfmadd213ss指令测试
void test_vfmadd213ss() {
    printf("--- Testing vfmadd213ss (Fused Multiply-Add of Scalar Single-Precision Floating-Point Values) ---\n");
    
    // 测试数据
    float a = 1.5f;
    float b = 0.5f;
    float c = 1.0f;
    float res = 0;
    
    // 预期结果 (vfmadd213ss: res = c*a + b)
    float expected = c * a + b;
    
    // 执行指令
    __asm__ __volatile__(
        "vmovss %1, %%xmm0\n\t"     // 加载a到xmm0
        "vmovss %2, %%xmm1\n\t"     // 加载b到xmm1
        "vmovss %3, %%xmm2\n\t"     // 加载c到xmm2
        "vfmadd213ss %%xmm1, %%xmm0, %%xmm2\n\t"  // xmm2 = xmm0*xmm1 + xmm2
        "vmovss %%xmm2, %0\n\t"     // 存回结果
        : "=m"(res)
        : "m"(a), "m"(b), "m"(c)
        : "xmm0", "xmm1", "xmm2"
    );
    
    printf("[Basic test]\n");
    printf("Input a: %f\n", a);
    printf("Input b: %f\n", b);
    printf("Input c: %f\n", c);
    printf("Result: %f\n", res);
    printf("Expected: %f\n", expected);
    
    // 验证结果
    if(!float_equal(res, expected, 1e-5f)) {
        printf("Mismatch: got %f, expected %f\n", res, expected);
    }
    
    // 测试特殊值
    printf("\n[Special values test]\n");
    float spec_a[4] = {INFINITY, -INFINITY, NAN, 0.0f};
    float spec_b[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float spec_c[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float spec_res[4] = {0};
    
    for(int i=0; i<4; i++) {
        __asm__ __volatile__(
            "vmovss %1, %%xmm0\n\t"
            "vmovss %2, %%xmm1\n\t"
            "vmovss %3, %%xmm2\n\t"
            "vfmadd213ss %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovss %%xmm2, %0\n\t"
            : "=m"(spec_res[i])
            : "m"(spec_a[i]), "m"(spec_b[i]), "m"(spec_c[i])
            : "xmm0", "xmm1", "xmm2"
        );
    }
    
    print_hex_float_vec("Special a", spec_a, 4);
    print_hex_float_vec("Special b", spec_b, 4);
    print_hex_float_vec("Special c", spec_c, 4);
    print_hex_float_vec("Special result", spec_res, 4);
}

int main() {
    test_vfmadd213ss();
    return 0;
}
