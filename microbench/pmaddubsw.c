#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <tmmintrin.h>  // SSSE3专用头文件

#define ITERATIONS 1000000000

int main() {
    // 初始化测试数据（无符号字节 vs 有符号字节）
    __m128i a = _mm_set_epi8(0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                             0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10);
    __m128i b = _mm_set_epi8(0xF0, 0xE1, 0xD2, 0xC3, 0xB4, 0xA5, 0x96, 0x87,
                             0x78, 0x69, 0x5A, 0x4B, 0x3C, 0x2D, 0x1E, 0x0F);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // SSE测试循环
    __asm__ volatile (
        "mov $1000000000, %%ecx\n\t"        // 加载循环计数器
        "1:\n\t"
        "pmaddubsw %2, %1\n\t"     // SSE版本指令
        "dec %%ecx\n\t"
        "jnz 1b\n\t"
        : "+x"(a)                  // 输入输出寄存器
        : "x"(a), "x"(b)
        : "ecx", "xmm0", "xmm1"
    );

    clock_gettime(CLOCK_MONOTONIC, &end);

    // 计算执行时间
    long long elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    printf("SSE PMADDUBSW: %d iterations in %.2fns/inst\n", 
           ITERATIONS, (double)elapsed_ns/ITERATIONS);

    return 0;
}