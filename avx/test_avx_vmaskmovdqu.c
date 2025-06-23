#include "avx.h"
#include <stdio.h>
#include <string.h>

#define TEST_CASE(name) printf("\n=== Test Case: %s ===\n", name)

// 测试VMASKMOVDQU指令
static void test_vmaskmovdqu_case(const char* name, 
                                 const uint8_t* src_data,
                                 const uint8_t* mask_data,
                                 const uint8_t* expected_data) {
    TEST_CASE(name);
    
    // 准备内存区域（故意不对齐）
    uint8_t buffer[32] __attribute__((aligned(32)));
    uint8_t* dest = buffer + 1; // 非对齐地址
    
    // 打印输入数据
    printf("Source Data: ");
    for (int i = 0; i < 16; i++) printf("%02X ", src_data[i]);
    printf("\nMask Data:   ");
    for (int i = 0; i < 16; i++) printf("%02X ", mask_data[i]);
    printf("\n");
    
    // 准备目标内存区域（初始化为0）
    memset(dest, 0, 16);
    
    // 使用内联汇编执行指令
    __m128i src = _mm_loadu_si128((__m128i*)src_data);
    __m128i mask = _mm_loadu_si128((__m128i*)mask_data);
    uint64_t eflags = 0;
    
    // 获取EFLAGS初始状态
    asm volatile ("pushfq; pop %0" : "=r"(eflags) : : "memory");
    
    // 执行VMASKMOVDQU指令（显式设置RDI）
    asm volatile (
        "mov %[dest], %%rdi\n\t"  // 设置目标内存地址到RDI
        "vmaskmovdqu %[mask], %[src]"
        : [src] "+x"(src)
        : [mask] "x"(mask), [dest] "r"(dest)
        : "rdi", "cc", "memory"
    );
    
    // 获取EFLAGS最终状态
    asm volatile ("pushfq; pop %0" : "=r"(eflags) : : "memory");
    
    // 打印结果
    printf("Result Data: ");
    for (int i = 0; i < 16; i++) printf("%02X ", dest[i]);
    printf("\n");
    
    // 验证结果
    int match = memcmp(dest, expected_data, 16) == 0;
    printf("Result %s expected value\n", match ? "matches" : "does not match");
    
    if (!match) {
        printf("Expected: ");
        for (int i = 0; i < 16; i++) printf("%02X ", expected_data[i]);
        printf("\n");
    }
    
    // 打印EFLAGS状态
    printf("EFLAGS after operation:\n");
    print_eflags_cond(eflags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
}

void test_vmaskmovdqu() {
    // 测试用例1: 交替mask
    uint8_t src1[16] = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
        0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00
    };
    uint8_t mask1[16] = {
        0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
        0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00
    };
    uint8_t expected1[16] = {
        0x11, 0x00, 0x33, 0x00, 0x55, 0x00, 0x77, 0x00,
        0x99, 0x00, 0xBB, 0x00, 0xDD, 0x00, 0xFF, 0x00
    };
    
    // 测试用例2: 全1 mask
    uint8_t src2[16] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };
    uint8_t mask2[16];
    memset(mask2, 0xFF, sizeof(mask2)); // 全1
    uint8_t expected2[16] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };
    
    // 测试用例3: 全0 mask
    uint8_t src3[16] = {
        0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
        0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF, 0xF0
    };
    uint8_t mask3[16] = {0}; // 全0
    uint8_t expected3[16] = {0}; // 应该没有写入
    
    // 执行测试
    test_vmaskmovdqu_case("Alternating Mask", src1, mask1, expected1);
    test_vmaskmovdqu_case("All 1s Mask", src2, mask2, expected2);
    test_vmaskmovdqu_case("All 0s Mask", src3, mask3, expected3);
}

int main() {
    CLEAR_FLAGS;
    test_vmaskmovdqu();
    return 0;
}
