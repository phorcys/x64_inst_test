#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// vfnmsub231sd指令测试
void test_vfnmsub231sd() {
    printf("--- Testing vfnmsub231sd (Fused Negative Multiply-Subtract of Scalar Double-Precision Floating-Point Values) ---\n");
    
    // 测试数据
    double a = 1.5;
    double b = 0.5;
    double c = 1.0;
    double res = 0;
    
    // 预期结果 (vfnmsub231sd: res = -(a*b) - c)
    double expected = -(a * b) - c;
    
    // 执行指令
    __asm__ __volatile__(
        "vmovsd %1, %%xmm0\n\t"     // 加载a到xmm0
        "vmovsd %2, %%xmm1\n\t"     // 加载b到xmm1
        "vmovsd %3, %%xmm2\n\t"     // 加载c到xmm2
        "vfnmsub231sd %%xmm1, %%xmm0, %%xmm2\n\t"  // xmm2 = -(xmm0*xmm1) - xmm2
        "vmovsd %%xmm2, %0\n\t"     // 存回结果
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
    if(!double_equal(res, expected, 1e-10)) {
        printf("Mismatch: got %f, expected %f\n", res, expected);
    }
    
    // 测试特殊值
    printf("\n[Special values test]\n");
    double spec_a[4] = {INFINITY, -INFINITY, NAN, 0.0};
    double spec_b[4] = {1.0, 1.0, 1.0, 1.0};
    double spec_c[4] = {1.0, 1.0, 1.0, 1.0};
    double spec_res[4] = {0};
    
    for(int i=0; i<4; i++) {
        __asm__ __volatile__(
            "vmovsd %1, %%xmm0\n\t"
            "vmovsd %2, %%xmm1\n\t"
            "vmovsd %3, %%xmm2\n\t"
            "vfnmsub231sd %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovsd %%xmm2, %0\n\t"
            : "=m"(spec_res[i])
            : "m"(spec_a[i]), "m"(spec_b[i]), "m"(spec_c[i])
            : "xmm0", "xmm1", "xmm2"
        );
    }
    
    print_double_vec_hex("Special a", spec_a, 4);
    print_double_vec_hex("Special b", spec_b, 4);
    print_double_vec_hex("Special c", spec_c, 4);
    print_double_vec_hex("Special result", spec_res, 4);
}

int main() {
    test_vfnmsub231sd();
    return 0;
}
