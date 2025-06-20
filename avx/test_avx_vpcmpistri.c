#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 打印字符串
static void print_str(const char *name, const char *str, int len) {
    printf("%s: \"", name);
    for (int i = 0; i < len; i++) {
        if (str[i] >= 32 && str[i] <= 126) {
            printf("%c", str[i]);
        } else {
            printf("\\x%02x", (unsigned char)str[i]);
        }
    }
    printf("\"\n");
}

// 测试VPCMPISTRI指令
#define TEST_VPCMPISTRI(imm8_val) \
    do { \
        __m128i va = _mm_loadu_si128((const __m128i*)a); \
        __m128i vb = _mm_loadu_si128((const __m128i*)b); \
        int result = _mm_cmpistri(va, vb, imm8_val); \
        \
        printf("\nTest: %s (Mode:0x%02x)\n", name, imm8_val); \
        print_str("Operand A", a, 16); \
        print_str("Operand B", b, 16); \
        printf("Result index: %d\n", result); \
    } while(0)

static void test_vpcmpistri(const char *name, const char *a, const char *b, int imm8) {
    switch(imm8) {
        case 0x00: TEST_VPCMPISTRI(0x00); break;
        case 0x04: TEST_VPCMPISTRI(0x04); break;
        case 0x08: TEST_VPCMPISTRI(0x08); break;
        case 0x0C: TEST_VPCMPISTRI(0x0C); break;
        default:
            printf("Unsupported immediate value: 0x%02x\n", imm8);
    }
}

int main() {
    // 测试数据
    char str1[16] = "HelloWorld123456";
    char str2[16] = "World";
    char str3[16] = "hello";
    char str4[16] = "123456";
    char empty[16] = {0};
    
    // 测试不同比较模式
    test_vpcmpistri("Equal any", str1, str2, 0x00); // 任意字符相等
    test_vpcmpistri("Equal range", str1, str3, 0x04); // 范围比较
    test_vpcmpistri("Equal each", str1, str1, 0x08); // 逐个字符比较
    test_vpcmpistri("Equal ordered", str1, str4, 0x0C); // 子串匹配
    
    // 边界测试
    test_vpcmpistri("Empty string", empty, str1, 0x00);
    test_vpcmpistri("Both empty", empty, empty, 0x00);
    
    return 0;
}
