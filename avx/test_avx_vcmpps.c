#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 测试谓词宏定义
#define CMP_EQ       0x00
#define CMP_LT       0x01
#define CMP_LE       0x02
#define CMP_UNORD    0x03
#define CMP_NEQ      0x04
#define CMP_NLT      0x05
#define CMP_NLE      0x06
#define CMP_ORD      0x07

// 内联汇编实现 vcmpps (使用switch处理立即数)
static void vcmpps(__m256 *dst, __m256 a, __m256 b, int imm) {
    switch(imm) {
        case CMP_EQ:
            asm volatile("vcmpps $0, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_LT:
            asm volatile("vcmpps $1, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_LE:
            asm volatile("vcmpps $2, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_UNORD:
            asm volatile("vcmpps $3, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NEQ:
            asm volatile("vcmpps $4, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NLT:
            asm volatile("vcmpps $5, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NLE:
            asm volatile("vcmpps $6, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_ORD:
            asm volatile("vcmpps $7, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        default:
            *dst = _mm256_setzero_ps();
            break;
    }
}

// 测试单个比较操作
static int test_case(const char *name, __m256 a, __m256 b, int imm, __m256 expected) {
    __m256 res = _mm256_setzero_ps();
    vcmpps(&res, a, b, imm);
    
    // 打印输入和输出
    printf("\nTest: %s\n", name);
    print_hex_float_vec("Operand A", (float*)&a, 8);
    print_hex_float_vec("Operand B", (float*)&b, 8);
    print_hex_float_vec("Expected", (float*)&expected, 8);
    print_hex_float_vec("Result  ", (float*)&res, 8);
    
    // 比较结果
    int ret = cmp_ymm(*(__m256i*)&res, *(__m256i*)&expected);
    printf("Result: %s\n", ret ? "PASS" : "FAIL");
    return ret;
}

int main() {
    int total = 0, passed = 0;
    
    // 定义测试值
    float nan = NAN;
    float inf = INFINITY;
    float neg_inf = -INFINITY;
    
    // 相等比较 (使用全1掩码表示true)
    __m256 a_eq = _mm256_setr_ps(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
    __m256 b_eq = _mm256_setr_ps(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
    __m256 exp_eq = _mm256_castsi256_ps(_mm256_set1_epi32(0xFFFFFFFF)); // 全1掩码
    passed += test_case("vcmpps EQ (equal)", a_eq, b_eq, CMP_EQ, exp_eq);
    total++;
    
    // 不等比较 (使用全1掩码表示true)
    __m256 a_neq = _mm256_setr_ps(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
    __m256 b_neq = _mm256_setr_ps(0.0f, 3.0f, 2.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    __m256 exp_neq = _mm256_castsi256_ps(_mm256_set1_epi32(0xFFFFFFFF)); // 全部不等
    passed += test_case("vcmpps NEQ (not equal)", a_neq, b_neq, CMP_NEQ, exp_neq);
    total++;
    
    // 小于比较 (使用全1掩码表示true)
    __m256 a_lt = _mm256_setr_ps(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
    __m256 b_lt = _mm256_setr_ps(2.0f, 1.0f, 4.0f, 3.0f, 6.0f, 5.0f, 8.0f, 7.0f);
    __m256 exp_lt = _mm256_castsi256_ps(
        _mm256_setr_epi32(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0)); 
    passed += test_case("vcmpps LT (less than)", a_lt, b_lt, CMP_LT, exp_lt);
    total++;
    
    // 无序比较 (NaN) (使用全1掩码表示true)
    __m256 a_unord = _mm256_setr_ps(1.0f, nan, 3.0f, nan, 5.0f, nan, 7.0f, nan);
    __m256 b_unord = _mm256_setr_ps(nan, 2.0f, nan, nan, nan, 6.0f, nan, nan);
    __m256 exp_unord = _mm256_castsi256_ps(_mm256_set1_epi32(0xFFFFFFFF)); // 全部无序
    passed += test_case("vcmpps UNORD (unordered)", a_unord, b_unord, CMP_UNORD, exp_unord);
    total++;
    
    // 有序比较 (NaN) (使用全1掩码表示true)
    __m256 a_ord = _mm256_setr_ps(1.0f, nan, 3.0f, 4.0f, nan, 6.0f, 7.0f, 8.0f);
    __m256 b_ord = _mm256_setr_ps(2.0f, 2.0f, nan, 4.0f, 5.0f, nan, nan, 8.0f);
    __m256 exp_ord = _mm256_castsi256_ps(
        _mm256_setr_epi32(0xFFFFFFFF, 0, 0, 0xFFFFFFFF, 0, 0, 0, 0xFFFFFFFF)); 
    passed += test_case("vcmpps ORD (ordered)", a_ord, b_ord, CMP_ORD, exp_ord);
    total++;
    
    // 无穷大比较 (使用全1掩码表示true)
    __m256 a_inf = _mm256_setr_ps(inf, neg_inf, inf, neg_inf, inf, neg_inf, inf, neg_inf);
    __m256 b_inf = _mm256_setr_ps(100.0f, -100.0f, inf, neg_inf, 200.0f, -200.0f, inf, neg_inf);
    __m256 exp_inf = _mm256_castsi256_ps(
        _mm256_setr_epi32(0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF)); 
    passed += test_case("vcmpps EQ with inf", a_inf, b_inf, CMP_EQ, exp_inf);
    total++;
    
    // 内存操作数测试 (使用正确的内存约束和全1掩码)
    ALIGNED(32) float mem_ops[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f}; // 32字节对齐
    __m256 a_mem = _mm256_setr_ps(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
    __m256 exp_mem = _mm256_castsi256_ps(_mm256_set1_epi32(0xFFFFFFFF));
    
    __m256 res_mem = _mm256_setzero_ps();
    asm volatile(
        "vcmpps %[imm], %[mem], %[src], %[dst]"
        : [dst] "=x"(res_mem)
        : [src] "x"(a_mem), 
          [mem] "m"(*((__m256*)mem_ops)), 
          [imm] "i"(CMP_EQ)
    );
    
    printf("\nTest: vcmpps with memory operand\n");
    print_hex_float_vec("Operand A", (float*)&a_mem, 8);
    print_hex_float_vec("Operand B (mem)", mem_ops, 8);
    print_hex_float_vec("Expected", (float*)&exp_mem, 8);
    print_hex_float_vec("Result  ", (float*)&res_mem, 8);
    
    int mem_result = cmp_ymm(*(__m256i*)&res_mem, *(__m256i*)&exp_mem);
    printf("Result: %s\n", mem_result ? "PASS" : "FAIL");
    passed += mem_result;
    total++;
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
