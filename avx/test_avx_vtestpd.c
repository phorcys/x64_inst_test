#include "avx.h"
#include <stdio.h>
#include <stdint.h>

static void print_flags(uint32_t flags) {
    printf("Flags: CF=%d, ZF=%d\n", (flags & 1), (flags >> 6) & 1);
}

static void test_vtestpd() {
    printf("=== Testing vtestpd (packed double-precision bit test) ===\n");
    int total_tests = 0;
    int passed_tests = 0;
    
    // 测试256位寄存器-寄存器操作
    printf("\n[Test 1: 256-bit reg-reg - all signs match]\n");
    {
        // 正数: 符号位0
        ALIGNED(32) double a[4] = {1.0, 2.0, 3.0, 4.0};
        ALIGNED(32) double b[4] = {5.0, 6.0, 7.0, 8.0};
        uint32_t flags = 0;
        
        CLEAR_FLAGS;
        __asm__ __volatile__(
            "vmovapd %1, %%ymm0\n\t"
            "vmovapd %2, %%ymm1\n\t"
            "vtestpd %%ymm1, %%ymm0\n\t"
            "pushf\n\t"
            "pop %0\n\t"
            : "=r"(flags)
            : "m"(*a), "m"(*b)
            : "ymm0", "ymm1"
        );
        
        print_double_vec("Operand A", a, 4);
        print_double_vec("Operand B", b, 4);
        print_flags(flags);
        
        // 预期: ZF=1 (所有符号匹配), CF=0 (NOT B的符号与A不匹配)
        int pass = 1;
        if (!(flags & (1 << 6))) { // ZF位
            printf("Expected ZF=1, got ZF=0\n");
            pass = 0;
        }
        if (flags & 1) { // CF位
            printf("Expected CF=0, got CF=1\n");
            pass = 0;
        }
        
        if (pass) {
            printf("[PASS] Test 1\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 1\n");
        }
        total_tests++;
    }
    
    printf("\n[Test 2: 256-bit reg-reg - mixed signs]\n");
    {
        // 混合符号
        ALIGNED(32) double a[4] = {-1.0, 2.0, -3.0, 4.0};
        ALIGNED(32) double b[4] = {-1.0, -2.0, 3.0, 4.0};
        uint32_t flags = 0;
        
        CLEAR_FLAGS;
        __asm__ __volatile__(
            "vmovapd %1, %%ymm0\n\t"
            "vmovapd %2, %%ymm1\n\t"
            "vtestpd %%ymm1, %%ymm0\n\t"
            "pushf\n\t"
            "pop %0\n\t"
            : "=r"(flags)
            : "m"(*a), "m"(*b)
            : "ymm0", "ymm1"
        );
        
        print_double_vec("Operand A", a, 4);
        print_double_vec("Operand B", b, 4);
        print_flags(flags);
        
        // 预期: ZF=0, CF=0
        int pass = 1;
        if (flags & (1 << 6)) { // ZF位
            printf("Expected ZF=0, got ZF=1\n");
            pass = 0;
        }
        if (flags & 1) { // CF位
            printf("Expected CF=0, got CF=1\n");
            pass = 0;
        }
        
        if (pass) {
            printf("[PASS] Test 2\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 2\n");
        }
        total_tests++;
    }
    
    printf("\n[Test 3: 256-bit reg-reg - A signs opposite of B]\n");
    {
        // A符号位与B相反
        ALIGNED(32) double a[4] = {-1.0, -2.0, -3.0, -4.0};
        ALIGNED(32) double b[4] = {1.0, 2.0, 3.0, 4.0};
        uint32_t flags = 0;
        
        CLEAR_FLAGS;
        __asm__ __volatile__(
            "vmovapd %1, %%ymm0\n\t"
            "vmovapd %2, %%ymm1\n\t"
            "vtestpd %%ymm1, %%ymm0\n\t"
            "pushf\n\t"
            "pop %0\n\t"
            : "=r"(flags)
            : "m"(*a), "m"(*b)
            : "ymm0", "ymm1"
        );
        
        print_double_vec("Operand A", a, 4);
        print_double_vec("Operand B", b, 4);
        print_flags(flags);
        
        // 预期: ZF=0 (符号不匹配), CF=1 (NOT B的符号与A完全匹配)
        int pass = 1;
        if (flags & (1 << 6)) { // ZF位
            printf("Expected ZF=0, got ZF=1\n");
            pass = 0;
        }
        if (!(flags & 1)) { // CF位
            printf("Expected CF=1, got CF=0\n");
            pass = 0;
        }
        
        if (pass) {
            printf("[PASS] Test 3\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 3\n");
        }
        total_tests++;
    }
    
    printf("\n[Test 4: 128-bit reg-mem]\n");
    {
        ALIGNED(16) double a[2] = {1.0, -2.0};
        ALIGNED(16) double b[2] = {1.0, -2.0};
        uint32_t flags = 0;
        
        CLEAR_FLAGS;
        __asm__ __volatile__(
            "vmovapd %1, %%xmm0\n\t"
            "vtestpd %2, %%xmm0\n\t"
            "pushf\n\t"
            "pop %0\n\t"
            : "=r"(flags)
            : "m"(*a), "m"(*b)
            : "xmm0"
        );
        
        print_double_vec("Operand A", a, 2);
        print_double_vec("Memory Operand", b, 2);
        print_flags(flags);
        
        // 预期: ZF=1 (所有符号匹配), CF=0
        int pass = 1;
        if (!(flags & (1 << 6))) { // ZF位
            printf("Expected ZF=1, got ZF=0\n");
            pass = 0;
        }
        if (flags & 1) { // CF位
            printf("Expected CF=0, got CF=1\n");
            pass = 0;
        }
        
        if (pass) {
            printf("[PASS] Test 4\n");
            passed_tests++;
        } else {
            printf("[FAIL] Test 4\n");
        }
        total_tests++;
    }
    
    // 测试总结
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed tests: %d\n", passed_tests);
    printf("Failed tests: %d\n", total_tests - passed_tests);
    
    if (passed_tests == total_tests) {
        printf("All vtestpd tests passed successfully!\n");
    } else {
        printf("Some vtestpd tests failed\n");
    }
}

int main() {
    test_vtestpd();
    return 0;
}
