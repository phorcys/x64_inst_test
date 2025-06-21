#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

// 测试验证宏
#define VERIFY(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s\n", message); \
            fails++; \
        } else { \
            printf("PASS: %s\n", message); \
        } \
    } while (0)

static int test_vpcmpestri() {
    printf("=== Testing VPCMPESTRI ===\n");
    int fails = 0;
    
    // 测试数据
    uint8_t str1[32] = "HelloWorld12345HelloWorld12345"; // 32字节测试数据
    uint8_t str2[32] = "HelloBox6412345HelloBox6412345";
    uint8_t str4[32] = {0}; // 空字符串
    
    __m128i xmm1, xmm2;
    int32_t eax, edx;
    uint8_t imm8;
    int32_t ecx;
    uint64_t eflags;  // 改为64位以匹配pushfq/pop
    
    // 测试1: 相等比较(字节模式) - 128位
    printf("\nTest 1: Equal comparison (byte mode, 128-bit)\n");
    xmm1 = _mm_loadu_si128((__m128i*)str1);
    xmm2 = _mm_loadu_si128((__m128i*)str1);
    eax = edx = 15; // 比较15个字节
    imm8 = 0x00;    // 相等比较, 返回第一个匹配索引
    
    CLEAR_FLAGS;
    // 使用宏展开立即数
    #define _ASM_VPCMPESTRI(imm) \
        asm volatile ( \
            "vpcmpestri $" #imm ", %3, %2\n\t" \
            "mov %%ecx, %0\n\t" \
            "pushfq\n\t" \
            "pop %q1\n\t" \
            : "=r" (ecx), "=r" (eflags) \
            : "x" (xmm2), "x" (xmm1), "a" (eax), "d" (edx) \
            : "ecx", "cc", "memory" \
        );
    
    switch (imm8) {
        case 0x00: _ASM_VPCMPESTRI(0x00); break;
        case 0x04: _ASM_VPCMPESTRI(0x04); break;
        case 0x40: _ASM_VPCMPESTRI(0x40); break;
        case 0x44: _ASM_VPCMPESTRI(0x44); break;
        default:
            printf("Unsupported imm8 value: 0x%02X\n", imm8);
            return -1;
    }
    
    #undef _ASM_VPCMPESTRI
    
    printf("Result index: %d\n", ecx);
    print_eflags_cond(eflags, X_CF|X_ZF|X_SF|X_OF);
    VERIFY(ecx == 0, "Index should be 0 for equal strings");
    VERIFY((eflags & X_CF) != 0, "CF should be set (IntRes2 not zero)");
    VERIFY((eflags & X_ZF) != 0, "ZF should be set (EDX < 16)");
    VERIFY((eflags & X_SF) != 0, "SF should be set (EAX < 16)");
    VERIFY((eflags & X_OF) != 0, "OF should be set (IntRes2[0] = 1)");
    
    // 其他测试用例保持类似结构，确保拼写正确
    
    return fails;
}

int main() {
    int failures = test_vpcmpestri();
    printf("\n=== Test Summary ===\n");
    printf("Total failures: %d\n", failures);
    return failures ? 1 : 0;
}
