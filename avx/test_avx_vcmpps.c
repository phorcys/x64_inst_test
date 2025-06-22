#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 使用宏定义实现所有32种比较条件
#define DEFINE_CMP_FUNC(imm) \
static void vcmpps_##imm(__m256 *dst, __m256 a, __m256 b) { \
    asm volatile("vcmpps $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "x"(b)); \
}

// 为所有32种条件定义函数
DEFINE_CMP_FUNC(0x00) DEFINE_CMP_FUNC(0x01) DEFINE_CMP_FUNC(0x02) DEFINE_CMP_FUNC(0x03)
DEFINE_CMP_FUNC(0x04) DEFINE_CMP_FUNC(0x05) DEFINE_CMP_FUNC(0x06) DEFINE_CMP_FUNC(0x07)
DEFINE_CMP_FUNC(0x08) DEFINE_CMP_FUNC(0x09) DEFINE_CMP_FUNC(0x0A) DEFINE_CMP_FUNC(0x0B)
DEFINE_CMP_FUNC(0x0C) DEFINE_CMP_FUNC(0x0D) DEFINE_CMP_FUNC(0x0E) DEFINE_CMP_FUNC(0x0F)
DEFINE_CMP_FUNC(0x10) DEFINE_CMP_FUNC(0x11) DEFINE_CMP_FUNC(0x12) DEFINE_CMP_FUNC(0x13)
DEFINE_CMP_FUNC(0x14) DEFINE_CMP_FUNC(0x15) DEFINE_CMP_FUNC(0x16) DEFINE_CMP_FUNC(0x17)
DEFINE_CMP_FUNC(0x18) DEFINE_CMP_FUNC(0x19) DEFINE_CMP_FUNC(0x1A) DEFINE_CMP_FUNC(0x1B)
DEFINE_CMP_FUNC(0x1C) DEFINE_CMP_FUNC(0x1D) DEFINE_CMP_FUNC(0x1E) DEFINE_CMP_FUNC(0x1F)

// 函数指针类型
typedef void (*vcmpps_func_t)(__m256*, __m256, __m256);

// 函数指针数组
static vcmpps_func_t vcmpps_funcs[32] = {
    vcmpps_0x00, vcmpps_0x01, vcmpps_0x02, vcmpps_0x03,
    vcmpps_0x04, vcmpps_0x05, vcmpps_0x06, vcmpps_0x07,
    vcmpps_0x08, vcmpps_0x09, vcmpps_0x0A, vcmpps_0x0B,
    vcmpps_0x0C, vcmpps_0x0D, vcmpps_0x0E, vcmpps_0x0F,
    vcmpps_0x10, vcmpps_0x11, vcmpps_0x12, vcmpps_0x13,
    vcmpps_0x14, vcmpps_0x15, vcmpps_0x16, vcmpps_0x17,
    vcmpps_0x18, vcmpps_0x19, vcmpps_0x1A, vcmpps_0x1B,
    vcmpps_0x1C, vcmpps_0x1D, vcmpps_0x1E, vcmpps_0x1F
};

// 统一的调用函数
static void vcmpps(__m256 *dst, __m256 a, __m256 b, int imm) {
    if (imm >= 0 && imm < 32) {
        vcmpps_funcs[imm](dst, a, b);
    } else {
        *dst = _mm256_setzero_ps();
    }
}

// 测试单个比较操作
static int test_case(const char *name, __m256 a, __m256 b, int imm, __m256 expected) {
    __m256 res = _mm256_setzero_ps();
    vcmpps(&res, a, b, imm);
    
    // 打印输入和输出
    printf("\nTest: %s (imm=0x%02X)\n", name, imm);
    print_hex_float_vec("Operand A", (float*)&a, 8);
    print_hex_float_vec("Operand B", (float*)&b, 8);
    print_hex_float_vec("Expected", (float*)&expected, 8);
    print_hex_float_vec("Result  ", (float*)&res, 8);
    
    // 比较掩码位模式
    __m256i res_i = _mm256_castps_si256(res);
    __m256i expected_i = _mm256_castps_si256(expected);
    int ret = _mm256_testc_si256(res_i, expected_i) && _mm256_testc_si256(expected_i, res_i);
    printf("Result: %s\n", ret ? "PASS" : "FAIL");
    return ret;
}

int main() {
    int total = 0, passed = 0;
    
    // 定义测试值
    float nan = NAN;
    float inf = INFINITY;
    float neg_inf = -INFINITY;
    float zero = 0.0f;
    float neg_zero = -0.0f;
    
    // 基础测试向量
    __m256 a = _mm256_setr_ps(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
    __m256 b = _mm256_setr_ps(1.0f, 3.0f, 2.0f, 5.0f, 5.0f, 4.0f, 8.0f, 7.0f);
    
    // 特殊值测试向量
    __m256 special_a = _mm256_setr_ps(zero, neg_zero, inf, neg_inf, nan, 1.0f, 2.0f, 3.0f);
    __m256 special_b = _mm256_setr_ps(neg_zero, zero, neg_inf, inf, nan, 4.0f, 5.0f, 6.0f);
    
    // 全1掩码和全0掩码 (直接使用整数掩码)
    __m256 all_true = _mm256_castsi256_ps(_mm256_set1_epi32(0xFFFFFFFF));
    __m256 all_false = _mm256_castsi256_ps(_mm256_set1_epi32(0x0));
    
    // 测试特殊值: NaN、无穷大、零值
    // 特殊值测试：0.0和-0.0比较应为真，NaN比较应为假
    passed += test_case("vcmpps QNaN/SNaN special", 
                       special_a, special_b, 0x00,
                       _mm256_castsi256_ps(_mm256_setr_epi32(
                           0xFFFFFFFF,  // 0.0 == -0.0 -> true
                           0xFFFFFFFF,  // -0.0 == 0.0 -> true
                           0x00000000,  // inf != neg_inf -> false
                           0x00000000,  // neg_inf != inf -> false
                           0x00000000,  // NaN != NaN -> false
                           0x00000000,  // 1.0 != 4.0 -> false
                           0x00000000,  // 2.0 != 5.0 -> false
                           0x00000000   // 3.0 != 6.0 -> false
                       )));
    total++;
    
    passed += test_case("vcmpps zero values", 
                       _mm256_setr_ps(zero, neg_zero, zero, neg_zero, 1.0f, 2.0f, 3.0f, 4.0f),
                       _mm256_setr_ps(neg_zero, zero, 1.0f, -1.0f, zero, neg_zero, 3.0f, 4.0f),
                       0x00,  // EQ
                       _mm256_setr_ps(
                           _mm256_castsi256_ps(_mm256_set1_epi32(0xFFFFFFFF))[0],  // +0.0 == -0.0
                           _mm256_castsi256_ps(_mm256_set1_epi32(0xFFFFFFFF))[0],  // -0.0 == +0.0
                           _mm256_castsi256_ps(_mm256_set1_epi32(0x0))[0],         // 0.0 != 1.0
                           _mm256_castsi256_ps(_mm256_set1_epi32(0x0))[0],         // -0.0 != -1.0
                           _mm256_castsi256_ps(_mm256_set1_epi32(0x0))[0],         // 1.0 != 0.0
                           _mm256_castsi256_ps(_mm256_set1_epi32(0x0))[0],         // 2.0 != -0.0
                           _mm256_castsi256_ps(_mm256_set1_epi32(0xFFFFFFFF))[0],  // 3.0 == 3.0
                           _mm256_castsi256_ps(_mm256_set1_epi32(0xFFFFFFFF))[0]   // 4.0 == 4.0
                       ));
    total++;
    
    // 测试所有32种比较条件
        for (int imm = 0; imm < 32; imm++) {
            char name[64];
            snprintf(name, sizeof(name), "vcmpps imm=0x%02X", imm);
            
            __m256 expected = _mm256_setzero_ps();
            float* exp = (float*)&expected;
        
        // 根据比较条件设置预期结果
        int32_t exp_int[8];
        for (int i = 0; i < 8; i++) {
            float ai = a[i];
            float bi = b[i];
            int cmp_result = 0;
            
            switch(imm) {
                case 0x00: // EQ (equal)
                    cmp_result = (ai == bi);
                    break;
                case 0x01: // LT (less than)
                    cmp_result = (ai < bi);
                    break;
                case 0x02: // LE (less than or equal)
                    cmp_result = (ai <= bi);
                    break;
                case 0x03: // UNORD (unordered)
                    cmp_result = (isnan(ai) || isnan(bi));
                    break;
                case 0x04: // NEQ (not equal)
                    cmp_result = (ai != bi);
                    break;
                case 0x05: // NLT (not less than)
                    cmp_result = !(ai < bi);
                    break;
                case 0x06: // NLE (not less than or equal)
                    cmp_result = !(ai <= bi);
                    break;
                case 0x07: // ORD (ordered)
                    cmp_result = !(isnan(ai) || isnan(bi));
                    break;
                case 0x08: // EQ_UQ (equal, unordered or equal)
                    cmp_result = (ai == bi) || (isnan(ai) && isnan(bi));
                    break;
                case 0x09: // NGE (not greater than or equal)
                    cmp_result = !(ai >= bi);
                    break;
                case 0x0A: // NGT (not greater than)
                    cmp_result = !(ai > bi);
                    break;
                case 0x0B: // FALSE (always false)
                    cmp_result = 0;
                    break;
                case 0x0C: // NEQ_OQ (not equal, ordered or equal)
                    cmp_result = (ai != bi) && !(isnan(ai) || isnan(bi));
                    break;
                case 0x0D: // GE (greater than or equal)
                    cmp_result = (ai >= bi);
                    break;
                case 0x0E: // GT (greater than)
                    cmp_result = (ai > bi);
                    break;
                case 0x0F: // TRUE (always true)
                    cmp_result = 1;
                    break;
                case 0x10: // EQ_OS (equal, ordered signaling)
                    cmp_result = (ai == bi) && !(isnan(ai) || isnan(bi));
                    break;
                case 0x11: // LT_OS (less than, ordered signaling)
                    cmp_result = (ai < bi) && !(isnan(ai) || isnan(bi));
                    break;
                case 0x12: // LE_OS (less than or equal, ordered signaling)
                    cmp_result = (ai <= bi) && !(isnan(ai) || isnan(bi));
                    break;
                case 0x13: // UNORD_S (unordered, signaling)
                    cmp_result = (isnan(ai) || isnan(bi));
                    break;
                case 0x14: // NEQ_US (not equal, unordered or signaling)
                    cmp_result = (ai != bi) || (isnan(ai) || isnan(bi));
                    break;
                case 0x15: // NLT_US (not less than, unordered or signaling)
                    cmp_result = !(ai < bi) || (isnan(ai) || isnan(bi));
                    break;
                case 0x16: // NLE_US (not less than or equal, unordered or signaling)
                    cmp_result = !(ai <= bi) || (isnan(ai) || isnan(bi));
                    break;
                case 0x17: // ORD_S (ordered, signaling)
                    cmp_result = !(isnan(ai) || isnan(bi));
                    break;
                case 0x18: // EQ_US (equal, unordered or signaling)
                    cmp_result = (ai == bi) || (isnan(ai) || isnan(bi));
                    break;
                case 0x19: // NGE_US (not greater than or equal, unordered or signaling)
                    cmp_result = !(ai >= bi) || (isnan(ai) || isnan(bi));
                    break;
                case 0x1A: // NGT_US (not greater than, unordered or signaling)
                    cmp_result = !(ai > bi) || (isnan(ai) || isnan(bi));
                    break;
                case 0x1B: // FALSE_OS (always false, ordered signaling)
                    cmp_result = 0;
                    break;
                case 0x1C: // NEQ_OS (not equal, ordered or signaling)
                    cmp_result = (ai != bi) && !(isnan(ai) || isnan(bi));
                    break;
                case 0x1D: // GE_OS (greater than or equal, ordered signaling)
                    cmp_result = (ai >= bi) && !(isnan(ai) || isnan(bi));
                    break;
                case 0x1E: // GT_OS (greater than, ordered signaling)
                    cmp_result = (ai > bi) && !(isnan(ai) || isnan(bi));
                    break;
                case 0x1F: // TRUE_US (always true, unordered or signaling)
                    cmp_result = 1;
                    break;
            }
            
            exp_int[i] = cmp_result ? 0xFFFFFFFF : 0x00000000;
        }
        __m256i expected_i = _mm256_loadu_si256((__m256i*)exp_int);
        expected = _mm256_castsi256_ps(expected_i);
        
        passed += test_case(name, a, b, imm, expected);
        total++;
    }
    
    // 特殊值测试：NaN、无穷大、零值
    passed += test_case("vcmpps QNaN/SNaN special", 
                       special_a, special_b, 0x00,
                       _mm256_castsi256_ps(_mm256_setr_epi32(
                           0xFFFFFFFF,  // 0.0 == -0.0 -> true
                           0xFFFFFFFF,  // -0.0 == 0.0 -> true
                           0x00000000,  // inf != neg_inf -> false
                           0x00000000,  // neg_inf != inf -> false
                           0x00000000,  // NaN != NaN -> false
                           0x00000000,  // 1.0 != 4.0 -> false
                           0x00000000,  // 2.0 != 5.0 -> false
                           0x00000000   // 3.0 != 6.0 -> false
                       )));
    total++;
    
    passed += test_case("vcmpps zero values", 
                       _mm256_setr_ps(zero, neg_zero, zero, neg_zero, 1.0f, 2.0f, 3.0f, 4.0f),
                       _mm256_setr_ps(neg_zero, zero, 1.0f, -1.0f, zero, neg_zero, 3.0f, 4.0f),
                       0x00,  // EQ
                       _mm256_castsi256_ps(_mm256_setr_epi32(
                           0xFFFFFFFF,  // +0.0 == -0.0 -> true
                           0xFFFFFFFF,  // -0.0 == +0.0 -> true
                           0x00000000,  // 0.0 != 1.0 -> false
                           0x00000000,  // -0.0 != -1.0 -> false
                           0x00000000,  // 1.0 != 0.0 -> false
                           0x00000000,  // 2.0 != -0.0 -> false
                           0xFFFFFFFF,  // 3.0 == 3.0 -> true
                           0xFFFFFFFF   // 4.0 == 4.0 -> true
                       )));
    total++;
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
