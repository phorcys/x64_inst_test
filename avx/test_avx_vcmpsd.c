#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 比较谓词宏定义
#define CMP_EQ       0x00
#define CMP_LT       0x01
#define CMP_LE       0x02
#define CMP_UNORD    0x03
#define CMP_NEQ      0x04
#define CMP_NLT      0x05
#define CMP_NLE      0x06
#define CMP_ORD      0x07

// 内联汇编实现 vcmpsd
static void vcmpsd(__m128d *dst, __m128d a, __m128d b, int imm) {
    switch(imm) {
        case CMP_EQ:
            asm volatile("vcmpsd $0, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_LT:
            asm volatile("vcmpsd $1, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_LE:
            asm volatile("vcmpsd $2, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_UNORD:
            asm volatile("vcmpsd $3, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NEQ:
            asm volatile("vcmpsd $4, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NLT:
            asm volatile("vcmpsd $5, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NLE:
            asm volatile("vcmpsd $6, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_ORD:
            asm volatile("vcmpsd $7, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        default:
            *dst = _mm_setzero_pd();
            break;
    }
}

// 测试单个比较操作
static int test_case(const char *name, __m128d a, __m128d b, int imm, __m128d expected) {
    __m128d res = _mm_setzero_pd();
    vcmpsd(&res, a, b, imm);
    
    // 打印输入和输出
    printf("\nTest: %s\n", name);
    print_double_vec_hex("Operand A", (double*)&a, 2);
    print_double_vec_hex("Operand B", (double*)&b, 2);
    print_double_vec_hex("Expected", (double*)&expected, 2);
    print_double_vec_hex("Result  ", (double*)&res, 2);
    
    // 比较结果
    uint64_t res_low = *((uint64_t*)&res);
    uint64_t exp_low = *((uint64_t*)&expected);
    int ret = (res_low == exp_low);
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
    __m128d b_eq = _mm_setr_pd(1.0, 2.0);
    __m128d exp_eq = _mm_castsi128_pd(_mm_set_epi64x(0, 0xFFFFFFFFFFFFFFFF)); // 低64位全1
    passed += test_case("vcmpsd EQ (equal)", a_eq, b_eq, CMP_EQ, exp_eq);
    total++;
    
    // 不等比较
    __m128d a_neq = _mm_setr_pd(1.0, 2.0);
    __m128d b_neq = _mm_setr_pd(0.0, 3.0);
    __m128d exp_neq = _mm_castsi128_pd(_mm_set_epi64x(0, 0xFFFFFFFFFFFFFFFF)); // 全部不等
    passed += test_case("vcmpsd NEQ (not equal)", a_neq, b_neq, CMP_NEQ, exp_neq);
    total++;
    
    // 小于比较
    __m128d a_lt = _mm_setr_pd(1.0, 3.0);
    __m128d b_lt = _mm_setr_pd(2.0, 1.0);
    __m128d exp_lt = _mm_castsi128_pd(_mm_set_epi64x(0, 0xFFFFFFFFFFFFFFFF)); // 1.0 < 2.0 -> true
    passed += test_case("vcmpsd LT (less than)", a_lt, b_lt, CMP_LT, exp_lt);
    total++;
    
    // 无序比较 (NaN)
    __m128d a_unord = _mm_setr_pd(1.0, nan);
    __m128d b_unord = _mm_setr_pd(nan, 2.0);
    __m128d exp_unord = _mm_castsi128_pd(_mm_set_epi64x(0, 0xFFFFFFFFFFFFFFFF)); // 全部无序
    passed += test_case("vcmpsd UNORD (unordered)", a_unord, b_unord, CMP_UNORD, exp_unord);
    total++;
    
    // 有序比较 (NaN)
    __m128d a_ord = _mm_setr_pd(1.0, nan);
    __m128d b_ord = _mm_setr_pd(2.0, nan);
    __m128d exp_ord = _mm_castsi128_pd(_mm_set_epi64x(0, 0xFFFFFFFFFFFFFFFF)); // 1.0和2.0有序
    passed += test_case("vcmpsd ORD (ordered)", a_ord, b_ord, CMP_ORD, exp_ord);
    total++;
    
    // 无穷大比较
    __m128d a_inf = _mm_setr_pd(inf, neg_inf);
    __m128d b_inf = _mm_setr_pd(100.0, -100.0);
    __m128d exp_inf = _mm_castsi128_pd(_mm_set_epi64x(0, 0)); // 都不相等
    passed += test_case("vcmpsd EQ with inf", a_inf, b_inf, CMP_EQ, exp_inf);
    total++;
    
    // 内存操作数测试
    ALIGNED(16) double mem_ops[2] = {1.0, 2.0}; // 16字节对齐
    __m128d a_mem = _mm_setr_pd(1.0, 2.0);
    __m128d exp_mem = _mm_castsi128_pd(_mm_set_epi64x(0, 0xFFFFFFFFFFFFFFFF)); // 相等
    
    __m128d res_mem = _mm_setzero_pd();
    asm volatile(
        "vcmpsd %[imm], (%[mem]), %[src], %[dst]"
        : [dst] "=x"(res_mem)
        : [src] "x"(a_mem), 
          [mem] "r"(mem_ops), 
          [imm] "i"(CMP_EQ)
    );
    
    printf("\nTest: vcmpsd with memory operand\n");
    print_double_vec_hex("Operand A", (double*)&a_mem, 2);
    print_double_vec_hex("Operand B (mem)", mem_ops, 2);
    print_double_vec_hex("Expected", (double*)&exp_mem, 2);
    print_double_vec_hex("Result  ", (double*)&res_mem, 2);
    
    uint64_t mem_res_low = *((uint64_t*)&res_mem);
    uint64_t mem_exp_low = *((uint64_t*)&exp_mem);
    int mem_result = (mem_res_low == mem_exp_low);
    printf("Result: %s\n", mem_result ? "PASS" : "FAIL");
    passed += mem_result;
    total++;
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
