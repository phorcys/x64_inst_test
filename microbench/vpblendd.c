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

#define TEST(imm) do{\
    min_cycles = 1e20; \
    for (int test = 0; test < NUM_TESTS; test++) { \
        start = rdtsc(); \
        asm volatile ( \
            "mov %1, %%ecx\n\t" \
            "vpcmpeqd %%ymm0, %%ymm0, %%ymm0\n\t" \
            "vpxor %%ymm1, %%ymm1, %%ymm1\n\t"  \
            "1:\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
           "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t"  \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
           "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
           "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
           "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
	    "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "vblendps %0, %%ymm1, %%ymm0, %%ymm2\n\t" \
            "dec %%ecx\n\t"       \
            "jnz 1b\n\t"           \
            : :"i"(imm), "i"(NUM_ITERATIONS) : "ecx", "ymm0", "ymm1", "ymm2", "cc"); \
        end = rdtsc(); \
        cycles_per_ins = (double)(end - start) / (NUM_ITERATIONS * INS_PER_ITER); \
        if (cycles_per_ins < min_cycles) {  min_cycles = cycles_per_ins; } \
         cycles=cycles_per_ins;\
    } \
}while(0)

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
    
        unsigned long long start,end;
	double min_cycles, cycles_per_ins, cycles;

	printf("Testing %-20s", pattern_names[0]);
        TEST(0x00);
        printf(" %.4f cycles/instruction\n", cycles);

	printf("Testing %-20s", pattern_names[1]);
        TEST(0xFF);
        printf(" %.4f cycles/instruction\n", cycles);
 	printf("Testing %-20s", pattern_names[2]);
        TEST(0xAA);
        printf(" %.4f cycles/instruction\n", cycles);
 	printf("Testing %-20s", pattern_names[3]);
        TEST(0x55);
        printf(" %.4f cycles/instruction\n", cycles);
 	printf("Testing %-20s", pattern_names[4]);
        TEST(0x96);
        printf(" %.4f cycles/instruction\n", cycles);
 	printf("Testing %-20s", pattern_names[5]);
        TEST(0x3c);
        printf(" %.4f cycles/instruction\n", cycles);
 
    
    return 0;
}
