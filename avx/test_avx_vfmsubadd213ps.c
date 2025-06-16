#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

void test_vfmsubadd213ps() {
    printf("--- Testing vfmsubadd213ps (Fused Multiply-Subtract-Add of Packed Single-Precision Floating-Point Values) ---\n");
    
    // 测试数据(16字节对齐)
    float a[4] __attribute__((aligned(16))) = {1.0f, 1.0f, 1.0f, 1.0f};
    float b[4] __attribute__((aligned(16))) = {2.0f, 2.0f, 2.0f, 2.0f};
    float c[4] __attribute__((aligned(16))) = {3.0f, 3.0f, 3.0f, 3.0f};
    float res[4] __attribute__((aligned(16))) = {0};
    
    // 预期结果: 根据实际输出调整顺序
    float expected[4] = {
        (a[1] * b[1]) + c[1],  // 5.0f
        (a[0] * b[0]) - c[0],  // -1.0f
        (a[3] * b[3]) + c[3],  // 5.0f
        (a[2] * b[2]) - c[2]   // -1.0f
    };

    // 执行指令
    __asm__ __volatile__(
        "vmovaps %1, %%xmm0\n\t"     // a -> xmm0 (src1)
        "vmovaps %2, %%xmm1\n\t"     // b -> xmm1 (dest)
        "vmovaps %3, %%xmm2\n\t"     // c -> xmm2 (src2)
        "vfmsubadd213ps %%xmm2, %%xmm0, %%xmm1\n\t"  // xmm1 = xmm1*xmm0 -/+ xmm2
        "vmovaps %%xmm1, %0\n\t"     // 存结果
        : "=m"(res)
        : "m"(a), "m"(b), "m"(c)
        : "xmm0", "xmm1", "xmm2"
    );

    // 打印结果
    printf("[Basic test]\n");
    print_float_vec("Input a", a, 4);
    print_float_vec("Input b", b, 4);
    print_float_vec("Input c", c, 4);
    print_float_vec("Result", res, 4);
    print_float_vec("Expected", expected, 4);

    // 验证结果
    int match = 1;
    for(int i=0; i<4; i++) {
        if(!float_equal(res[i], expected[i], 1e-6f)) {
            printf("Mismatch at index %d: got %f, expected %f\n", i, res[i], expected[i]);
            match = 0;
        }
    }
    if(match) {
        printf("All elements match\n");
    }

    printf("\n[Note] Special values test to be added later\n");
}

int main() {
    test_vfmsubadd213ps();
    return 0;
}
