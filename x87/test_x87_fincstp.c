#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fincstp() {
    printf("Testing FINCSTP instruction\n");
    printf("FINCSTP increments stack pointer by 1\n");

    // 初始化x87栈
    asm volatile ("finit\n\t");
    
    // 加载测试值
    long double values[] = {1.0L, 2.0L, 3.0L};
    for (int i = 0; i < 3; i++) {
        asm volatile ("fldt %0\n\t" : : "m" (values[i]));
    }

    // 记录初始状态
    printf("\nInitial state:\n");
    print_x87_status();
    
    // 执行FINCSTP
    asm volatile ("fincstp\n\t");
    
    // 记录执行后状态
    printf("\nAfter FINCSTP:\n");
    print_x87_status();

    // 验证栈指针变化
    unsigned short status;
    asm volatile ("fnstsw %0\n\t" : "=m" (status));
    int new_top = (status >> 11) & 0x7;
    if (new_top == 6) {  // 初始top=5, 加1后应为6
        printf("PASS: Stack pointer correctly incremented\n");
    } else {
        printf("FAIL: Expected top=6, got %d\n", new_top);
    }

    printf("\nFINCSTP test completed\n");
}

int main() {
    test_fincstp();
    return 0;
}
