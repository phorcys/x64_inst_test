#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>

// 打印字符串
static void print_str(const char *name, const char *str, int len) {
    printf("%s: '", name);
    for (int i = 0; i < len; i++) {
        printf("%c", str[i] ? str[i] : '.');
    }
    printf("'\n");
}

// 测试单个比较操作
static int test_case(const char *name, const char *a, int alen, 
                    const char *b, int blen, int mode, int expected_idx) {
    __m128i va = _mm_loadu_si128((const __m128i*)a);
    __m128i vb = _mm_loadu_si128((const __m128i*)b);
    int result;
    uint64_t eflags; // 改为64位类型
    
    // 为每种模式创建独立的内联汇编块
    if (mode == _SIDD_CMP_EQUAL_ORDERED) {
        asm volatile(
            "vpcmpestri $0x0c, %[vb], %[va]\n\t"  // 修正立即数为0x0c
            "pushfq\n\t"
            "pop %[ef]\n\t"
            : [ef] "=r" (eflags), "=c" (result)
            : [va] "x" (va), [vb] "x" (vb), "a" (alen), "d" (blen)
            : "cc", "memory"
        );
    } else if (mode == _SIDD_CMP_RANGES) {
        asm volatile(
            "vpcmpestri $0x04, %[vb], %[va]\n\t"
            "pushfq\n\t"
            "pop %[ef]\n\t"
            : [ef] "=r" (eflags), "=c" (result)
            : [va] "x" (va), [vb] "x" (vb), "a" (alen), "d" (blen)
            : "cc", "memory"
        );
    } else if (mode == (_SIDD_CMP_EQUAL_EACH | _SIDD_UBYTE_OPS)) {
        asm volatile(
            "vpcmpestri $0x08, %[vb], %[va]\n\t"
            "pushfq\n\t"
            "pop %[ef]\n\t"
            : [ef] "=r" (eflags), "=c" (result)
            : [va] "x" (va), [vb] "x" (vb), "a" (alen), "d" (blen)
            : "cc", "memory"
        );
    } else {
        printf("Invalid mode: 0x%x\n", mode);
        return 0;
    }
    
    // 删除重复的模式检查和类型转换
    
    int passed = (result == expected_idx);
    
    printf("\nTest: %s (mode=0x%x)\n", name, mode);
    print_str("String A", a, alen);
    print_str("String B", b, blen);
    printf("Result index: %d (expected: %d)\n", result, expected_idx);
    printf("EFLAGS: 0x%016lx\n", eflags);
    // 添加调试输出
    printf("Mode: 0x%x, A len: %d, B len: %d\n", mode, alen, blen);
    printf("Result: %s\n", passed ? "PASS" : "FAIL");
    
    return passed;
}

int main() {
    int total = 0, passed = 0;
    
    // 测试相等比较
    {
        char a1[16] = "HelloWorld";
        char b1[16] = "HelloWorld";
        passed += test_case("Equal strings", a1, 10, b1, 10, 
                          _SIDD_CMP_EQUAL_ORDERED, 0); // 修正为匹配起始位置0
        total++;
        
        char a2[16] = "Hello";
        char b2[16] = "World";
        passed += test_case("Different strings", a2, 5, b2, 5,
                          _SIDD_CMP_EQUAL_ORDERED, 16); // 修正为无匹配16
        total++;
        
        char a3[16] = "";
        char b3[16] = "";
        passed += test_case("Empty strings", a3, 0, b3, 0,
                          _SIDD_CMP_EQUAL_ORDERED, 0); // 修正为0（空字符串长度）
        total++;
    }
    
    // 测试范围比较
    {
        char a1[16] = "ABCDEF";
        char b1[16] = "CD";
        passed += test_case("Range check (in)", a1, 6, b1, 2,
                          _SIDD_CMP_RANGES, 0); // 修正为第一个匹配位置
        total++;
        
        char a2[16] = "XYZ";
        char b2[16] = "AB";
        passed += test_case("Range check (out)", a2, 3, b2, 2,
                          _SIDD_CMP_RANGES, 16); // 修改预期为16
        total++;
    }
    
    // 测试不同模式
    {
        char a1[16] = "abc";
        char b1[16] = "ABC";
        passed += test_case("Case sensitive unequal", a1, 3, b1, 3,
                          _SIDD_CMP_EQUAL_ORDERED, 16); // 修正为无匹配16
        total++;
        
    // 移除大小写不敏感测试（指令本身不支持）
    // passed += test_case("Case insensitive equal", a1, 3, b1, 3,
    //                   _SIDD_CMP_EQUAL_EACH | _SIDD_UBYTE_OPS, 3);
    // total++;
    }
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
