#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <mmintrin.h>

#define ITERATIONS 1000000000

int main() {
    // 初始化MMX测试数据
    __m64 a = _mm_set1_pi16(0x1234);
    __m64 b = _mm_set1_pi16(0x5678);

    // 加载数据到MMX寄存器
    __asm__ volatile (
        "movq %0, %%mm0\n\t"
        "movq %1, %%mm1\n\t"
        : 
        : "y" (a), "y" (b)
        : "mm0", "mm1"
    );

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // MMX指令测试循环
    __asm__ volatile (
        "mov $1000000000, %%ecx\n\t"       // 加载循环计数器
        "1:\n\t"
        "pmulhrsw %%mm1, %%mm0\n\t" // MMX版本指令
        "dec %%ecx\n\t"
        "jnz 1b\n\t"
        : 
        :
        : "ecx", "mm0", "mm1"
    );

    clock_gettime(CLOCK_MONOTONIC, &end);

    // 清理MMX状态
    _mm_empty();

    // 计算执行时间
    long long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL 
                         + (end.tv_nsec - start.tv_nsec);
    double elapsed_sec = (double)elapsed_ns / 1e9;

    printf("Executed %d MMX PMULHRSW instructions in %.6f seconds\n",
           ITERATIONS, elapsed_sec);
    printf("Average latency: %.2f ns per instruction\n",
           (double)elapsed_ns / ITERATIONS);

    return 0;
}