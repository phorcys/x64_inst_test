#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <mmintrin.h>

#define ITERATIONS 1000000000

int main() {
    // 初始化MMX测试数据（低8位有效）
    __m64 a = _mm_set_pi8(0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08);
    __m64 b = _mm_set_pi8(0xF0, 0xE1, 0xD2, 0xC3, 0xB4, 0xA5, 0x96, 0x87);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // MMX测试循环
    __asm__ volatile (
        "mov $1000000000, %%ecx\n\t"        // 加载循环计数器
        "1:\n\t"
        "pmaddubsw %2, %1\n\t"     // MMX版本指令
        "dec %%ecx\n\t"
        "jnz 1b\n\t"
        : "+y"(a)                  // 使用MMX约束符
        : "y"(a), "y"(b)
        : "ecx", "mm0", "mm1"
    );

    clock_gettime(CLOCK_MONOTONIC, &end);
    _mm_empty();  // 清除MMX状态

    // 计算执行时间
    long long elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    printf("MMX PMADDUBSW: %d iterations in %.2fns/inst\n",
           ITERATIONS, (double)elapsed_ns/ITERATIONS);

    return 0;
}