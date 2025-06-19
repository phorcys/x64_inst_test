#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "avx.h"

// 内联汇编宏定义
#define BLSMSK32(dest, src) \
    asm volatile ("blsmsk %1, %0" : "=r"(dest) : "r"(src))

#define BLSMSK64(dest, src) \
    asm volatile ("blsmsk %1, %0" : "=r"(dest) : "r"(src))

static void test_blsmsk_32() {
    printf("=== Testing 32-bit BLSMSK instruction ===\n");
    
    struct TestCase32 {
        uint32_t input;
        uint32_t expected;
        uint32_t expected_cf;
    } cases[] = {
        {0x12345678, 0x0000000F, 0},  // 最低位在第3位，掩码为0-3位
        {0x00000000, 0xFFFFFFFF, 1},  // 输入0，CF=1
        {0x80000000, 0xFFFFFFFF, 0},  // 最高位
        {0x00000001, 0x00000001, 0},  // 最低位
        {0xFFFFFFF0, 0x0000001F, 0},  // 低位为0，修正期望值
        {0x0000000F, 0x00000001, 0},   // 低4位为1
        {0x00001000, 0x00001FFF, 0},   // 中间位测试(第12位)
        {0x00000000, 0xFFFFFFFF, 1},  // 重复零输入测试
        {0x00000001, 0x00000001, 0}    // 重复最低位测试
    };
    
    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
        uint32_t result;
        uint64_t rflags_before, rflags_after; // 使用64位RFLAGS寄存器
        
        // 保存RFLAGS
        asm volatile ("pushfq\n\tpop %0" : "=r"(rflags_before));
        
        // 执行指令
        // 测试寄存器操作数和内存操作数
        if (i % 2 == 0) {
            BLSMSK32(result, cases[i].input);  // 寄存器操作数
        } else {
            uint32_t mem_input = cases[i].input;
            asm volatile ("blsmsk %1, %0" : "=r"(result) : "m"(mem_input));  // 内存操作数
        }
        
        // 获取RFLAGS
        asm volatile ("pushfq\n\tpop %0" : "=r"(rflags_after));
        
        // 计算CF标志
        uint32_t cf = (rflags_after & 1) ? 1 : 0;
        
        printf("Test %zu: Input=0x%08X\n", i+1, cases[i].input);
        printf("  Expected: 0x%08X, Result: 0x%08X\n", cases[i].expected, result);
        printf("  Expected CF: %d, Actual CF: %d\n", cases[i].expected_cf, cf);
        printf("  RFLAGS: 0x%016lX\n", rflags_after);
        
        if (result != cases[i].expected || cf != cases[i].expected_cf) {
            printf("  [FAIL] Test failed!\n");
        } else {
            printf("  [PASS] Test passed!\n");
        }
        
        print_eflags_cond((uint32_t)rflags_after, 0x84D); // 只传递低32位
        printf("\n");
    }
}

static void test_blsmsk_64() {
    printf("=== Testing 64-bit BLSMSK instruction ===\n");
    
    struct TestCase64 {
        uint64_t input;
        uint64_t expected;
        uint32_t expected_cf;
    } cases[] = {
        {0x123456789ABCDEF0, 0x000000000000001F, 0},  // 修正期望值
        {0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 1},
        {0x8000000000000000, 0xFFFFFFFFFFFFFFFF, 0},
        {0x0000000000000001, 0x0000000000000001, 0},
        {0xFFFFFFFFFFFFFFF0, 0x000000000000001F, 0},  // 修正期望值
        {0x000000000000000F, 0x0000000000000001, 0},   // 低4位为1
        {0x0000000010000000, 0x000000001FFFFFFF, 0},   // 中间位测试(第28位)
        {0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 1},  // 重复零输入测试
        {0x0000000000000001, 0x0000000000000001, 0}    // 重复最低位测试
    };
    
    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
        uint64_t result;
        uint64_t rflags_before, rflags_after; // 使用64位RFLAGS寄存器
        
        // 保存RFLAGS
        asm volatile ("pushfq\n\tpop %0" : "=r"(rflags_before));
        
        // 执行指令
        // 测试寄存器操作数和内存操作数
        if (i % 2 == 0) {
            BLSMSK64(result, cases[i].input);  // 寄存器操作数
        } else {
            uint64_t mem_input = cases[i].input;
            asm volatile ("blsmsk %1, %0" : "=r"(result) : "m"(mem_input));  // 内存操作数
        }
        
        // 获取RFLAGS
        asm volatile ("pushfq\n\tpop %0" : "=r"(rflags_after));
        
        // 计算CF标志
        uint32_t cf = (rflags_after & 1) ? 1 : 0;
        
        printf("Test %zu: Input=0x%016" PRIX64 "\n", i+1, cases[i].input);
        printf("  Expected: 0x%016" PRIX64 ", Result: 0x%016" PRIX64 "\n", 
               cases[i].expected, result);
        printf("  Expected CF: %d, Actual CF: %d\n", cases[i].expected_cf, cf);
        printf("  RFLAGS: 0x%016lX\n", rflags_after);
        
        if (result != cases[i].expected || cf != cases[i].expected_cf) {
            printf("  [FAIL] Test failed!\n");
        } else {
            printf("  [PASS] Test passed!\n");
        }
        
        // 检查SF标志位是否正确
        uint32_t sf = (rflags_after & 0x80) ? 1 : 0;
        uint32_t expected_sf = (cases[i].expected >> 31) & 1;  // 对于32位测试
        if (i >= sizeof(cases)/sizeof(cases[0])) {
            expected_sf = (cases[i].expected >> 63) & 1;  // 对于64位测试
        }
        
        print_eflags_cond((uint32_t)rflags_after, 0x84D); // 显示CF/SF/ZF/OF
        
        if (sf != expected_sf) {
            printf("  [WARNING] SF flag mismatch! Expected: %d, Actual: %d\n", 
                  expected_sf, sf);
        }
        printf("\n");
    }
}

int main() {
    test_blsmsk_32();
    test_blsmsk_64();
    return 0;
}
