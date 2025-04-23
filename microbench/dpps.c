#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <immintrin.h>

#define ITERATIONS 1000000000

int main() {
    // 初始化单精度浮点测试数据
    __m128 a = _mm_set_ps(1.1f, 2.2f, 3.3f, 4.4f);
    __m128 b = _mm_set_ps(0.5f, 1.5f, 2.5f, 3.5f);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    __asm__ volatile (
        "movdqu %0, %%xmm0\n\t"
        "movdqu %1, %%xmm1\n\t"
        "mov $1000000000, %%ecx\n\t"
        "1:\n\t"
        "dpps $0xff, %%xmm1, %%xmm0\n\t" // 全掩码模式
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

    printf("Executed %d DPPS instructions in %.6f seconds\n", 
           ITERATIONS, elapsed_sec);
    printf("Average latency: %.2f ns per instruction\n", 
           (double)elapsed_ns / ITERATIONS);

    return 0;
}
