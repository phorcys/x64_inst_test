#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// 测试vtestpd指令
int test_vtestpd() {
    int ret = 0;
    printf("===== Testing vtestpd =====\n");

    // 测试数据
    ALIGNED(32) double src1[4] = {
        1.0, -2.0, 3.0, -4.0
    };
    ALIGNED(32) double src2[4] = {
        1.0, 2.0, 3.0, 4.0
    };
    ALIGNED(32) double src3[4] = {
        -1.0, -2.0, -3.0, -4.0
    };
    ALIGNED(32) double src4[4] = {
        NAN, INFINITY, -INFINITY, 0.0
    };

    __m256d ymm1, ymm2, ymm3, ymm4;
    uint32_t eflags;

    // 加载测试数据
    ymm1 = _mm256_loadu_pd(src1);
    ymm2 = _mm256_loadu_pd(src2);
    ymm3 = _mm256_loadu_pd(src3);
    ymm4 = _mm256_loadu_pd(src4);

    // 测试混合符号情况
    printf("\n=== Test case 1: Mixed signs ===\n");
    print_double_vec("Input", src1, 4);
    __asm__ __volatile__(
        "push $0\n\t"  // 清除EFLAGS
        "popfq\n\t"
        "vtestpd %1, %1\n\t"
        "pushfq\n\t"
        "pop %q0\n\t"
        : "=r"(eflags)
        : "x"(ymm1)
        : "cc"
    );
    print_eflags(eflags);
    if(!(eflags & (1 << 6))) {  // 检查ZF=0
        printf("ZF check passed\n");
    } else {
        printf("ZF check failed\n");
        ret = 1;
    }
    if(!(eflags & (1 << 0))) {  // 检查CF=0
        printf("CF check passed\n");
    } else {
        printf("CF check failed\n");
        ret = 1;
    }
    // 测试全正数情况
    printf("\n=== Test case 2: All positive ===\n");
    print_double_vec("Input", src2, 4);
    __asm__ __volatile__(
        "push $0\n\t"  // 清除EFLAGS
        "popfq\n\t"
        "vtestpd %1, %1\n\t"
        "pushfq\n\t"
        "pop %q0\n\t"
        : "=r"(eflags)
        : "x"(ymm2)
        : "cc"
    );
    print_eflags(eflags);

    // 测试全负数情况
    printf("\n=== Test case 3: All negative ===\n");
    print_double_vec("Input", src3, 4);
    __asm__ __volatile__(
        "push $0\n\t"  // 清除EFLAGS
        "popfq\n\t"
        "vtestpd %1, %1\n\t"
        "pushfq\n\t"
        "pop %q0\n\t"
        : "=r"(eflags)
        : "x"(ymm3)
        : "cc"
    );
    print_eflags(eflags);
    if(!(eflags & (1 << 6))) {  // 检查ZF=0
        printf("ZF check passed\n");
    } else {
        printf("ZF check failed\n");
        ret = 1;
    }
    if(!(eflags & (1 << 0))) {  // 检查CF=0
        printf("CF check passed\n");
    } else {
        printf("CF check failed\n");
        ret = 1;
    }

    // 测试特殊值(NaN, INF)
    printf("\n=== Test case 4: Special values ===\n");
    print_double_vec("Input", src4, 4);
    __asm__ __volatile__(
        "push $0\n\t"  // 清除EFLAGS
        "popfq\n\t"
        "vtestpd %1, %1\n\t"
        "pushfq\n\t"
        "pop %q0\n\t"
        : "=r"(eflags)
        : "x"(ymm4)
        : "cc"
    );
    print_eflags(eflags);
    if(!(eflags & (1 << 6))) {  // 检查ZF=0
        printf("ZF check passed\n");
    } else {
        printf("ZF check failed\n");
        ret = 1;
    }
    if(!(eflags & (1 << 0))) {  // 检查CF=0
        printf("CF check passed\n");
    } else {
        printf("CF check failed\n");
        ret = 1;
    }    
    return ret;
}

int main() {
    return test_vtestpd();
}
