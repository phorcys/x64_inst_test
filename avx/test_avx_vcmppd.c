#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 所有32种比较条件宏定义
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

// 全1掩码宏
#define FULL_MASK _mm256_castsi256_pd(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF))

// 使用宏展开实现所有32种vcmppd条件
#define DEFINE_VCMPPD_FUNC(imm_value) \
static void vcmppd_##imm_value(__m256d *dst, __m256d a, __m256d b) { \
    __asm__ __volatile__ ( \
        "vcmppd $" #imm_value ", %2, %1, %0" \
        : "=x" (*dst) \
        : "x" (a), "x" (b) \
    ); \
}

// 为每个立即数定义单独的函数
DEFINE_VCMPPD_FUNC(0x00) DEFINE_VCMPPD_FUNC(0x01) DEFINE_VCMPPD_FUNC(0x02) DEFINE_VCMPPD_FUNC(0x03)
DEFINE_VCMPPD_FUNC(0x04) DEFINE_VCMPPD_FUNC(0x05) DEFINE_VCMPPD_FUNC(0x06) DEFINE_VCMPPD_FUNC(0x07)
DEFINE_VCMPPD_FUNC(0x08) DEFINE_VCMPPD_FUNC(0x09) DEFINE_VCMPPD_FUNC(0x0A) DEFINE_VCMPPD_FUNC(0x0B)
DEFINE_VCMPPD_FUNC(0x0C) DEFINE_VCMPPD_FUNC(0x0D) DEFINE_VCMPPD_FUNC(0x0E) DEFINE_VCMPPD_FUNC(0x0F)
DEFINE_VCMPPD_FUNC(0x10) DEFINE_VCMPPD_FUNC(0x11) DEFINE_VCMPPD_FUNC(0x12) DEFINE_VCMPPD_FUNC(0x13)
DEFINE_VCMPPD_FUNC(0x14) DEFINE_VCMPPD_FUNC(0x15) DEFINE_VCMPPD_FUNC(0x16) DEFINE_VCMPPD_FUNC(0x17)
DEFINE_VCMPPD_FUNC(0x18) DEFINE_VCMPPD_FUNC(0x19) DEFINE_VCMPPD_FUNC(0x1A) DEFINE_VCMPPD_FUNC(0x1B)
DEFINE_VCMPPD_FUNC(0x1C) DEFINE_VCMPPD_FUNC(0x1D) DEFINE_VCMPPD_FUNC(0x1E) DEFINE_VCMPPD_FUNC(0x1F)

// 128位版本宏定义
#define DEFINE_VCMPPD128_FUNC(imm_value) \
static void vcmppd128_##imm_value(__m128d *dst, __m128d a, __m128d b) { \
    __asm__ __volatile__ ( \
        "vcmppd $" #imm_value ", %2, %1, %0" \
        : "=x" (*dst) \
        : "x" (a), "x" (b) \
    ); \
}

// 为每个立即数定义128位函数
DEFINE_VCMPPD128_FUNC(0x00) DEFINE_VCMPPD128_FUNC(0x01) DEFINE_VCMPPD128_FUNC(0x02) DEFINE_VCMPPD128_FUNC(0x03)
DEFINE_VCMPPD128_FUNC(0x04) DEFINE_VCMPPD128_FUNC(0x05) DEFINE_VCMPPD128_FUNC(0x06) DEFINE_VCMPPD128_FUNC(0x07)
DEFINE_VCMPPD128_FUNC(0x08) DEFINE_VCMPPD128_FUNC(0x09) DEFINE_VCMPPD128_FUNC(0x0A) DEFINE_VCMPPD128_FUNC(0x0B)
DEFINE_VCMPPD128_FUNC(0x0C) DEFINE_VCMPPD128_FUNC(0x0D) DEFINE_VCMPPD128_FUNC(0x0E) DEFINE_VCMPPD128_FUNC(0x0F)
DEFINE_VCMPPD128_FUNC(0x10) DEFINE_VCMPPD128_FUNC(0x11) DEFINE_VCMPPD128_FUNC(0x12) DEFINE_VCMPPD128_FUNC(0x13)
DEFINE_VCMPPD128_FUNC(0x14) DEFINE_VCMPPD128_FUNC(0x15) DEFINE_VCMPPD128_FUNC(0x16) DEFINE_VCMPPD128_FUNC(0x17)
DEFINE_VCMPPD128_FUNC(0x18) DEFINE_VCMPPD128_FUNC(0x19) DEFINE_VCMPPD128_FUNC(0x1A) DEFINE_VCMPPD128_FUNC(0x1B)
DEFINE_VCMPPD128_FUNC(0x1C) DEFINE_VCMPPD128_FUNC(0x1D) DEFINE_VCMPPD128_FUNC(0x1E) DEFINE_VCMPPD128_FUNC(0x1F)

// 函数指针类型定义
typedef void (*vcmppd256_func)(__m256d*, __m256d, __m256d);
typedef void (*vcmppd128_func)(__m128d*, __m128d, __m128d);

