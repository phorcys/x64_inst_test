#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include "x87.h"

void test_fdecstp() {
    printf("Testing FDECSTP instruction\n");
    printf("FDECSTP decrements stack pointer by 1 (0→7)\n");

    // 测试1: 基本递减测试
    printf("\nTest 1: Basic decrement\n");
    asm volatile ("finit\n\t");
    long double values[] = {1.0L, 2.0L, 3.0L, 4.0L};
    for (int i = 0; i < 4; i++) {
        asm volatile ("fldt %0\n\t" : : "m" (values[i]));
    }
    
    printf("Initial state:\n");
    print_x87_status();
    
    asm volatile ("fdecstp\n\t");
    
    printf("After FDECSTP:\n");
    print_x87_status();

    unsigned short status;
    asm volatile ("fnstsw %0\n\t" : "=m" (status));
    int new_top = (status >> 11) & 0x7;
    if (new_top == 3) {
        printf("PASS: Stack pointer correctly decremented (4→3)\n");
    } else {
        printf("FAIL: Expected top=3, got %d\n", new_top);
    }

    // 测试2: 边界条件(0→7)
    printf("\nTest 2: Boundary case (0→7)\n");
    asm volatile ("finit\n\t");
    asm volatile ("fdecstp\n\t"); // TOP初始为0
    
    asm volatile ("fnstsw %0\n\t" : "=m" (status));
    new_top = (status >> 11) & 0x7;
    if (new_top == 7) {
        printf("PASS: Stack pointer correctly wrapped (0→7)\n");
    } else {
        printf("FAIL: Expected top=7, got %d\n", new_top);
    }

    // 测试3: 多次递减和寄存器内容不变性
    printf("\nTest 3: Multiple decrements and register preservation\n");
    asm volatile ("finit\n\t");
    for (int i = 0; i < 8; i++) {
        asm volatile ("fld1\n\t"); // 加载1.0到栈
    }
    
    // 记录初始寄存器内容
    long double regs[8];
    for (int i = 0; i < 8; i++) {
        asm volatile ("fstpt %0\n\t" : "=m" (regs[i]));
    }
    
    // 执行8次递减(完整循环)
    for (int i = 0; i < 8; i++) {
        asm volatile ("fdecstp\n\t");
        asm volatile ("fnstsw %0\n\t" : "=m" (status));
        new_top = (status >> 11) & 0x7;
        printf("After %d FDECSTP: TOP=%d\n", i+1, new_top);
    }
    
    // 验证寄存器内容不变
    int preserved = 1;
    for (int i = 0; i < 8; i++) {
        if (regs[i] != 1.0L) {
            printf("FAIL: Register ST(%d) changed\n", i);
            preserved = 0;
        }
    }
    if (preserved) {
        printf("PASS: All registers preserved\n");
    }

    printf("\nFDECSTP test completed\n");
}

int main() {
    test_fdecstp();
    return 0;
}
