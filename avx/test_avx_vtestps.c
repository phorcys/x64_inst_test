#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试vtestps指令
int test_vtestps() {
    int ret = 0;
    printf("===== Testing vtestps =====\n");

    // 测试数据
    ALIGNED(32) float src1[8] = {
        1.0f, -2.0f, 3.0f, -4.0f,
        5.0f, -6.0f, 7.0f, -8.0f
    };
    ALIGNED(32) float src2[8] = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f
    };
    ALIGNED(32) float src3[8] = {
        -1.0f, -2.0f, -3.0f, -4.0f,
        -5.0f, -6.0f, -7.0f, -8.0f
    };

    __m256 ymm1, ymm2, ymm3;
    uint32_t eflags;

    // 加载测试数据
    ymm1 = _mm256_loadu_ps(src1);
    ymm2 = _mm256_loadu_ps(src2);
    ymm3 = _mm256_loadu_ps(src3);

    // 测试混合符号情况
    printf("\n=== Test case: Mixed signs ===\n");
    __asm__ __volatile__(
        "vtestps %1, %1\n\t"
        "pushfq\n\t"
        "pop %q0\n\t"
        : "=r"(eflags)
        : "x"(ymm1)
        : "cc"
    );
        print_eflags_cond(eflags, 0x41);
    if(!(eflags & (1 << 6))) {  // 检查ZF=0
        printf("ZF check passed\n");
    } else {
        printf("ZF check failed\n");
        ret = 1;
    }
    if(eflags & (1 << 0)) {  // 修正：检查CF=1（混合符号时CF应为1）
        printf("CF check passed\n");
    } else {
        printf("CF check failed\n");
        ret = 1;
    }

    // 测试全正数情况
    printf("\n=== Test case: All positive ===\n");
    __asm__ __volatile__(
        "vtestps %1, %1\n\t"
        "pushfq\n\t"
        "pop %q0\n\t"
        : "=r"(eflags)
        : "x"(ymm2)
        : "cc"
    );
        print_eflags_cond(eflags, 0x41);
    if(eflags & (1 << 6)) {  // 检查ZF=1
        printf("ZF check passed\n");
    } else {
        printf("ZF check failed\n");
        ret = 1;
    }
    if(eflags & (1 << 0)) {  // 修正：检查CF=1（全正数时CF应为1）
        printf("CF check passed\n");
    } else {
        printf("CF check failed\n");
        ret = 1;
    }

    // 测试全负数情况
    printf("\n=== Test case: All negative ===\n");
    __asm__ __volatile__(
        "vtestps %1, %1\n\t"
        "pushfq\n\t"
        "pop %q0\n\t"
        : "=r"(eflags)
        : "x"(ymm3)
        : "cc"
    );
        print_eflags_cond(eflags, 0x41);
    if(!(eflags & (1 << 6))) {  // 检查ZF=0
        printf("ZF check passed\n");
    } else {
        printf("ZF check failed\n");
        ret = 1;
    }
    if(eflags & (1 << 0)) {  // 检查CF=1
        printf("CF check passed\n");
    } else {
        printf("CF check failed\n");
        ret = 1;
    }

    return ret;
}

int main() {
    return test_vtestps();
}
