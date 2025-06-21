#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

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


    #define _ASM_VPCMPESTRM(imm) \
    asm volatile ( \
        "vpcmpestrm $" #imm ", %3, %2\n\t" \
        "mov %%ecx, %0\n\t" \
        "pushfq\n\t" \
        "pop %q1\n\t" \
        : "=r" (ecx), "=r" (eflags) \
        : "x" (xmm2), "x" (xmm1), "a" (eax), "d" (edx) \
        : "ecx", "cc", "memory" \
    );

// 使用宏封装vpcmpestrm指令（使用位置参数）
#define vpcmpestrm_wrapper(imm8, a, b, eax_val, edx_val, result, eflags) do { \
    __m128i res; \
    uint64_t flags; \
    asm volatile ( \
        "vpcmpestrm $%6, %2, %3\n\t" \
        "movdqa %%xmm0, %0\n\t" \
        "pushfq\n\t" \
        "pop %q1\n\t" \
        : "=x" (res), "=r" (flags) \
        : "x" (a), "x" (b), \
          "a" (eax_val), "d" (edx_val), "i" (imm8) \
        : "ecx", "cc", "xmm0" \
    ); \
    *(result) = res; \
    *(eflags) = (uint32_t)(flags & 0xFFFFFFFF); \
} while(0)


static int test_vpcmpestrm() {
    printf("=== Testing VPCMPESTRM ===\n");
    int fails = 0;
    
    // 测试数据
    uint8_t str1[32] = "HelloWorld12345";
    uint8_t str2[32] = "HelloBox64";
    uint8_t str4[32] = {0}; // 空字符串
    
    __m128i xmm1, xmm2, result;
    int32_t eax, edx, ecx;
    uint32_t eflags;
    
    // 测试1: 相等比较(字节模式) - 位掩码
    printf("\nTest 1: Equal comparison (byte mode, bit mask)\n");
    xmm1 = _mm_loadu_si128((__m128i*)str1);
    xmm2 = _mm_loadu_si128((__m128i*)str1);
    eax = edx = 15; // 比较15个字节
    
    CLEAR_FLAGS;
    _ASM_VPCMPESTRM(0x00);
    
    print_xmm("Result mask", result);
    print_eflags_cond(eflags, X_CF|X_ZF|X_SF|X_OF);
    
    // 测试2: 不等比较(字节模式) - 字节掩码
    printf("\nTest 2: Unequal comparison (byte mode, byte mask)\n");
    xmm1 = _mm_loadu_si128((__m128i*)str1);
    xmm2 = _mm_loadu_si128((__m128i*)str2);
    eax = edx = 10;
    
    CLEAR_FLAGS;
    _ASM_VPCMPESTRM(0x44);
    
    print_xmm("Result mask", result);
    print_eflags_cond(eflags, X_CF|X_ZF|X_SF|X_OF);

    
    // 测试3: 边界值测试(字节模式)
    printf("\nTest 3: Boundary values (byte mode)\n");
    uint8_t bound_str1[16] = {0x00, 0xFF, 0x80, 0x7F, 0x01, 0xFE, 0x81, 0x7E};
    uint8_t bound_str2[16] = {0x00, 0x00, 0x80, 0x80, 0x01, 0x01, 0x81, 0x81};
    eax = edx = 8;
    
    xmm1 = _mm_loadu_si128((__m128i*)bound_str1);
    xmm2 = _mm_loadu_si128((__m128i*)bound_str2);
    
    CLEAR_FLAGS;
    _ASM_VPCMPESTRM(0x04);
    
    print_xmm("Result mask", result);

    
    // 测试4: 长度溢出测试
    printf("\nTest 4: Length overflow\n");
    xmm1 = _mm_loadu_si128((__m128i*)str1);
    xmm2 = _mm_loadu_si128((__m128i*)str1);
    eax = edx = 20; // >16, 应饱和到16
    
    CLEAR_FLAGS;
    _ASM_VPCMPESTRM(0x00);;
    
    print_eflags_cond(eflags, X_CF|X_ZF|X_SF|X_OF);

    
    // 测试5: 空字符串测试
    printf("\nTest 5: Empty strings\n");
    xmm1 = _mm_loadu_si128((__m128i*)str4);
    xmm2 = _mm_loadu_si128((__m128i*)str4);
    eax = edx = 0;
    
    CLEAR_FLAGS;
    _ASM_VPCMPESTRM(0x00);
    
    print_xmm("Result mask", result);

    // 测试6: 负长度测试
    printf("\nTest 6: Negative length\n");
    xmm1 = _mm_loadu_si128((__m128i*)str1);
    xmm2 = _mm_loadu_si128((__m128i*)str2);
    eax = -10;  // 负长度，应取绝对值为10
    edx = -10;
    
    CLEAR_FLAGS;
    _ASM_VPCMPESTRM(0x44);
    
    print_xmm("Result mask", result);

    
    // 测试7: 字(word)模式测试
    printf("\nTest 7: Word mode comparison\n");
    uint16_t words1[8] = {0x1234, 0x5678, 0x9ABC, 0xDEF0, 0x1234, 0x5678, 0x9ABC, 0xDEF0};
    uint16_t words2[8] = {0x1234, 0x5679, 0x9ABC, 0xDEF0, 0x1234, 0x5678, 0x9ABC, 0xDEF0};
    eax = edx = 7;  // 比较7个字
    
    xmm1 = _mm_loadu_si128((__m128i*)words1);
    xmm2 = _mm_loadu_si128((__m128i*)words2);
    
    CLEAR_FLAGS;
    _ASM_VPCMPESTRM(0x4c);
    
    print_xmm("Result mask", result);

    
    return fails;
}

int main() {
    int failures = test_vpcmpestrm();
    printf("\n=== Test Summary ===\n");
    printf("Total failures: %d\n", failures);
    return failures ? 1 : 0;
}
