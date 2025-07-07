#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试寄存器操作数
static void test_reg(uint8_t index, uint16_t value, __m128i expected, const char* desc) {
    __m128i src = _mm_setr_epi16(0x0001, 0x0002, 0x0003, 0x0004, 
                                 0x0005, 0x0006, 0x0007, 0x0008);
    __m128i dst;
    uint32_t src_dword = (uint32_t)value;
    
    switch(index) {
        case 0:
            asm volatile ("vpinsrw $0, %1, %2, %0" : "=x"(dst) : "r"(src_dword), "x"(src) : "memory");
            break;
        case 1:
            asm volatile ("vpinsrw $1, %1, %2, %0" : "=x"(dst) : "r"(src_dword), "x"(src) : "memory");
            break;
        case 2:
            asm volatile ("vpinsrw $2, %1, %2, %0" : "=x"(dst) : "r"(src_dword), "x"(src) : "memory");
            break;
        case 3:
            asm volatile ("vpinsrw $3, %1, %2, %0" : "=x"(dst) : "r"(src_dword), "x"(src) : "memory");
            break;
        case 4:
            asm volatile ("vpinsrw $4, %1, %2, %0" : "=x"(dst) : "r"(src_dword), "x"(src) : "memory");
            break;
        case 5:
            asm volatile ("vpinsrw $5, %1, %2, %0" : "=x"(dst) : "r"(src_dword), "x"(src) : "memory");
            break;
        case 6:
            asm volatile ("vpinsrw $6, %1, %2, %0" : "=x"(dst) : "r"(src_dword), "x"(src) : "memory");
            break;
        case 7:
            asm volatile ("vpinsrw $7, %1, %2, %0" : "=x"(dst) : "r"(src_dword), "x"(src) : "memory");
            break;
        case 8: // 回绕到0
            asm volatile ("vpinsrw $0, %1, %2, %0" : "=x"(dst) : "r"(src_dword), "x"(src) : "memory");
            break;
        case 15: // 回绕到7
            asm volatile ("vpinsrw $7, %1, %2, %0" : "=x"(dst) : "r"(src_dword), "x"(src) : "memory");
            break;
        default:
            printf("Invalid index: %d\n", index);
            return;
    }
    
    print_xmm("  SRC     ", src);
    printf("  Word    : 0x%04X\n", value);
    printf("  Index   : %d\n", index);
    print_xmm("  Expected", expected);
    print_xmm("  Actual  ", dst);
    
    if (!cmp_xmm(dst, expected)) {
        printf("  [FAIL] %s\n\n", desc);
    } else {
        printf("  [PASS] %s\n\n", desc);
    }
}

// 测试内存操作数
static void test_mem(uint8_t index, uint16_t value, __m128i expected, const char* desc) {
    __m128i src = _mm_setr_epi16(0x0001, 0x0002, 0x0003, 0x0004, 
                                 0x0005, 0x0006, 0x0007, 0x0008);
    __m128i dst;
    ALIGNED(16) uint16_t mem_word = value;
    
    switch(index) {
        case 1:
            asm volatile ("vpinsrw $1, %1, %2, %0" : "=x"(dst) : "m"(mem_word), "x"(src) : "memory");
            break;
        case 2:
            asm volatile ("vpinsrw $2, %1, %2, %0" : "=x"(dst) : "m"(mem_word), "x"(src) : "memory");
            break;
        case 4:
            asm volatile ("vpinsrw $4, %1, %2, %0" : "=x"(dst) : "m"(mem_word), "x"(src) : "memory");
            break;
        case 5:
            asm volatile ("vpinsrw $5, %1, %2, %0" : "=x"(dst) : "m"(mem_word), "x"(src) : "memory");
            break;
        default:
            printf("Invalid index for mem: %d\n", index);
            return;
    }
    
    print_xmm("  SRC     ", src);
    printf("  Word    : 0x%04X\n", value);
    printf("  Index   : %d\n", index);
    print_xmm("  Expected", expected);
    print_xmm("  Actual  ", dst);
    
    if (!cmp_xmm(dst, expected)) {
        printf("  [FAIL] %s\n\n", desc);
    } else {
        printf("  [PASS] %s\n\n", desc);
    }
}

