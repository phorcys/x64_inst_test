#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 测试谓词宏定义 (扩展更多比较类型)
#define CMP_EQ       0x00
#define CMP_LT       0x01
#define CMP_LE       0x02
#define CMP_UNORD    0x03
#define CMP_NEQ      0x04
#define CMP_NLT      0x05
#define CMP_NLE      0x06
#define CMP_ORD      0x07
#define CMP_EQ_UQ    0x08
#define CMP_NGE      0x09
#define CMP_NGT      0x0A
#define CMP_FALSE    0x0B
#define CMP_NEQ_OQ   0x0C
#define CMP_GE       0x0D
#define CMP_GT       0x0E
#define CMP_TRUE     0x0F
#define CMP_EQ_OS    0x10
#define CMP_LT_OQ    0x11
#define CMP_LE_OQ    0x12
#define CMP_UNORD_S  0x13
#define CMP_NEQ_US   0x14
#define CMP_NLT_UQ   0x15
#define CMP_NLE_UQ   0x16
#define CMP_ORD_S    0x17
#define CMP_EQ_US    0x18
#define CMP_NGE_UQ   0x19
#define CMP_NGT_UQ   0x1A
#define CMP_FALSE_OS 0x1B
#define CMP_NEQ_OS   0x1C
#define CMP_GE_OQ    0x1D
#define CMP_GT_OQ    0x1E
#define CMP_TRUE_US  0x1F

