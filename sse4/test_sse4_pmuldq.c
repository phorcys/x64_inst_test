#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试PMULDQ指令(有符号32位乘法产生64位结果)
void test_pmuldq() {
    printf("Testing PMULDQ instruction\n");
    printf("-------------------------\n");
    
    // 测试数据
    struct TestData {
        int32_t a[4];
        int32_t b[4];
        int64_t expect[2];
    } tests[] = {
        // 正常值测试
        {{1, 2, 3, 4}, {5, 6, 7, 8}, {1L*5L, 3L*7L}},
        {{-1, -2, -3, -4}, {5, 6, 7, 8}, {-1L*5L, -3L*7L}},
        // 边界值测试
        {{0x7FFFFFFF, 0, 0x80000000, 0}, {1, 0, -1, 0}, 
         {0x7FFFFFFFL*1L, 0x80000000L*1L}},  // PMULDQ treats 0x80000000 as +2147483648
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nTest case %zu:\n", i+1);
        
        // 寄存器版本测试
        int64_t res_reg[2];
        __asm__ volatile (
            "movdqu %1, %%xmm0\n\t"
            "movdqu %2, %%xmm1\n\t"
            "pmuldq %%xmm1, %%xmm0\n\t"
            "movdqu %%xmm0, %0\n\t"
            : "=m" (res_reg)
            : "m" (tests[i].a), "m" (tests[i].b)
            : "xmm0", "xmm1"
        );
        
        printf("Register version:\n");
        printf("  Result: [%ld, %ld]\n", res_reg[0], res_reg[1]);
        printf("  Expect: [%ld, %ld]\n", 
               tests[i].expect[0], tests[i].expect[1]);
        printf("  %s\n", 
               (res_reg[0] != tests[i].expect[0] || res_reg[1] != tests[i].expect[1]) ? "FAIL" : "PASS");

        // 内存版本测试
        int64_t res_mem[2];
        __asm__ volatile (
            "movdqu %1, %%xmm0\n\t"
            "pmuldq %2, %%xmm0\n\t"
            "movdqu %%xmm0, %0\n\t"
            : "=m" (res_mem)
            : "m" (tests[i].a), "m" (tests[i].b)
            : "xmm0"
        );
        
        printf("Memory version:\n");
        printf("  Result: [%ld, %ld]\n", res_mem[0], res_mem[1]);
        printf("  Expect: [%ld, %ld]\n", 
               tests[i].expect[0], tests[i].expect[1]);
        printf("  %s\n", 
               (res_mem[0] != tests[i].expect[0] || res_mem[1] != tests[i].expect[1]) ? "FAIL" : "PASS");
    }
}

int main() {
    test_pmuldq();
    return 0;
}
