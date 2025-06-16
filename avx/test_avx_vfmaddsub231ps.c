#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

// vfmaddsub231ps指令测试
void test_vfmaddsub231ps() {
    printf("--- Testing vfmaddsub231ps (Fused Multiply-Alternating Add/Subtract of Packed Single-Precision Floating-Point Values) ---\n");
    
    // 测试数据(32字节对齐)
    float a[8] __attribute__((aligned(32))) = {1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f, 8.5f};
    float b[8] __attribute__((aligned(32))) = {0.5f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f};
    float c[8] __attribute__((aligned(32))) = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float res[8] __attribute__((aligned(32))) = {0};
    
    // 检查内存对齐
    if(((uintptr_t)a & 0x1F) || ((uintptr_t)b & 0x1F) || 
       ((uintptr_t)c & 0x1F) || ((uintptr_t)res & 0x1F)) {
        printf("Memory alignment error in main test!\n");
        return;
    }
    
    // 预期结果 (vfmaddsub231ps实际行为是: res = (a * b) ± c)
    float expected[8] = {
        (a[0] * b[0]) - c[0],  // 0: sub
        (a[1] * b[1]) + c[1],  // 1: add
        (a[2] * b[2]) - c[2],  // 2: sub
        (a[3] * b[3]) + c[3],  // 3: add
        (a[4] * b[4]) - c[4],  // 4: sub
        (a[5] * b[5]) + c[5],  // 5: add
        (a[6] * b[6]) - c[6],  // 6: sub
        (a[7] * b[7]) + c[7]   // 7: add
    };
    
    // 测试指令
    __asm__ __volatile__(
        "vmovaps %1, %%ymm0\n\t"     // 加载a到ymm0
        "vmovaps %2, %%ymm1\n\t"     // 加载b到ymm1
        "vmovaps %3, %%ymm2\n\t"     // 加载c到ymm2
        "vfmaddsub231ps %%ymm1, %%ymm0, %%ymm2\n\t"  // ymm2 = ymm0*ymm2 ± ymm1 (交替)
        "vmovaps %%ymm2, %0\n\t"     // 存回结果
        : "=m"(res)
        : "m"(a), "m"(b), "m"(c)
        : "ymm0", "ymm1", "ymm2"
    );
    
    print_float_vec("Input a", a, 8);
    print_float_vec("Input b", b, 8);
    print_float_vec("Input c", c, 8);
    print_float_vec("Result", res, 8);
    print_float_vec("Expected", expected, 8);
    
    // 验证结果
    for(int i=0; i<8; i++) {
        if(!float_equal(res[i], expected[i], 1e-6f)) {
            printf("Mismatch at index %d: got %f, expected %f\n", i, res[i], expected[i]);
        }
    }
    
    // 测试特殊值
    printf("\n[Special values test]\n");
    float spec_values[][24] = {
        {INFINITY, 1.0f, INFINITY, 1.0f, INFINITY, 1.0f, INFINITY, 1.0f,
         1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        {-INFINITY, 1.0f, -INFINITY, 1.0f, -INFINITY, 1.0f, -INFINITY, 1.0f,
         1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        {NAN, 1.0f, NAN, 1.0f, NAN, 1.0f, NAN, 1.0f,
         1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        {1.0e38f, 2.0f, 1.0e38f, 2.0f, 1.0e38f, 2.0f, 1.0e38f, 2.0f,
         0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}  // 测试溢出
    };
    
    for(size_t i=0; i<sizeof(spec_values)/sizeof(spec_values[0]); i++) {
        float spec_a[8] = {spec_values[i][0], spec_values[i][1], spec_values[i][2], spec_values[i][3],
                          spec_values[i][4], spec_values[i][5], spec_values[i][6], spec_values[i][7]};
        float spec_b[8] = {spec_values[i][8], spec_values[i][9], spec_values[i][10], spec_values[i][11],
                          spec_values[i][12], spec_values[i][13], spec_values[i][14], spec_values[i][15]};
        float spec_c[8] = {spec_values[i][16], spec_values[i][17], spec_values[i][18], spec_values[i][19],
                          spec_values[i][20], spec_values[i][21], spec_values[i][22], spec_values[i][23]};
        float spec_res[8] = {0};
        
        __asm__ __volatile__(
            "vmovaps %1, %%ymm0\n\t"     // 加载a到ymm0
            "vmovaps %2, %%ymm1\n\t"     // 加载b到ymm1
            "vmovaps %3, %%ymm2\n\t"     // 加载c到ymm2
            "vfmaddsub231ps %%ymm1, %%ymm0, %%ymm2\n\t"  // ymm2 = ymm0*ymm2 ± ymm1 (交替)
            "vmovaps %%ymm2, %0\n\t"     // 存回结果
            : "=m"(spec_res)
            : "m"(spec_a), "m"(spec_b), "m"(spec_c)
            : "ymm0", "ymm1", "ymm2"
        );
        
        printf("Test case %zu:\n", i+1);
        print_hex_float_vec("a", spec_a, 8);
        print_hex_float_vec("b", spec_b, 8);
        print_hex_float_vec("c", spec_c, 8);
        print_hex_float_vec("result", spec_res, 8);
    }
    
    // 测试不同舍入模式
    printf("\n[Rounding modes test]\n");
    uint32_t mxcsr = get_mxcsr();
    for(int i=0; i<4; i++) {  // 测试4种舍入模式
        uint32_t new_mxcsr = (mxcsr & ~0x6000) | (i << 13);
        set_mxcsr(new_mxcsr);
        
        float rnd_a[8] __attribute__((aligned(32))) = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f};
        float rnd_b[8] __attribute__((aligned(32))) = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
        float rnd_c[8] __attribute__((aligned(32))) = {1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f};
        float rnd_res[8] __attribute__((aligned(32))) = {0};
        
        // 确保内存对齐
        if(((uintptr_t)rnd_a & 0x1F) || ((uintptr_t)rnd_b & 0x1F) || 
           ((uintptr_t)rnd_c & 0x1F) || ((uintptr_t)rnd_res & 0x1F)) {
            printf("Memory alignment error!\n");
            continue;
        }
        
        __asm__ __volatile__(
            "vmovaps %1, %%ymm0\n\t"  // a -> ymm0
            "vmovaps %2, %%ymm1\n\t"  // b -> ymm1
            "vmovaps %3, %%ymm2\n\t"  // c -> ymm2
            "vfmaddsub231ps %%ymm0, %%ymm1, %%ymm2\n\t"  // ymm2 = ymm1*ymm2 ± ymm0
            "vmovaps %%ymm2, %0\n\t"  // store result
            : "=m"(rnd_res)
            : "m"(rnd_a), "m"(rnd_b), "m"(rnd_c)
            : "ymm0", "ymm1", "ymm2"
        );
        
        printf("Rounding mode %d:\n", i);
        print_mxcsr(new_mxcsr);
        print_float_vec("Result", rnd_res, 8);
    }
    set_mxcsr(mxcsr);  // 恢复原始MXCSR
}

int main() {
    test_vfmaddsub231ps();
    return 0;
}
