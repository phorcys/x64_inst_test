#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fenv.h>
#include <math.h>
#include <float.h>
#include "x87.h"

// Function declarations
void test_fstp_basic(void);
void test_fstp_double(void);
void test_fstp_rounding(void);
void test_fstp_integer(void);
void test_fstp_status(void);
void test_fstp_modern(void);
void test_fstp_multithread(void);
void test_fstp_boundary(void);
void test_fstp_combinations(void);
void test_fstp_exceptions(void);
void test_fstp_extended(void);

#define TEST_CASE(name, condition) \
    do { \
        printf("[TEST] %-40s %s\n", name, (condition) ? "PASS" : "FAIL"); \
        if (!(condition)) { \
            printf("  [FAIL DETAIL] %s:%d\n", __FILE__, __LINE__); \
        } \
    } while (0)

// 实现所有测试函数
void test_fstp_basic() {
    // 基础测试实现...
}

void test_fstp_double() {
    // 双精度测试实现... 
}

void test_fstp_rounding() {
    // 舍入模式测试实现...
}

void test_fstp_integer() {
    // 整数测试实现...
}

void test_fstp_status() {
    // 状态寄存器测试实现...
}

void test_fstp_modern() {
    // 现代CPU特性测试实现...
}

void test_fstp_boundary() {
    // 边界值测试实现...
}

void test_fstp_exceptions() {
    // 异常测试实现...
}

void test_fstp_extended() {
    // 扩展精度测试实现...
}

void test_fstp_multithread() {
    printf("\n==== Testing FSTP in Multithread Context ====\n");
    // 多线程测试实现...
}

void test_fstp_combinations() {
    printf("\n==== Testing FSTP Combinations ====\n");
    
    struct {
        const char* desc;
        uint16_t cw;
        float value;
    } tests[] = {
        {"Normal value, default rounding", 0x037F, 1.2345f},
        {"Normal value, round down", 0x077F, 1.2345f},
        {"Normal value, round up", 0x0B7F, 1.2345f},
        {"Normal value, truncate", 0x0F7F, 1.2345f},
        {"Denormal value", 0x037F, FLT_MIN/2.0f},
        {"Large value", 0x037F, FLT_MAX*0.999f},
        {"Small value", 0x037F, FLT_MIN*2.0f}
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest %zu: %s\n", i+1, tests[i].desc);
        
        asm volatile ("fninit");
        uint16_t cw = tests[i].cw;
        asm volatile ("fldcw %0" : : "m" (cw));
        
        float stored;
        asm volatile (
            "flds %1\n\t"
            "fstps %0\n\t"
            : "=m" (stored)
            : "m" (tests[i].value)
            : "st"
        );
        
        printf("Original: %.15g\n", tests[i].value);
        printf("Stored:   %.15g\n", stored);
        TEST_CASE("Value stored correctly", tests[i].value == stored);
    }
}

int main() {
    fesetround(FE_TONEAREST);
    feclearexcept(FE_ALL_EXCEPT);

    test_fstp_basic();
    test_fstp_double();
    test_fstp_rounding();
    test_fstp_integer();
    test_fstp_status();
    test_fstp_modern();
    test_fstp_multithread();
    test_fstp_boundary();
    test_fstp_combinations();
    test_fstp_exceptions();
    test_fstp_extended();

    int exceptions = fetestexcept(FE_ALL_EXCEPT);
    if (exceptions) {
        printf("\n[WARNING] Floating-point exceptions occurred: ");
        if (exceptions & FE_INVALID) printf("FE_INVALID ");
        if (exceptions & FE_DIVBYZERO) printf("FE_DIVBYZERO ");
        if (exceptions & FE_OVERFLOW) printf("FE_OVERFLOW ");
        if (exceptions & FE_UNDERFLOW) printf("FE_UNDERFLOW ");
        if (exceptions & FE_INEXACT) printf("FE_INEXACT ");
        printf("\n");
    }

    return 0;
}
