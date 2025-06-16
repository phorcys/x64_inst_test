#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 比较谓词宏定义 (与vcmpss指令相同)
#define CMP_EQ       0x00
#define CMP_LT       0x01
#define CMP_LE       0x02
#define CMP_UNORD    0x03
#define CMP_NEQ      0x04
#define CMP_NLT      0x05
#define CMP_NLE      0x06
#define CMP_ORD      0x07

// 内联汇编实现 vcmpss
static void vcmpss(__m128 *dst, __m128 a, __m128 b, int imm) {
    switch(imm) {
        case CMP_EQ:
            asm volatile("vcmpss $0, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_LT:
            asm volatile("vcmpss $1, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_LE:
            asm volatile("vcmpss $2, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_UNORD:
            asm volatile("vcmpss $3, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NEQ:
            asm volatile("vcmpss $4, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NLT:
            asm volatile("vcmpss $5, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NLE:
            asm volatile("vcmpss $6, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_ORD:
            asm volatile("vcmpss $7, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        default:
            *dst = _mm_setzero_ps();
            break;
    }
}

// 测试单个比较操作
static int test_case(const char *name, __m128 a, __m128 b, int imm, __m128 expected) {
    __m128 res = _mm_setzero_ps();
    vcmpss(&res, a, b, imm);
    
    // 打印输入和输出
    printf("\nTest: %s\n", name);
    print_hex_float_vec("Operand A", (float*)&a, 4);
    print_hex_float_vec("Operand B", (float*)&b, 4);
    print_hex_float_vec("Expected", (float*)&expected, 4);
    print_hex_float_vec("Result  ", (float*)&res, 4);
    
    // 比较结果（只检查低32位）
    uint32_t res_low = *((uint32_t*)&res);
    uint32_t exp_low = *((uint32_t*)&expected);
    int ret = (res_low == exp_low);
    printf("Result: %s\n", ret ? "PASS" : "FAIL");
    return ret;
}

int main() {
    int total = 0, passed = 0;
    
    // 定义测试值
    float nan = NAN;
    float inf = INFINITY;
    float neg_inf = -INFINITY;
    
    // 相等比较
    __m128 a_eq = _mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f);
    __m128 b_eq = _mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f);
    __m128 exp_eq = _mm_castsi128_ps(_mm_set_epi32(0, 0, 0, 0xFFFFFFFF)); // 低32位全1
    passed += test_case("vcmpss EQ (equal)", a_eq, b_eq, CMP_EQ, exp_eq);
    total++;
    
    // 不等比较
    __m128 a_neq = _mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f);
    __m128 b_neq = _mm_setr_ps(0.0f, 3.0f, 2.0f, 5.0f);
    __m128 exp_neq = _mm_castsi128_ps(_mm_set_epi32(0, 0, 0, 0xFFFFFFFF)); // 全部不等
    passed += test_case("vcmpss NEQ (not equal)", a_neq, b_neq, CMP_NEQ, exp_neq);
    total++;
    
    // 小于比较
    __m128 a_lt = _mm_setr_ps(1.0f, 3.0f, 5.0f, 7.0f);
    __m128 b_lt = _mm_setr_ps(2.0f, 1.0f, 5.0f, 6.0f);
    __m128 exp_lt = _mm_castsi128_ps(_mm_set_epi32(0, 0, 0, 0xFFFFFFFF)); // 1.0 < 2.0 -> true
    passed += test_case("vcmpss LT (less than)", a_lt, b_lt, CMP_LT, exp_lt);
    total++;
    
    // 无序比较 (NaN)
    __m128 a_unord = _mm_setr_ps(1.0f, nan, 3.0f, 4.0f);
    __m128 b_unord = _mm_setr_ps(nan, 2.0f, nan, nan);
    __m128 exp_unord = _mm_castsi128_ps(_mm_set_epi32(0, 0, 0, 0xFFFFFFFF)); // 全部无序
    passed += test_case("vcmpss UNORD (unordered)", a_unord, b_unord, CMP_UNORD, exp_unord);
    total++;
    
    // 有序比较 (NaN)
    __m128 a_ord = _mm_setr_ps(1.0f, nan, 3.0f, 4.0f);
    __m128 b_ord = _mm_setr_ps(2.0f, nan, 3.0f, nan);
    __m128 exp_ord = _mm_castsi128_ps(_mm_set_epi32(0, 0, 0, 0xFFFFFFFF)); // 1.0和2.0有序
    passed += test_case("vcmpss ORD (ordered)", a_ord, b_ord, CMP_ORD, exp_ord);
    total++;
    
    // 无穷大比较
    __m128 a_inf = _mm_setr_ps(inf, neg_inf, 100.0f, -100.0f);
    __m128 b_inf = _mm_setr_ps(100.0f, -100.0f, inf, neg_inf);
    __m128 exp_inf = _mm_setzero_ps(); // 都不相等
    passed += test_case("vcmpss EQ with inf", a_inf, b_inf, CMP_EQ, exp_inf);
    total++;
    
    // 内存操作数测试
    ALIGNED(16) float mem_ops[4] = {1.0f, 2.0f, 3.0f, 4.0f}; // 16字节对齐
    __m128 a_mem = _mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f);
    __m128 exp_mem = _mm_castsi128_ps(_mm_set_epi32(0, 0, 0, 0xFFFFFFFF)); // 相等
    
    __m128 res_mem = _mm_setzero_ps();
    asm volatile(
        "vcmpss %[imm], (%[mem]), %[src], %[dst]"
        : [dst] "=x"(res_mem)
        : [src] "x"(a_mem), 
          [mem] "r"(mem_ops), 
          [imm] "i"(CMP_EQ)
    );
    
    printf("\nTest: vcmpss with memory operand\n");
    print_hex_float_vec("Operand A", (float*)&a_mem, 4);
    print_hex_float_vec("Operand B (mem)", mem_ops, 4);
    print_hex_float_vec("Expected", (float*)&exp_mem, 4);
    print_hex_float_vec("Result  ", (float*)&res_mem, 4);
    
    uint32_t mem_res_low = *((uint32_t*)&res_mem);
    uint32_t mem_exp_low = *((uint32_t*)&exp_mem);
    int mem_result = (mem_res_low == mem_exp_low);
    printf("Result: %s\n", mem_result ? "PASS" : "FAIL");
    passed += mem_result;
    total++;
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
