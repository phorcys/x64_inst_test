#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fdecstp() {
    printf("Testing FDECSTP instruction\n");
    printf("FDECSTP decrements stack pointer by 1\n");

    // 初始化x87栈
    asm volatile ("finit\n\t");
    
    // 加载测试值
    long double values[] = {1.0L, 2.0L, 3.0L, 4.0L};
    for (int i = 0; i < 4; i++) {
        asm volatile ("fldt %0\n\t" : : "m" (values[i]));
    }

    // 记录初始状态
    printf("\nInitial state:\n");
    print_x87_status();
    
    // 执行FDECSTP
    asm volatile ("fdecstp\n\t");
    
    // 记录执行后状态
    printf("\nAfter FDECSTP:\n");
    print_x87_status();

    // 验证栈指针变化
    unsigned short status;
    asm volatile ("fnstsw %0\n\t" : "=m" (status));
    int new_top = (status >> 11) & 0x7;
    if (new_top == 3) {  // 初始top=4, 减1后应为3
        printf("PASS: Stack pointer correctly decremented\n");
    } else {
        printf("FAIL: Expected top=3, got %d\n", new_top);
    }

    printf("\nFDECSTP test completed\n");
}

int main() {
    test_fdecstp();
    return 0;
}
