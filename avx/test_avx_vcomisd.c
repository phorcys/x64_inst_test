#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 获取EFLAGS寄存器值
static inline uint64_t get_eflags() {
    uint64_t eflags;
    asm volatile ("pushfq\n\tpopq %0" : "=r"(eflags));
    return eflags;
}

// 设置EFLAGS寄存器值
static inline void set_eflags(uint64_t eflags) {
    asm volatile ("pushq %0\n\tpopfq" : : "r"(eflags) : "cc", "memory");
}

// 打印EFLAGS差异
static inline void print_eflags_diff(uint64_t before, uint64_t after) {
    uint32_t changed = before ^ after;
    printf("EFLAGS changed: ");
    if(changed & X_CF) printf("CF ");
    if(changed & X_PF) printf("PF ");
    if(changed & X_AF) printf("AF ");
    if(changed & X_ZF) printf("ZF ");
    if(changed & X_SF) printf("SF ");
    if(changed & X_OF) printf("OF ");
    printf("\n");
}

// 测试单个比较操作
static int test_case(const char *name, __m128d a, __m128d b, uint32_t expected_flags) {
    uint32_t actual_flags;
    
    // 执行比较并获取EFLAGS
    asm volatile(
        "vcomisd %[b], %[a]\n\t"
        "pushfq\n\t"
        "popq %%rax\n\t"
        "movl %%eax, %[flags]"
        : [flags] "=r"(actual_flags)
        : [a] "x"(a),
          [b] "xm"(b)
        : "cc", "rax"
    );
    
    // 提取相关标志位
    actual_flags &= (X_CF|X_PF|X_ZF);
    
    // 打印输入和输出
    printf("\nTest: %s\n", name);
    print_double_vec_hex("Operand A", (double*)&a, 2);
    print_double_vec_hex("Operand B", (double*)&b, 2);
    
    // 比较结果
    int ret = (actual_flags == expected_flags);
    printf("Expected flags: ");
    print_eflags_cond(expected_flags, X_CF|X_PF|X_ZF);
    printf("Actual flags:   ");
    print_eflags_cond(actual_flags, X_CF|X_PF|X_ZF);
    printf("Result: %s\n", ret ? "PASS" : "FAIL");
    return ret;
}

int main() {
    int total = 0, passed = 0;
    
    // 定义测试值
    double nan = NAN;
    double inf = INFINITY;
    double neg_inf = -INFINITY;
    
    // 相等比较
    __m128d a_eq = _mm_setr_pd(1.0, 2.0);
    __m128d b_eq = _mm_setr_pd(1.0, 3.0); // 只比较低位
    passed += test_case("VCOMISD equal", a_eq, b_eq, X_ZF);
    total++;
    
    // 小于比较
    __m128d a_lt = _mm_setr_pd(1.0, 2.0);
    __m128d b_lt = _mm_setr_pd(2.0, 1.0);
    passed += test_case("VCOMISD less than", a_lt, b_lt, X_CF);
    total++;
    
    // 大于比较
    __m128d a_gt = _mm_setr_pd(2.0, 1.0);
    __m128d b_gt = _mm_setr_pd(1.0, 2.0);
    passed += test_case("VCOMISD greater than", a_gt, b_gt, 0);
    total++;
    
    // 无序比较 (NaN)
    __m128d a_unord = _mm_setr_pd(nan, 2.0);
    __m128d b_unord = _mm_setr_pd(1.0, nan);
    passed += test_case("VCOMISD unordered (NaN)", a_unord, b_unord, X_PF|X_CF|X_ZF);
    total++;
    
    // 无穷大比较
    __m128d a_inf = _mm_setr_pd(inf, neg_inf);
    __m128d b_inf = _mm_setr_pd(100.0, -100.0);
    passed += test_case("VCOMISD with inf", a_inf, b_inf, 0); // inf > 100
    total++;
    
    // 内存操作数测试
    ALIGNED(16) double mem_ops[2] = {1.0, 2.0};
    __m128d a_mem = _mm_setr_pd(1.0, 3.0);
    
    uint32_t flags_before = get_eflags();
    set_eflags(0);
    
    uint64_t flags_after = 0;
    __m128d tmp = a_mem;
    
    asm volatile(
        "vcomisd (%[mem]), %[a]\n\t"
        "pushfq\n\t"
        "popq %[flags]"
        : [a] "+x"(tmp),
          [flags] "=r"(flags_after)
        : [mem] "r"(mem_ops)
        : "cc"
    );
    
    uint64_t mem_flags = (flags_after & (X_CF|X_PF|X_ZF));
    
    printf("\nTest: VCOMISD with memory operand\n");
    print_double_vec_hex("Operand A", (double*)&a_mem, 2);
    print_double_vec_hex("Operand B (mem)", mem_ops, 2);
    print_eflags_diff(flags_before, flags_after);
    
    int mem_result = (mem_flags == X_ZF); // 应该相等
    printf("Expected flags: ");
    print_eflags_cond(X_ZF, X_CF|X_PF|X_ZF);
    printf("Result: %s\n", mem_result ? "PASS" : "FAIL");
    passed += mem_result;
    total++;
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
