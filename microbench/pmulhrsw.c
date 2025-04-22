#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <immintrin.h>

#define ITERATIONS 1000000000

int main() {
    // 初始化包含测试数据的 XMM 寄存器
    __m128i a = _mm_set1_epi16(0x1234);
    __m128i b = _mm_set1_epi16(0x5678);

    // 将测试数据加载到 XMM 寄存器
    __asm__ volatile (
        "movdqu %0, %%xmm0\n\t"
        "movdqu %1, %%xmm1\n\t"
        : 
        : "x" (a), "x" (b)
        : "xmm0", "xmm1"
    );

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // 执行测试循环的汇编代码
    __asm__ volatile (
        "mov $1000000000, %%ecx\n\t"       // 加载循环计数器
        "1:\n\t"
        "pmulhrsw %%xmm1, %%xmm0\n\t" // 被测指令
        "dec %%ecx\n\t"            // 递减计数器
        "jnz 1b\n\t"               // 循环控制
        : 
        : 
        : "ecx", "xmm0", "xmm1"
    );

    clock_gettime(CLOCK_MONOTONIC, &end);

    // 计算执行时间
    long long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL 
                         + (end.tv_nsec - start.tv_nsec);
    double elapsed_sec = (double)elapsed_ns / 1e9;

    printf("Executed %d PMULHRSW instructions in %.6f seconds\n", 
           ITERATIONS, elapsed_sec);
    printf("Average latency: %.2f ns per instruction\n", 
           (double)elapsed_ns / ITERATIONS);

    return 0;
}