// 测试VPINSRW指令
int test_vpinsrw() {
    int ret = 0;
    const char* test_name = "VPINSRW tests";
    
    printf("=== %s ===\n", test_name);
    
    // 寄存器操作数测试
    test_reg(0, 0x1234, _mm_setr_epi16(0x1234, 0x0002, 0x0003, 0x0004, 
                                        0x0005, 0x0006, 0x0007, 0x0008),
             "VPINSRW xmm, xmm, reg, 0 (insert at pos 0)");
    
    test_reg(3, 0x5678, _mm_setr_epi16(0x0001, 0x0002, 0x0003, 0x5678, 
                                        0x0005, 0x0006, 0x0007, 0x0008),
             "VPINSRW xmm, xmm, reg, 3 (insert at pos 3)");
    
    test_reg(7, 0x9ABC, _mm_setr_epi16(0x0001, 0x0002, 0x0003, 0x0004, 
                                        0x0005, 0x0006, 0x0007, 0x9ABC),
             "VPINSRW xmm, xmm, reg, 7 (insert at pos 7)");
    
    // 内存操作数测试
    test_mem(2, 0xDEF0, _mm_setr_epi16(0x0001, 0x0002, 0xDEF0, 0x0004, 
                                        0x0005, 0x0006, 0x0007, 0x0008),
             "VPINSRW xmm, xmm, mem, 2 (insert at pos 2)");
    
    test_mem(5, 0x0FED, _mm_setr_epi16(0x0001, 0x0002, 0x0003, 0x0004, 
                                        0x0005, 0x0FED, 0x0007, 0x0008),
             "VPINSRW xmm, xmm, mem, 5 (insert at pos 5)");
    
    // 边界值测试
    test_reg(0, 0x0000, _mm_setr_epi16(0x0000, 0x0002, 0x0003, 0x0004, 
                                        0x0005, 0x0006, 0x0007, 0x0008),
             "VPINSRW xmm, xmm, reg, 0 (min value)");
    
    test_reg(7, 0xFFFF, _mm_setr_epi16(0x0001, 0x0002, 0x0003, 0x0004, 
                                        0x0005, 0x0006, 0x0007, 0xFFFF),
             "VPINSRW xmm, xmm, reg, 7 (max value)");
    
    // 特殊值测试
    test_mem(4, 0x7FFF, _mm_setr_epi16(0x0001, 0x0002, 0x0003, 0x0004, 
                                        0x7FFF, 0x0006, 0x0007, 0x0008),
             "VPINSRW xmm, xmm, mem, 4 (0x7FFF)");
    
    test_mem(1, 0x8000, _mm_setr_epi16(0x0001, 0x8000, 0x0003, 0x0004, 
                                        0x0005, 0x0006, 0x0007, 0x0008),
             "VPINSRW xmm, xmm, mem, 1 (0x8000)");
    
    // 索引超出范围测试（应该回绕）
    test_reg(8, 0xABCD, _mm_setr_epi16(0xABCD, 0x0002, 0x0003, 0x0004, 
                                        0x0005, 0x0006, 0x0007, 0x0008),
             "VPINSRW xmm, xmm, reg, 8 (wrap to 0)");
    
    test_reg(15, 0xDCBA, _mm_setr_epi16(0x0001, 0x0002, 0x0003, 0x0004, 
                                        0x0005, 0x0006, 0x0007, 0xDCBA),
             "VPINSRW xmm, xmm, reg, 15 (wrap to 7)");
    
    printf("=== %s completed ===\n", test_name);
    return 0;
}

int main() {
    return test_vpinsrw();
}
