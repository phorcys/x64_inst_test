#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// vfmaddsub231pd指令测试
void test_vfmaddsub231pd() {
    printf("--- Testing vfmaddsub231pd (Fused Multiply-Alternating Add/Subtract of Packed Double-Precision Floating-Point Values) ---\n");
    
    // 测试数据(32字节对齐)
    double a[4] __attribute__((aligned(32))) = {1.5, 2.5, 3.5, 4.5};
    double b[4] __attribute__((aligned(32))) = {0.5, 1.5, 2.5, 3.5};
    double c[4] __attribute__((aligned(32))) = {1.0, 2.0, 3.0, 4.0};
    double res[4] __attribute__((aligned(32))) = {0};
    
    // 检查内存对齐
    if(((uintptr_t)a & 0x1F) || ((uintptr_t)b & 0x1F) || 
       ((uintptr_t)c & 0x1F) || ((uintptr_t)res & 0x1F)) {
        printf("Memory alignment error in main test!\n");
        return;
    }
    
    // 预期结果 (vfmaddsub231pd实际行为是: res = (a * b) ± c)
    double expected[4] = {
        (a[0] * b[0]) - c[0],  // 0: sub
        (a[1] * b[1]) + c[1],  // 1: add
        (a[2] * b[2]) - c[2],  // 2: sub
        (a[3] * b[3]) + c[3]   // 3: add
    };
    
    // 测试指令
    __asm__ __volatile__(
        "vmovapd %1, %%ymm0\n\t"     // 加载a到ymm0
        "vmovapd %2, %%ymm1\n\t"     // 加载b到ymm1
        "vmovapd %3, %%ymm2\n\t"     // 加载c到ymm2
        "vfmaddsub231pd %%ymm1, %%ymm0, %%ymm2\n\t"  // ymm2 = ymm0*ymm2 ± ymm1 (交替)
        "vmovapd %%ymm2, %0\n\t"     // 存回结果
        : "=m"(res)
        : "m"(a), "m"(b), "m"(c)
        : "ymm0", "ymm1", "ymm2"
    );
    
    print_double_vec("Input a", a, 4);
    print_double_vec("Input b", b, 4);
    print_double_vec("Input c", c, 4);
    print_double_vec("Result", res, 4);
    print_double_vec("Expected", expected, 4);
    
    // 验证结果
    for(int i=0; i<4; i++) {
        if(!double_equal(res[i], expected[i], 1e-12)) {
            printf("Mismatch at index %d: got %f, expected %f\n", i, res[i], expected[i]);
        }
    }
    
    // 测试特殊值
    printf("\n[Special values test]\n");
    double spec_values[][12] = {
        {INFINITY, 1.0, INFINITY, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
        {-INFINITY, 1.0, -INFINITY, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
        {NAN, 1.0, NAN, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
        {0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
        {1.0e308, 2.0, 1.0e308, 2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}  // 测试溢出
    };
    
    for(size_t i=0; i<sizeof(spec_values)/sizeof(spec_values[0]); i++) {
        double spec_a[4] = {spec_values[i][0], spec_values[i][1], spec_values[i][2], spec_values[i][3]};
        double spec_b[4] = {spec_values[i][4], spec_values[i][5], spec_values[i][6], spec_values[i][7]};
        double spec_c[4] = {spec_values[i][8], spec_values[i][9], spec_values[i][10], spec_values[i][11]};
        double spec_res[4] = {0};
        
        __asm__ __volatile__(
            "vmovapd %1, %%ymm0\n\t"     // 加载a到ymm0
            "vmovapd %2, %%ymm1\n\t"     // 加载b到ymm1
            "vmovapd %3, %%ymm2\n\t"     // 加载c到ymm2
            "vfmaddsub231pd %%ymm2, %%ymm1, %%ymm0\n\t"  // ymm0 = ymm1*ymm0 ± ymm2 (交替)
            "vmovapd %%ymm0, %0\n\t"     // 存回结果
            : "=m"(spec_res)
            : "m"(spec_a), "m"(spec_b), "m"(spec_c)
            : "ymm0", "ymm1", "ymm2"
        );
        
        printf("Test case %zu:\n", i+1);
        print_double_vec_hex("a", spec_a, 4);
        print_double_vec_hex("b", spec_b, 4);
        print_double_vec_hex("c", spec_c, 4);
        print_double_vec_hex("result", spec_res, 4);
    }
    
    // 测试不同舍入模式
    printf("\n[Rounding modes test]\n");
    uint32_t mxcsr = get_mxcsr();
    for(int i=0; i<4; i++) {  // 测试4种舍入模式
        uint32_t new_mxcsr = (mxcsr & ~0x6000) | (i << 13);
        set_mxcsr(new_mxcsr);
        
        double rnd_a[4] __attribute__((aligned(32))) = {0.1, 0.1, 0.1, 0.1};
        double rnd_b[4] __attribute__((aligned(32))) = {1.0, 1.0, 1.0, 1.0};
        double rnd_c[4] __attribute__((aligned(32))) = {1.5, 1.5, 1.5, 1.5};
        double rnd_res[4] __attribute__((aligned(32))) = {0};
        
        // 确保内存对齐
        if(((uintptr_t)rnd_a & 0x1F) || ((uintptr_t)rnd_b & 0x1F) || 
           ((uintptr_t)rnd_c & 0x1F) || ((uintptr_t)rnd_res & 0x1F)) {
            printf("Memory alignment error!\n");
            continue;
        }
        
        __asm__ __volatile__(
            "vmovapd %1, %%ymm0\n\t"  // a -> ymm0
            "vmovapd %2, %%ymm1\n\t"  // b -> ymm1
            "vmovapd %3, %%ymm2\n\t"  // c -> ymm2
            "vfmaddsub231pd %%ymm0, %%ymm1, %%ymm2\n\t"  // ymm2 = ymm1*ymm2 ± ymm0
            "vmovapd %%ymm2, %0\n\t"  // store result
            : "=m"(rnd_res)
            : "m"(rnd_a), "m"(rnd_b), "m"(rnd_c)
            : "ymm0", "ymm1", "ymm2"
        );
        
        printf("Rounding mode %d:\n", i);
        print_mxcsr(new_mxcsr);
        print_double_vec("Result", rnd_res, 4);
    }
    set_mxcsr(mxcsr);  // 恢复原始MXCSR
}

int main() {
    test_vfmaddsub231pd();
    return 0;
}
