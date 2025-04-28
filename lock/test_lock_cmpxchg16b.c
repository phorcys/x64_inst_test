#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

// 测试配置
#define PADDING_SIZE 16
#define NUM_THREADS 4
#define NUM_ITERATIONS 10000

typedef struct {
    uint64_t high;
    uint64_t low;
} uint128_t;

// 内联汇编宏
static inline uint64_t cmpxchg16b(uint128_t *dest, uint128_t *expected, const uint128_t desired) {
    uint64_t flags;
    __asm__ volatile (
        "lock cmpxchg16b %1\n\t"
        "pushfq\n\t"
        "popq %0"
        : "=r" (flags), "+m" (*dest), "+d" (expected->high), "+a" (expected->low)
        : "c" (desired.high), "b" (desired.low)
        : "cc", "memory"
    );
    return flags;
}

// 初始化128位值
void init_uint128(uint128_t *val, uint64_t high, uint64_t low) {
    val->high = high;
    val->low = low;
}

// 打印128位值和标志位
void print_result(const char *label, const uint128_t *val, uint64_t flags) {
    printf("%s:\n", label);
    printf("  Value: 0x%016lx%016lx\n", val->high, val->low);
    printf("  Flags: ZF=%lu\n",
           (flags >> 6) & 1  // ZF
           ); // OF
}

// 基本功能测试
void test_basic() {
    printf("=== Basic Functionality Tests ===\n");
    
    uint128_t mem, expected, desired;
    
    // 测试1: 比较相等，应该交换
    init_uint128(&mem, 0x1111111111111111, 0x2222222222222222);
    init_uint128(&expected, 0x1111111111111111, 0x2222222222222222);
    init_uint128(&desired, 0x3333333333333333, 0x4444444444444444);
    
    print_result("Before (equal)", &mem, 0);
    uint64_t flags = cmpxchg16b(&mem, &expected, desired);
    print_result("After (equal)", &mem, flags);
    printf("Expected: "); print_result("", &expected, 0);
    
    // 测试2: 比较不相等，不应该交换
    init_uint128(&mem, 0x5555555555555555, 0x6666666666666666);
    init_uint128(&expected, 0x1111111111111111, 0x2222222222222222);
    init_uint128(&desired, 0x7777777777777777, 0x8888888888888888);
    
    print_result("\nBefore (not equal)", &mem, 0);
    flags = cmpxchg16b(&mem, &expected, desired);
    print_result("After (not equal)", &mem, flags);
    printf("Expected: "); print_result("", &expected, 0);
}

// 边界值测试
void test_boundary() {
    printf("\n=== Boundary Value Tests ===\n");
    
    uint128_t mem, expected, desired;
    uint64_t flags;
    
    // 全0测试
    init_uint128(&mem, 0x0, 0x0);
    init_uint128(&expected, 0x0, 0x0);
    init_uint128(&desired, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
    
    flags = cmpxchg16b(&mem, &expected, desired);
    printf("All zeros test: %s\n", (flags & (1 << 6)) ? "PASS" : "FAIL");
    print_result("Result", &mem, flags);
    
    // 全1测试
    init_uint128(&mem, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
    init_uint128(&expected, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
    init_uint128(&desired, 0x0, 0x0);
    
    flags = cmpxchg16b(&mem, &expected, desired);
    printf("\nAll ones test: %s\n", (flags & (1 << 6)) ? "PASS" : "FAIL");
    print_result("Result", &mem, flags);
    
}

int main() {
    test_basic();
    test_boundary();
    return 0;
}
