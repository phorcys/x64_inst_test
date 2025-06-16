#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// vfmsub213ss指令测试
void test_vfmsub213ss() {
    printf("--- Testing vfmsub213ss (Fused Multiply-Subtract of Scalar Single-Precision Floating-Point Values) ---\n");
    
    // 测试数据
    float a = 1.5f;
    float b = 0.5f;
    float c = 1.0f;
    float res = 0;
    
    // 预期结果 (vfmsub213ss: res = a*b - c)
    float expected = a * b - c;
    
    // 执行指令
    __asm__ __volatile__(
        "vmovss %1, %%xmm0\n\t"     // 加载a到xmm0 (第一个操作数)
        "vmovss %2, %%xmm1\n\t"     // 加载b到xmm1 (第二个操作数)
        "vmovss %3, %%xmm2\n\t"     // 加载c到xmm2 (第三个操作数)
        "vfmsub213ss %%xmm2, %%xmm1, %%xmm0\n\t"  // xmm0 = xmm0*xmm1 - xmm2
        "vmovss %%xmm0, %0\n\t"     // 存回结果
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
            "vfmsub213ss %%xmm2, %%xmm1, %%xmm0\n\t"
            "vmovss %%xmm0, %0\n\t"
            : "=m"(spec_res[i])
            : "m"(spec_a[i]), "m"(spec_b[i]), "m"(spec_c[i])
            : "xmm0", "xmm1", "xmm2"
        );
    }
    
    print_float_vec("Special a", spec_a, 4);
    print_float_vec("Special b", spec_b, 4);
    print_float_vec("Special c", spec_c, 4);
    print_float_vec("Special result", spec_res, 4);
}

int main() {
    test_vfmsub213ss();
    return 0;
}
