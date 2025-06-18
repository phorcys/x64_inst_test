#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 打印256位浮点向量
static void print_m256d(const char* name, __m256d vec) {
    double *vals = (double*)&vec;
    printf("%s: [%.6f, %.6f, %.6f, %.6f]\n", name, vals[0], vals[1], vals[2], vals[3]);
}

int main() {
    int errors = 0;
    
    printf("=== Testing vmaskmovps (conditional move based on mask) ===\n");
    
    // 测试256位寄存器操作
    printf("\n[Test 1: 256-bit mask operation]\n");
    {
        ALIGNED(32) double src[4] = {10.0, 20.0, 30.0, 40.0};
        ALIGNED(32) double mask[4] = {-1.0, 0.0, -1.0, 0.0}; // -1.0表示设置所有位
        ALIGNED(32) double dest[4] = {0.0, 0.0, 0.0, 0.0};
        ALIGNED(32) double expected[4] = {10.0, 0.0, 30.0, 0.0};
        
        __m256d src_vec = _mm256_load_pd(src);
        __m256d mask_vec = _mm256_load_pd(mask);
        
        CLEAR_FLAGS;
        
        // 内联汇编实现vmaskmovps
        asm volatile (
            "vmaskmovpd %1, %2, %0"
            : "=m"(*dest)
            : "v"(src_vec), "v"(mask_vec)
            : "memory"
        );
        
        // 打印输入和结果
        print_m256d("Source", src_vec);
        print_m256d("Mask", mask_vec);
        print_m256d("Result", _mm256_load_pd(dest));
        print_m256d("Expected", _mm256_load_pd(expected));
        
        // 检查结果
        for (int i = 0; i < 4; i++) {
            if (dest[i] != expected[i]) {
                printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                       i, expected[i], dest[i]);
                errors++;
            }
        }
        
        uint64_t eflags = 0;
        asm volatile ("pushfq; pop %0" : "=r"(eflags));
        print_eflags(eflags);
    }
    
    printf("\n[Test 2: 256-bit mask with boundary values]\n");
    {
        ALIGNED(32) double src[4] = {INFINITY, -INFINITY, NAN, 0.0};
        ALIGNED(32) double mask[4] = {0.0, -1.0, 0.0, -1.0};
        ALIGNED(32) double dest[4] = {50.0, 60.0, 70.0, 80.0};
        ALIGNED(32) double expected[4] = {50.0, -INFINITY, 70.0, 0.0};
        
        __m256d src_vec = _mm256_load_pd(src);
        __m256d mask_vec = _mm256_load_pd(mask);
        
        CLEAR_FLAGS;
        
        asm volatile (
            "vmaskmovpd %1, %2, %0"
            : "=m"(*dest)
            : "v"(src_vec), "v"(mask_vec)
            : "memory"
        );
        
        // 打印输入和结果
        print_m256d("Source", src_vec);
        print_m256d("Mask", mask_vec);
        print_m256d("Result", _mm256_load_pd(dest));
        print_m256d("Expected", _mm256_load_pd(expected));
        
        // 检查结果
        for (int i = 0; i < 4; i++) {
            if (i == 1 || i == 3) {
                // 检查特殊值位置
                if (i == 1 && dest[i] != -INFINITY) {
                    printf("Mismatch at position %d: expected -INF, got %.6f\n", i, dest[i]);
                    errors++;
                }
                if (i == 3 && dest[i] != 0.0) {
                    printf("Mismatch at position %d: expected 0.0, got %.6f\n", i, dest[i]);
                    errors++;
                }
            } else {
                // 检查常规值位置
                if (dest[i] != expected[i]) {
                    printf("Mismatch at position %d: expected %.6f, got %.6f\n", 
                           i, expected[i], dest[i]);
                    errors++;
                }
            }
        }
        
        uint64_t eflags = 0;
        asm volatile ("pushfq; pop %0" : "=r"(eflags));
        print_eflags(eflags);
    }
    
    printf("\n=== Tests complete: %d errors ===\n", errors);
    return errors ? 1 : 0;
}