// 内联汇编实现 vcmppd (使用switch处理立即数)
static void vcmppd(__m256d *dst, __m256d a, __m256d b, int imm) {
    switch(imm) {
        case CMP_EQ:
            asm volatile("vcmppd $0, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_LT:
            asm volatile("vcmppd $1, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_LE:
            asm volatile("vcmppd $2, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_UNORD:
            asm volatile("vcmppd $3, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NEQ:
            asm volatile("vcmppd $4, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NLT:
            asm volatile("vcmppd $5, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NLE:
            asm volatile("vcmppd $6, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_ORD:
            asm volatile("vcmppd $7, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_GT:
            asm volatile("vcmppd $14, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_GE:
            asm volatile("vcmppd $13, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        case CMP_NGT:
            asm volatile("vcmppd $10, %2, %1, %0" : "=x"(*dst) : "x"(a), "x"(b), "0"(*dst));
            break;
        default:
            *dst = _mm256_setzero_pd();
            break;
    }
}

// 测试单个比较操作
static int test_case(const char *name, __m256d a, __m256d b, int imm, __m256d expected) {
    __m256d res = _mm256_setzero_pd();
    vcmppd(&res, a, b, imm);
    
    // 打印输入和输出
    printf("\nTest: %s\n", name);
    print_double_vec_hex("Operand A", (double*)&a, 4);
    print_double_vec_hex("Operand B", (double*)&b, 4);
    print_double_vec_hex("Expected", (double*)&expected, 4);
    print_double_vec_hex("Result  ", (double*)&res, 4);
    
    // 比较结果
    int ret = cmp_ymm(*(__m256i*)&res, *(__m256i*)&expected);
    printf("Result: %s\n", ret ? "PASS" : "FAIL");
    return ret;
}

int main() {
    int total = 0, passed = 0;
    
    // 定义测试值
    double nan = NAN;
    double inf = INFINITY;
    double neg_inf = -INFINITY;
    // 移除未使用的 normal 和 values 数组
    
    // 相等比较 (使用全1掩码表示true)
    __m256d a_eq = _mm256_setr_pd(1.0, 2.0, 3.0, 4.0);
    __m256d b_eq = _mm256_setr_pd(1.0, 2.0, 3.0, 4.0);
    __m256d exp_eq = _mm256_castsi256_pd(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)); // 全1掩码
    passed += test_case("vcmppd EQ (equal)", a_eq, b_eq, CMP_EQ, exp_eq);
    total++;
    
    // 不等比较 (使用全1掩码表示true)
    __m256d a_neq = _mm256_setr_pd(1.0, 2.0, 3.0, 4.0);
    __m256d b_neq = _mm256_setr_pd(0.0, 3.0, 2.0, 5.0);
    __m256d exp_neq = _mm256_castsi256_pd(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)); // 全部不等
    passed += test_case("vcmppd NEQ (not equal)", a_neq, b_neq, CMP_NEQ, exp_neq);
    total++;
    
    // 小于比较 (使用全1掩码表示true)
    __m256d a_lt = _mm256_setr_pd(1.0, 2.0, 3.0, 4.0);
    __m256d b_lt = _mm256_setr_pd(2.0, 1.0, 4.0, 3.0);
    __m256d exp_lt = _mm256_castsi256_pd(
        _mm256_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF, 0)); // [1<2, 2!<1, 3<4, 4!<3]
    passed += test_case("vcmppd LT (less than)", a_lt, b_lt, CMP_LT, exp_lt);
    total++;
    
    // 无序比较 (NaN) (使用全1掩码表示true)
    __m256d a_unord = _mm256_setr_pd(1.0, nan, 3.0, nan);
    __m256d b_unord = _mm256_setr_pd(nan, 2.0, nan, nan);
    __m256d exp_unord = _mm256_castsi256_pd(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF)); // 全部无序
    passed += test_case("vcmppd UNORD (unordered)", a_unord, b_unord, CMP_UNORD, exp_unord);
    total++;
    
    // 有序比较 (NaN) (使用全1掩码表示true)
    __m256d a_ord = _mm256_setr_pd(1.0, nan, 3.0, 4.0);
    __m256d b_ord = _mm256_setr_pd(2.0, 2.0, nan, 4.0);
    __m256d exp_ord = _mm256_castsi256_pd(
        _mm256_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0, 0, 0xFFFFFFFFFFFFFFFF)); // [1<2有序, nan无序, nan无序, 4=4有序]
    passed += test_case("vcmppd ORD (ordered)", a_ord, b_ord, CMP_ORD, exp_ord);
    total++;
    
    // 无穷大比较 (使用全1掩码表示true)
    __m256d a_inf = _mm256_setr_pd(inf, neg_inf, inf, neg_inf);
    __m256d b_inf = _mm256_setr_pd(100.0, -100.0, inf, neg_inf);
    __m256d exp_inf = _mm256_castsi256_pd(
        _mm256_setr_epi64x(0, 0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF)); // [inf!=100, -inf!=-100, inf==inf, -inf==-inf]
    passed += test_case("vcmppd EQ with inf", a_inf, b_inf, CMP_EQ, exp_inf);
    total++;
    
    // 内存操作数测试 (使用正确的内存约束和全1掩码)
    ALIGNED(32) double mem_ops[4] = {1.0, 2.0, 3.0, 4.0}; // 32字节对齐
    __m256d a_mem = _mm256_setr_pd(1.0, 2.0, 3.0, 4.0);
    __m256d exp_mem = _mm256_castsi256_pd(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF));
    
    __m256d res_mem = _mm256_setzero_pd();
    asm volatile(
        "vcmppd %[imm], %[mem], %[src], %[dst]"
        : [dst] "=x"(res_mem)
        : [src] "x"(a_mem), 
          [mem] "m"(*((__m256d*)mem_ops)), 
          [imm] "i"(CMP_EQ)
    );
    
    printf("\nTest: vcmppd with memory operand\n");
    print_double_vec_hex("Operand A", (double*)&a_mem, 4);
    print_double_vec_hex("Operand B (mem)", mem_ops, 4);
    print_double_vec_hex("Expected", (double*)&exp_mem, 4);
    print_double_vec_hex("Result  ", (double*)&res_mem, 4);
    
    int mem_result = cmp_ymm(*(__m256i*)&res_mem, *(__m256i*)&exp_mem);
    printf("Result: %s\n", mem_result ? "PASS" : "FAIL");
    passed += mem_result;
    total++;
    
    // 大于比较测试 (使用全1掩码表示true)
    __m256d a_gt = _mm256_setr_pd(2.0, 1.0, 4.0, 3.0);
    __m256d b_gt = _mm256_setr_pd(1.0, 2.0, 3.0, 4.0);
    __m256d exp_gt = _mm256_castsi256_pd(
        _mm256_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF, 0)); // [2>1, 1!>2, 4>3, 3!>4]
    passed += test_case("vcmppd GT (greater than)", a_gt, b_gt, CMP_GT, exp_gt);
    total++;

    // 大于等于比较测试 (使用全1掩码表示true)
    __m256d a_ge = _mm256_setr_pd(2.0, 1.0, 4.0, 4.0);
    __m256d b_ge = _mm256_setr_pd(1.0, 2.0, 4.0, 3.0);
    __m256d exp_ge = _mm256_castsi256_pd(
        _mm256_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF)); // [2>=1, 1!>=2, 4>=4, 4>=3]
    passed += test_case("vcmppd GE (greater or equal)", a_ge, b_ge, CMP_GE, exp_ge);
    total++;

    // 非大于比较测试 (使用全1掩码表示true)
    __m256d a_ngt = _mm256_setr_pd(1.0, 2.0, 3.0, 4.0);
    __m256d b_ngt = _mm256_setr_pd(2.0, 1.0, 4.0, 3.0);
    __m256d exp_ngt = _mm256_castsi256_pd(
        _mm256_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF, 0)); // [1!>2, 2>1, 3!>4, 4>3]
    passed += test_case("vcmppd NGT (not greater than)", a_ngt, b_ngt, CMP_NGT, exp_ngt);
    total++;

    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
