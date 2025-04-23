#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <immintrin.h>

#define ITERATIONS 1000000000

int main() {
    // 初始化双精度浮点测试数据
    __m128d a = _mm_set_pd(1.1, 2.2);
    __m128d b = _mm_set_pd(0.5, 1.5);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    __asm__ volatile (
        "movdqu %0, %%xmm0\n\t"
        "movdqu %1, %%xmm1\n\t"
        "mov $1000000000, %%ecx\n\t"
        "1:\n\t"
        "dppd $0x33, %%xmm1, %%xmm0\n\t" // 全掩码模式
        "dec %%ecx\n\t"
        "jnz 1b\n\t"
        : 
        : "x" (a), "x" (b)
        : "ecx", "xmm0", "xmm1", "cc"
    );

    clock_gettime(CLOCK_MONOTONIC, &end);

    long long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL 
                         + (end.tv_nsec - start.tv_nsec);
    double elapsed_sec = (double)elapsed_ns / 1e9;

    printf("Executed %d DPPD instructions in %.6f seconds\n", 
           ITERATIONS, elapsed_sec);
    printf("Average latency: %.2f ns per instruction\n", 
           (double)elapsed_ns / ITERATIONS);

    return 0;
}