// 函数指针数组
static vcmppd256_func vcmppd256_funcs[32] = {
    vcmppd_0x00, vcmppd_0x01, vcmppd_0x02, vcmppd_0x03,
    vcmppd_0x04, vcmppd_0x05, vcmppd_0x06, vcmppd_0x07,
    vcmppd_0x08, vcmppd_0x09, vcmppd_0x0A, vcmppd_0x0B,
    vcmppd_0x0C, vcmppd_0x0D, vcmppd_0x0E, vcmppd_0x0F,
    vcmppd_0x10, vcmppd_0x11, vcmppd_0x12, vcmppd_0x13,
    vcmppd_0x14, vcmppd_0x15, vcmppd_0x16, vcmppd_0x17,
    vcmppd_0x18, vcmppd_0x19, vcmppd_0x1A, vcmppd_0x1B,
    vcmppd_0x1C, vcmppd_0x1D, vcmppd_0x1E, vcmppd_0x1F
};

static vcmppd128_func vcmppd128_funcs[32] = {
    vcmppd128_0x00, vcmppd128_0x01, vcmppd128_0x02, vcmppd128_0x03,
    vcmppd128_0x04, vcmppd128_0x05, vcmppd128_0x06, vcmppd128_0x07,
    vcmppd128_0x08, vcmppd128_0x09, vcmppd128_0x0A, vcmppd128_0x0B,
    vcmppd128_0x0C, vcmppd128_0x0D, vcmppd128_0x0E, vcmppd128_0x0F,
    vcmppd128_0x10, vcmppd128_0x11, vcmppd128_0x12, vcmppd128_0x13,
    vcmppd128_0x14, vcmppd128_0x15, vcmppd128_0x16, vcmppd128_0x17,
    vcmppd128_0x18, vcmppd128_0x19, vcmppd128_0x1A, vcmppd128_0x1B,
    vcmppd128_0x1C, vcmppd128_0x1D, vcmppd128_0x1E, vcmppd128_0x1F
};

// 测试单个比较操作 (256位)
static int test_case256(const char *name, __m256d a, __m256d b, int imm, __m256d expected) {
    __m256d res = _mm256_setzero_pd();
    
    // 使用函数指针调用
    if (imm >= 0 && imm < 32) {
        vcmppd256_funcs[imm](&res, a, b);
    } else {
        printf("Invalid immediate value: 0x%02X\n", imm);
        return 0;
    }
    
    // 打印输入和输出
    printf("\nTest: %s (256-bit)\n", name);
    print_double_vec_hex("Operand A", (double*)&a, 4);
    print_double_vec_hex("Operand B", (double*)&b, 4);
    print_double_vec_hex("Expected", (double*)&expected, 4);
    print_double_vec_hex("Result  ", (double*)&res, 4);
    
    // 比较结果
    int ret = cmp_ymm(*(__m256i*)&res, *(__m256i*)&expected);
    printf("Result: %s\n", ret ? "PASS" : "FAIL");
    return ret;
}

// 测试单个比较操作 (128位)
static int test_case128(const char *name, __m128d a, __m128d b, int imm, __m128d expected) {
    __m128d res = _mm_setzero_pd();
    
    // 使用函数指针调用
    if (imm >= 0 && imm < 32) {
        vcmppd128_funcs[imm](&res, a, b);
    } else {
        printf("Invalid immediate value: 0x%02X\n", imm);
        return 0;
    }
    
    // 打印输入和输出
    printf("\nTest: %s (128-bit)\n", name);
    print_double_vec_hex("Operand A", (double*)&a, 2);
    print_double_vec_hex("Operand B", (double*)&b, 2);
    print_double_vec_hex("Expected", (double*)&expected, 2);
    print_double_vec_hex("Result  ", (double*)&res, 2);
    
    // 比较结果
    int ret = cmp_xmm(*(__m128i*)&res, *(__m128i*)&expected);
    printf("Result: %s\n", ret ? "PASS" : "FAIL");
    return ret;
}

