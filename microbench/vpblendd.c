#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <x86intrin.h>

// 测试参数
#define NUM_ITERATIONS 10000000   // 循环迭代次数
#define INS_PER_ITER 50           // 每次迭代执行的指令数
#define NUM_TESTS 10              // 重复测试次数

// 精确计时函数
static inline unsigned long long rdtsc() {
    unsigned int lo, hi;
    asm volatile (
        "mfence\n\t"
        "rdtsc\n\t"
        : "=a" (lo), "=d" (hi)
    );
    return ((unsigned long long)hi << 32) | lo;
}

// VPBLENDD 测试函数
double test_vpblendd(uint8_t imm8) {
    // 预热缓存 (确保CPU频率稳定)
    for (int i = 0; i < 1000; i++) {
        asm volatile (
            "vpblendd $8, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $8, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $8, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $8, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $8, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $8, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $8, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $8, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $8, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $8, %%ymm1, %%ymm0, %%ymm2\n\t"
            : 
            : 
            : "ymm2"
        );
    }
    
    double min_cycles = 1e20;
    
    for (int test = 0; test < NUM_TESTS; test++) {
        unsigned long long start = rdtsc();
        
        // 核心测试循环 (完全在汇编中)
        asm volatile (
            "mov $100000000, %%ecx\n\t"   // 设置循环计数器
            
            // 设置源寄存器
            "vpcmpeqd %%ymm0, %%ymm0, %%ymm0\n\t"  // src1 = 全1
            "vpxor %%ymm1, %%ymm1, %%ymm1\n\t"     // src2 = 全0
            
            "1:\n\t"
            // 执行 10 条 VPBLENDD 指令
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
           "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
           "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
           "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
           "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
	    "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"
            "vpblendd $33, %%ymm1, %%ymm0, %%ymm2\n\t"

            "dec %%ecx\n\t"             // 减少计数器
            "jnz 1b\n\t"                // 循环直到零
            
            : 
            : 
            : "ecx", "ymm0", "ymm1", "ymm2", "cc"
        );
        
        unsigned long long end = rdtsc();
        double cycles_per_ins = (double)(end - start) / (NUM_ITERATIONS * INS_PER_ITER);
        
        if (cycles_per_ins < min_cycles) {
            min_cycles = cycles_per_ins;
        }
        
        printf(".");
        fflush(stdout);
    }
    
    return min_cycles;
}

int main() {
    // 测试不同的掩码模式
    uint8_t imm8_patterns[] = {
        0x00,   // 全选 src1
        0xFF,   // 全选 src2
        0xAA,   // 交替选择 (10101010)
        0x55,   // 交替选择 (01010101)
        0x96,   // 随机模式 (10010110)
        0x3C    // 随机模式 (00111100)
    };
    
    const char* pattern_names[] = {
        "All src1 (0x00)",
        "All src2 (0xFF)",
        "Alternate 1 (0xAA)",
        "Alternate 2 (0x55)",
        "Random 1 (0x96)",
        "Random 2 (0x3C)"
    };
    
    printf("VPBLENDD Microbenchmark (x64)\n");
    printf("Iterations: %d, Instructions per iteration: %d\n\n", 
           NUM_ITERATIONS, INS_PER_ITER);
    
    for (int i = 0; i < sizeof(imm8_patterns)/sizeof(imm8_patterns[0]); i++) {
        uint8_t imm8 = imm8_patterns[i];
        printf("Testing %-20s", pattern_names[i]);
        
        double cycles = test_vpblendd(imm8);
        
        printf(" %.4f cycles/instruction\n", cycles);
    }
    
    return 0;
}