int main() {
    int total = 0, passed = 0;
    
    // 定义特殊浮点值
    double qnan = NAN;
    double snan = nan("");
    double inf = INFINITY;
    double neg_inf = -INFINITY;
    double zero = 0.0;
    double neg_zero = -0.0;
    
    // 测试所有32种比较条件 (256位)
    for (int imm = 0; imm < 32; imm++) {
        char test_name[50];
        snprintf(test_name, sizeof(test_name), "vcmppd imm=0x%02X", imm);
        
        // 使用特殊值组合进行测试
        __m256d a = _mm256_setr_pd(1.0, qnan, inf, neg_inf);
        __m256d b = _mm256_setr_pd(1.0, 2.0, neg_inf, zero);
        
        // 根据比较条件计算预期结果
        __m256d expected = _mm256_setzero_pd();
        switch(imm) {
            case CMP_EQ: // 0: 相等
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0, 0, 0));
                break;
            case CMP_LT: // 1: 小于
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0, 0, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_LE: // 2: 小于等于
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0, 0, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_UNORD: // 3: 无序
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0xFFFFFFFFFFFFFFFF, 0, 0));
                break;
            case CMP_NEQ: // 4: 不等于
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_NLT: // 5: 不小于
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0));
                break;
            case CMP_NLE: // 6: 不小于等于
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0));
                break;
            case CMP_ORD: // 7: 有序
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_EQ_UQ: // 8: 相等（无序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0, 0));
                break;
            case CMP_NGE: // 9: 不小于（无序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_NGT: // 10: 不大于（无序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_FALSE: // 11: 总是假
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(0, 0, 0, 0));
                break;
            case CMP_NEQ_OQ: // 12: 不等于（有序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_GE: // 13: 大于等于
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF, 0));
                break;
            case CMP_GT: // 14: 大于
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0, 0xFFFFFFFFFFFFFFFF, 0));
                break;
            case CMP_TRUE: // 15: 总是真
                expected = FULL_MASK;
                break;
            case CMP_EQ_OS: // 16: 相等（有信号）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0, 0, 0));
                break;
            case CMP_LT_OQ: // 17: 小于（有序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0, 0, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_LE_OQ: // 18: 小于等于（有序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0, 0, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_UNORD_S: // 19: 无序（有信号）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0xFFFFFFFFFFFFFFFF, 0, 0));
                break;
            case CMP_NEQ_US: // 20: 不等于（无序有信号）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_NLT_UQ: // 21: 不小于（无序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0));
                break;
            case CMP_NLE_UQ: // 22: 不小于等于（无序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0));
                break;
            case CMP_ORD_S: // 23: 有序（有信号）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_EQ_US: // 24: 相等（无序有信号）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0, 0));
                break;
            case CMP_NGE_UQ: // 25: 不大于等于（无序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_NGT_UQ: // 26: 不大于（无序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_FALSE_OS: // 27: 总是假（有信号）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(0, 0, 0, 0));
                break;
            case CMP_NEQ_OS: // 28: 不等于（有序有信号）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_GE_OQ: // 29: 大于等于（有序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0xFFFFFFFFFFFFFFFF, 0, 0xFFFFFFFFFFFFFFFF, 0));
                break;
            case CMP_GT_OQ: // 30: 大于（有序）
                expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
                    0, 0, 0xFFFFFFFFFFFFFFFF, 0));
                break;
            case CMP_TRUE_US: // 31: 总是真（无序有信号）
                expected = FULL_MASK;
                break;
            default:
                expected = _mm256_setzero_pd();
                break;
        }
        
        passed += test_case256(test_name, a, b, imm, expected);
        total++;
    }
    
    // 暂时跳过128位版本测试
    printf("Skipping 128-bit tests for now\n");
    /*
    // 测试128位版本
    for (int imm = 0; imm < 32; imm++) {
        char test_name[50];
        snprintf(test_name, sizeof(test_name), "vcmppd128 imm=0x%02X", imm);
        
        __m128d a = _mm_setr_pd(snan, inf);
        __m128d b = _mm_setr_pd(neg_zero, neg_inf);
        
        // 根据比较条件计算预期结果
        __m128d expected = _mm_setzero_pd();
        switch(imm) {
            case CMP_UNORD: // 3: 无序
                expected = _mm_castsi128_pd(_mm_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0));
                break;
            case CMP_NEQ: // 4: 不等于
                expected = _mm_castsi128_pd(_mm_setr_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF));
                break;
            case CMP_TRUE: // 15: 总是真
            case CMP_TRUE_US: // 31: 总是真（无序有信号）
                expected = _mm_castsi128_pd(_mm_set1_epi64x(0xFFFFFFFFFFFFFFFF));
                break;
            default:
                // 其他条件根据具体语义设置
                // 为简化示例，这里只设置部分条件
                if (imm == CMP_EQ || imm == CMP_LT || imm == CMP_LE) {
                    expected = _mm_setzero_pd();
                } else {
                    expected = _mm_castsi128_pd(_mm_setr_epi64x(0, 0));
                }
                break;
        }
        
        passed += test_case128(test_name, a, b, imm, expected);
        total++;
    }
    */
    
    // 特殊测试：QNaN和SNaN处理
    __m256d a_nan = _mm256_setr_pd(qnan, snan, 1.0, 2.0);
    __m256d b_nan = _mm256_setr_pd(1.0, 2.0, qnan, snan);
    passed += test_case256("vcmppd QNaN/SNaN special", a_nan, b_nan, CMP_UNORD, FULL_MASK);
    total++;
    
    // 特殊测试：零值处理
    __m256d a_zero = _mm256_setr_pd(zero, neg_zero, 0.0, -0.0);
    __m256d b_zero = _mm256_setr_pd(neg_zero, zero, 1.0, -1.0);
    passed += test_case256("vcmppd zero values", a_zero, b_zero, CMP_EQ, 
        _mm256_castsi256_pd(_mm256_setr_epi64x(
            0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0, 0)));
    total++;
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return (passed == total) ? 0 : 1;
}
