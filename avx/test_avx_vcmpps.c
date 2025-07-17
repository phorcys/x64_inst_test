#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// ========== 添加128位支持 ==========
// 128位比较函数宏
#define DEFINE_CMP128_FUNC(imm) \
static void vcmpps128_##imm(__m128 *dst, __m128 a, __m128 b) { \
    asm volatile("vcmpps $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "x"(b)); \
}

// 256位比较函数宏
#define DEFINE_CMP256_FUNC(imm) \
static void vcmpps256_##imm(__m256 *dst, __m256 a, __m256 b) { \
    asm volatile("vcmpps $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "x"(b)); \
}

// 为所有32种条件定义128位函数
DEFINE_CMP128_FUNC(0x00) DEFINE_CMP128_FUNC(0x01) DEFINE_CMP128_FUNC(0x02) DEFINE_CMP128_FUNC(0x03)
DEFINE_CMP128_FUNC(0x04) DEFINE_CMP128_FUNC(0x05) DEFINE_CMP128_FUNC(0x06) DEFINE_CMP128_FUNC(0x07)
DEFINE_CMP128_FUNC(0x08) DEFINE_CMP128_FUNC(0x09) DEFINE_CMP128_FUNC(0x0A) DEFINE_CMP128_FUNC(0x0B)
DEFINE_CMP128_FUNC(0x0C) DEFINE_CMP128_FUNC(0x0D) DEFINE_CMP128_FUNC(0x0E) DEFINE_CMP128_FUNC(0x0F)
DEFINE_CMP128_FUNC(0x10) DEFINE_CMP128_FUNC(0x11) DEFINE_CMP128_FUNC(0x12) DEFINE_CMP128_FUNC(0x13)
DEFINE_CMP128_FUNC(0x14) DEFINE_CMP128_FUNC(0x15) DEFINE_CMP128_FUNC(0x16) DEFINE_CMP128_FUNC(0x17)
DEFINE_CMP128_FUNC(0x18) DEFINE_CMP128_FUNC(0x19) DEFINE_CMP128_FUNC(0x1A) DEFINE_CMP128_FUNC(0x1B)
DEFINE_CMP128_FUNC(0x1C) DEFINE_CMP128_FUNC(0x1D) DEFINE_CMP128_FUNC(0x1E) DEFINE_CMP128_FUNC(0x1F)

// 为所有32种条件定义256位函数
DEFINE_CMP256_FUNC(0x00) DEFINE_CMP256_FUNC(0x01) DEFINE_CMP256_FUNC(0x02) DEFINE_CMP256_FUNC(0x03)
DEFINE_CMP256_FUNC(0x04) DEFINE_CMP256_FUNC(0x05) DEFINE_CMP256_FUNC(0x06) DEFINE_CMP256_FUNC(0x07)
DEFINE_CMP256_FUNC(0x08) DEFINE_CMP256_FUNC(0x09) DEFINE_CMP256_FUNC(0x0A) DEFINE_CMP256_FUNC(0x0B)
DEFINE_CMP256_FUNC(0x0C) DEFINE_CMP256_FUNC(0x0D) DEFINE_CMP256_FUNC(0x0E) DEFINE_CMP256_FUNC(0x0F)
DEFINE_CMP256_FUNC(0x10) DEFINE_CMP256_FUNC(0x11) DEFINE_CMP256_FUNC(0x12) DEFINE_CMP256_FUNC(0x13)
DEFINE_CMP256_FUNC(0x14) DEFINE_CMP256_FUNC(0x15) DEFINE_CMP256_FUNC(0x16) DEFINE_CMP256_FUNC(0x17)
DEFINE_CMP256_FUNC(0x18) DEFINE_CMP256_FUNC(0x19) DEFINE_CMP256_FUNC(0x1A) DEFINE_CMP256_FUNC(0x1B)
DEFINE_CMP256_FUNC(0x1C) DEFINE_CMP256_FUNC(0x1D) DEFINE_CMP256_FUNC(0x1E) DEFINE_CMP256_FUNC(0x1F)

// 128位函数指针类型和数组
typedef void (*vcmpps128_func_t)(__m128*, __m128, __m128);
static vcmpps128_func_t vcmpps128_funcs[32] = {
    vcmpps128_0x00, vcmpps128_0x01, vcmpps128_0x02, vcmpps128_0x03,
    vcmpps128_0x04, vcmpps128_0x05, vcmpps128_0x06, vcmpps128_0x07,
    vcmpps128_0x08, vcmpps128_0x09, vcmpps128_0x0A, vcmpps128_0x0B,
    vcmpps128_0x0C, vcmpps128_0x0D, vcmpps128_0x0E, vcmpps128_0x0F,
    vcmpps128_0x10, vcmpps128_0x11, vcmpps128_0x12, vcmpps128_0x13,
    vcmpps128_0x14, vcmpps128_0x15, vcmpps128_0x16, vcmpps128_0x17,
    vcmpps128_0x18, vcmpps128_0x19, vcmpps128_0x1A, vcmpps128_0x1B,
    vcmpps128_0x1C, vcmpps128_0x1D, vcmpps128_0x1E, vcmpps128_0x1F
};

// 256位函数指针类型和数组
typedef void (*vcmpps256_func_t)(__m256*, __m256, __m256);
static vcmpps256_func_t vcmpps256_funcs[32] = {
    vcmpps256_0x00, vcmpps256_0x01, vcmpps256_0x02, vcmpps256_0x03,
    vcmpps256_0x04, vcmpps256_0x05, vcmpps256_0x06, vcmpps256_0x07,
    vcmpps256_0x08, vcmpps256_0x09, vcmpps256_0x0A, vcmpps256_0x0B,
    vcmpps256_0x0C, vcmpps256_0x0D, vcmpps256_0x0E, vcmpps256_0x0F,
    vcmpps256_0x10, vcmpps256_0x11, vcmpps256_0x12, vcmpps256_0x13,
    vcmpps256_0x14, vcmpps256_0x15, vcmpps256_0x16, vcmpps256_0x17,
    vcmpps256_0x18, vcmpps256_0x19, vcmpps256_0x1A, vcmpps256_0x1B,
    vcmpps256_0x1C, vcmpps256_0x1D, vcmpps256_0x1E, vcmpps256_0x1F
};

// 统一的调用函数（256位）
static void vcmpps256(__m256 *dst, __m256 a, __m256 b, int imm) {
    if (imm >= 0 && imm < 32) {
        vcmpps256_funcs[imm](dst, a, b);
    } else {
        *dst = _mm256_setzero_ps();
    }
}

// 统一的调用函数（128位）

// ========== 添加内存操作数支持 ==========
// 带内存操作数的256位比较宏（修复操作数）
#define DEFINE_CMP256_MEM_FUNC(imm) \
static void vcmpps256_mem_##imm(__m256 *dst, __m256 a, float *mem) { \
    asm volatile("vcmpps $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "m"(*mem)); \
}

// 带内存操作数的128位比较宏（修复操作数）
#define DEFINE_CMP128_MEM_FUNC(imm) \
static void vcmpps128_mem_##imm(__m128 *dst, __m128 a, float *mem) { \
    asm volatile("vcmpps $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "m"(*mem)); \
}

// 为所有32种条件定义256位内存操作数函数
DEFINE_CMP256_MEM_FUNC(0x00) DEFINE_CMP256_MEM_FUNC(0x01) DEFINE_CMP256_MEM_FUNC(0x02) DEFINE_CMP256_MEM_FUNC(0x03)
DEFINE_CMP256_MEM_FUNC(0x04) DEFINE_CMP256_MEM_FUNC(0x05) DEFINE_CMP256_MEM_FUNC(0x06) DEFINE_CMP256_MEM_FUNC(0x07)
DEFINE_CMP256_MEM_FUNC(0x08) DEFINE_CMP256_MEM_FUNC(0x09) DEFINE_CMP256_MEM_FUNC(0x0A) DEFINE_CMP256_MEM_FUNC(0x0B)
DEFINE_CMP256_MEM_FUNC(0x0C) DEFINE_CMP256_MEM_FUNC(0x0D) DEFINE_CMP256_MEM_FUNC(0x0E) DEFINE_CMP256_MEM_FUNC(0x0F)
DEFINE_CMP256_MEM_FUNC(0x10) DEFINE_CMP256_MEM_FUNC(0x11) DEFINE_CMP256_MEM_FUNC(0x12) DEFINE_CMP256_MEM_FUNC(0x13)
DEFINE_CMP256_MEM_FUNC(0x14) DEFINE_CMP256_MEM_FUNC(0x15) DEFINE_CMP256_MEM_FUNC(0x16) DEFINE_CMP256_MEM_FUNC(0x17)
DEFINE_CMP256_MEM_FUNC(0x18) DEFINE_CMP256_MEM_FUNC(0x19) DEFINE_CMP256_MEM_FUNC(0x1A) DEFINE_CMP256_MEM_FUNC(0x1B)
DEFINE_CMP256_MEM_FUNC(0x1C) DEFINE_CMP256_MEM_FUNC(0x1D) DEFINE_CMP256_MEM_FUNC(0x1E) DEFINE_CMP256_MEM_FUNC(0x1F)

// 为所有32种条件定义128位内存操作数函数
DEFINE_CMP128_MEM_FUNC(0x00) DEFINE_CMP128_MEM_FUNC(0x01) DEFINE_CMP128_MEM_FUNC(0x02) DEFINE_CMP128_MEM_FUNC(0x03)
DEFINE_CMP128_MEM_FUNC(0x04) DEFINE_CMP128_MEM_FUNC(0x05) DEFINE_CMP128_MEM_FUNC(0x06) DEFINE_CMP128_MEM_FUNC(0x07)
DEFINE_CMP128_MEM_FUNC(0x08) DEFINE_CMP128_MEM_FUNC(0x09) DEFINE_CMP128_MEM_FUNC(0x0A) DEFINE_CMP128_MEM_FUNC(0x0B)
DEFINE_CMP128_MEM_FUNC(0x0C) DEFINE_CMP128_MEM_FUNC(0x0D) DEFINE_CMP128_MEM_FUNC(0x0E) DEFINE_CMP128_MEM_FUNC(0x0F)
DEFINE_CMP128_MEM_FUNC(0x10) DEFINE_CMP128_MEM_FUNC(0x11) DEFINE_CMP128_MEM_FUNC(0x12) DEFINE_CMP128_MEM_FUNC(0x13)
DEFINE_CMP128_MEM_FUNC(0x14) DEFINE_CMP128_MEM_FUNC(0x15) DEFINE_CMP128_MEM_FUNC(0x16) DEFINE_CMP128_MEM_FUNC(0x17)
DEFINE_CMP128_MEM_FUNC(0x18) DEFINE_CMP128_MEM_FUNC(0x19) DEFINE_CMP128_MEM_FUNC(0x1A) DEFINE_CMP128_MEM_FUNC(0x1B)
DEFINE_CMP128_MEM_FUNC(0x1C) DEFINE_CMP128_MEM_FUNC(0x1D) DEFINE_CMP128_MEM_FUNC(0x1E) DEFINE_CMP128_MEM_FUNC(0x1F)

// 256位内存操作数函数指针类型和数组
typedef void (*vcmpps256_mem_func_t)(__m256*, __m256, float*);
static vcmpps256_mem_func_t vcmpps256_mem_funcs[32] = {
    vcmpps256_mem_0x00, vcmpps256_mem_0x01, vcmpps256_mem_0x02, vcmpps256_mem_0x03,
    vcmpps256_mem_0x04, vcmpps256_mem_0x05, vcmpps256_mem_0x06, vcmpps256_mem_0x07,
    vcmpps256_mem_0x08, vcmpps256_mem_0x09, vcmpps256_mem_0x0A, vcmpps256_mem_0x0B,
    vcmpps256_mem_0x0C, vcmpps256_mem_0x0D, vcmpps256_mem_0x0E, vcmpps256_mem_0x0F,
    vcmpps256_mem_0x10, vcmpps256_mem_0x11, vcmpps256_mem_0x12, vcmpps256_mem_0x13,
    vcmpps256_mem_0x14, vcmpps256_mem_0x15, vcmpps256_mem_0x16, vcmpps256_mem_0x17,
    vcmpps256_mem_0x18, vcmpps256_mem_0x19, vcmpps256_mem_0x1A, vcmpps256_mem_0x1B,
    vcmpps256_mem_0x1C, vcmpps256_mem_0x1D, vcmpps256_mem_0x1E, vcmpps256_mem_0x1F
};

// 128位内存操作数函数指针类型和数组
typedef void (*vcmpps128_mem_func_t)(__m128*, __m128, float*);
static vcmpps128_mem_func_t vcmpps128_mem_funcs[32] = {
    vcmpps128_mem_0x00, vcmpps128_mem_0x01, vcmpps128_mem_0x02, vcmpps128_mem_0x03,
    vcmpps128_mem_0x04, vcmpps128_mem_0x05, vcmpps128_mem_0x06, vcmpps128_mem_0x07,
    vcmpps128_mem_0x08, vcmpps128_mem_0x09, vcmpps128_mem_0x0A, vcmpps128_mem_0x0B,
    vcmpps128_mem_0x0C, vcmpps128_mem_0x0D, vcmpps128_mem_0x0E, vcmpps128_mem_0x0F,
    vcmpps128_mem_0x10, vcmpps128_mem_0x11, vcmpps128_mem_0x12, vcmpps128_mem_0x13,
    vcmpps128_mem_0x14, vcmpps128_mem_0x15, vcmpps128_mem_0x16, vcmpps128_mem_0x17,
    vcmpps128_mem_0x18, vcmpps128_mem_0x19, vcmpps128_mem_0x1A, vcmpps128_mem_0x1B,
    vcmpps128_mem_0x1C, vcmpps128_mem_0x1D, vcmpps128_mem_0x1E, vcmpps128_mem_0x1F
};

// 测试用例类型枚举
typedef enum {
    TEST_REG,       // 寄存器操作数
    TEST_MEM        // 内存操作数
} test_type_t;

// 增强的测试函数
static int test_case(const char *name, void *a, void *b, int imm, void *expected, 
                     int width, test_type_t test_type) {
    if (width == 1) { // 256位测试
        __m256 res = _mm256_setzero_ps();
        __m256 a_reg = _mm256_loadu_ps((float*)a);
        __m256 b_reg;
        __m256 expected_reg = _mm256_loadu_ps((float*)expected);
        
        if (test_type == TEST_REG) {
            vcmpps256_funcs[imm](&res, a_reg, b_reg);
        } else { // TEST_MEM
            vcmpps256_mem_funcs[imm](&res, a_reg, (float*)b);
        }
        
        // 比较结果
        __m256i res_i = _mm256_castps_si256(res);
        __m256i expected_i = _mm256_castps_si256(expected_reg);
        int ret = _mm256_testc_si256(res_i, expected_i) && 
                  _mm256_testc_si256(expected_i, res_i);
        
        if (!ret) {
            printf("❌ FAIL: %s\n", name);
            printf("  A: ");
            for (int k = 0; k < (width ? 8 : 4); k++) printf("%f ", ((float*)a)[k]);
            printf("\n  B: ");
            for (int k = 0; k < (width ? 8 : 4); k++) printf("%f ", test_type == TEST_REG ? ((float*)b)[k] : ((float*)b)[0]);
            printf("\n  Expected: ");
            for (int k = 0; k < (width ? 8 : 4); k++) printf("0x%08X ", ((uint32_t*)expected)[k]);
            printf("\n  Got:      ");
            for (int k = 0; k < (width ? 8 : 4); k++) printf("0x%08X ", ((uint32_t*)&res)[k]);
            printf("\n");
        } else {
            printf("✅ PASS: %s\n", name);
        }
        return ret;
    } 
    else { // 128位测试
        __m128 res = _mm_setzero_ps();
        __m128 a_reg = _mm_loadu_ps((float*)a);
        __m128 b_reg;
        __m128 expected_reg = _mm_loadu_ps((float*)expected);
        
        if (test_type == TEST_REG) {
            b_reg = _mm_loadu_ps((float*)b);
        } else {
            b_reg = _mm_setzero_ps(); // 内存操作数将在汇编中处理
        }
        
        if (test_type == TEST_REG) {
            b_reg = _mm_loadu_ps((float*)b);
            vcmpps128_funcs[imm](&res, a_reg, b_reg);
        } else { // TEST_MEM
            vcmpps128_mem_funcs[imm](&res, a_reg, (float*)b);
        }
        
        // 比较结果
        __m128i res_i = _mm_castps_si128(res);
        __m128i expected_i = _mm_castps_si128(expected_reg);
        int ret = _mm_testc_si128(res_i, expected_i) && 
                  _mm_testc_si128(expected_i, res_i);
        
        printf("%s: %s\n", name, ret ? "✅ PASS" : "❌ FAIL");
        return ret;
    }
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
    
    // 移除未使用的变量声明
    // 测试特殊值: NaN、无穷大、零值
    // 特殊值测试：0.0和-0.0比较应为真，NaN比较应为假
    // 特殊值测试
    __m256 special_expected = _mm256_castsi256_ps(_mm256_setr_epi32(
        0xFFFFFFFF,  // 0.0 == -0.0 -> true
        0xFFFFFFFF,  // -0.0 == 0.0 -> true
        0x00000000,  // inf != neg_inf -> false
        0x00000000,  // neg_inf != inf -> false
        0x00000000,  // NaN != NaN -> false
        0x00000000,  // 1.0 != 4.0 -> false
        0x00000000,  // 2.0 != 5.0 -> false
        0x00000000   // 3.0 != 6.0 -> false
    ));
    
    passed += test_case("vcmpps QNaN/SNaN special", 
                       &special_a, &special_b, 0x00,
                       &special_expected, 1, TEST_REG);
    total++;
    
    // 零值测试
    __m256 zero_test_a = _mm256_setr_ps(zero, neg_zero, zero, neg_zero, 1.0f, 2.0f, 3.0f, 4.0f);
    __m256 zero_test_b = _mm256_setr_ps(neg_zero, zero, 1.0f, -1.0f, zero, neg_zero, 3.0f, 4.0f);
    
    __m256 zero_expected = _mm256_castsi256_ps(_mm256_setr_epi32(
        0xFFFFFFFF,  // +0.0 == -0.0
        0xFFFFFFFF,  // -0.0 == +0.0
        0x00000000,  // 0.0 != 1.0
        0x00000000,  // -0.0 != -1.0
        0x00000000,  // 1.0 != 0.0
        0x00000000,  // 2.0 != -0.0
        0xFFFFFFFF,  // 3.0 == 3.0
        0xFFFFFFFF   // 4.0 == 4.0
    ));
    
    passed += test_case("vcmpps zero values", 
                       &zero_test_a, &zero_test_b, 0x00,
                       &zero_expected, 1, TEST_REG);
    total++;
    
    // 测试所有32种比较条件
        for (int imm = 0; imm < 32; imm++) {
            char name[64];
            snprintf(name, sizeof(name), "vcmpps imm=0x%02X", imm);
            
            __m256 expected = _mm256_setzero_ps();
        
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
        
        __m256 a_val = a;
        __m256 b_val = b;
        passed += test_case(name, &a_val, &b_val, imm, &expected, 1, TEST_REG);
        total++;
    }
    
    // 特殊值测试：NaN、无穷大、零值
    // 移除重复的特殊值测试（保留一个即可）
    // 保留边界值测试
    
    // 专门的边界值测试（覆盖所有16种核心条件）
    const char* condition_names[16] = {
        "EQ", "LT", "LE", "UNORD", "NEQ", "NLT", "NLE", "ORD",
        "EQ_UQ", "NGE", "NGT", "FALSE", "NEQ_OQ", "GE", "GT", "TRUE"
    };
    
    // 边界值测试向量：包含各种边界情况
    float boundary_values[] = {
        0.0f, -0.0f, INFINITY, -INFINITY, 
        NAN, 1.0f, -1.0f, 2.0f
    };
    
    // 生成所有可能的双操作数组合 (8x8=64种组合)
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            __m256 bound_a = _mm256_set1_ps(boundary_values[i]);
            __m256 bound_b = _mm256_set1_ps(boundary_values[j]);
            
            // 测试所有16种核心条件
            for (int imm = 0; imm < 16; imm++) {
                char name[128];
                snprintf(name, sizeof(name), "vcmpps BOUNDARY [%d:%d] cond=%s", 
                         i, j, condition_names[imm]);
                
                // 计算预期结果
                __m256 expected = _mm256_setzero_ps();
                int32_t exp_int[8];
                for (int k = 0; k < 8; k++) {
                    float ai = boundary_values[i];
                    float bi = boundary_values[j];
                    int cmp_result = 0;
                    
                // 根据imm值计算预期结果（正确处理特殊值）
                int unordered = isnan(ai) || isnan(bi);
                int ordered = !unordered;
                int equal = (ai == bi);
                int less = (ai < bi);
                int greater = (ai > bi);
                
                switch(imm) {
                    case 0x00: cmp_result = equal; break;                      // EQ
                    case 0x01: cmp_result = less && ordered; break;            // LT
                    case 0x02: cmp_result = (less || equal) && ordered; break; // LE
                    case 0x03: cmp_result = unordered; break;                  // UNORD
                    case 0x04: cmp_result = !equal; break;                     // NEQ
                    case 0x05: cmp_result = !less || unordered; break;         // NLT
                    case 0x06: cmp_result = !(less || equal) || unordered; break; // NLE
                    case 0x07: cmp_result = ordered; break;                    // ORD
                    case 0x08: cmp_result = equal || (isnan(ai) && isnan(bi)); break; // EQ_UQ
                    case 0x09: cmp_result = !(greater || equal) || unordered; break; // NGE
                    case 0x0A: cmp_result = !greater || unordered; break;      // NGT
                    case 0x0B: cmp_result = 0; break;                          // FALSE
                    case 0x0C: cmp_result = !equal && ordered; break;          // NEQ_OQ
                    case 0x0D: cmp_result = (greater || equal) && ordered; break; // GE
                    case 0x0E: cmp_result = greater && ordered; break;         // GT
                    case 0x0F: cmp_result = 1; break;                          // TRUE
                    // 处理16-31的扩展条件
                    case 0x10: cmp_result = equal && ordered; break;           // EQ_OS
                    case 0x11: cmp_result = less && ordered; break;            // LT_OS
                    case 0x12: cmp_result = (less || equal) && ordered; break; // LE_OS
                    case 0x13: cmp_result = unordered; break;                  // UNORD_S
                    case 0x14: cmp_result = !equal || unordered; break;        // NEQ_US
                    case 0x15: cmp_result = !less || unordered; break;         // NLT_US
                    case 0x16: cmp_result = !(less || equal) || unordered; break; // NLE_US
                    case 0x17: cmp_result = ordered; break;                    // ORD_S
                    case 0x18: cmp_result = equal || unordered; break;         // EQ_US
                    case 0x19: cmp_result = !(greater || equal) || unordered; break; // NGE_US
                    case 0x1A: cmp_result = !greater || unordered; break;      // NGT_US
                    case 0x1B: cmp_result = 0; break;                          // FALSE_OS
                    case 0x1C: cmp_result = !equal && ordered; break;          // NEQ_OS
                    case 0x1D: cmp_result = (greater || equal) && ordered; break; // GE_OS
                    case 0x1E: cmp_result = greater && ordered; break;         // GT_OS
                    case 0x1F: cmp_result = 1; break;                          // TRUE_US
                }
                    exp_int[k] = cmp_result ? 0xFFFFFFFF : 0x00000000;
                }
                __m256i expected_i = _mm256_loadu_si256((__m256i*)exp_int);
                expected = _mm256_castsi256_ps(expected_i);
                
                __m256 bound_a_val = bound_a;
                __m256 bound_b_val = bound_b;
                passed += test_case(name, &bound_a_val, &bound_b_val, imm, &expected, 1, TEST_REG);
                total++;
            }
        }
    }
    
    // 测试总结（改进输出格式）
    printf("\n\n=== TEST SUMMARY ===\n");
    printf("Total tests: %d\n", total);
    printf("Passed tests: %d\n", passed);
    printf("Failed tests: %d\n", total - passed);
    printf("====================\n");
    
    // ========== 添加内存操作数测试 ==========
    printf("\n=== Testing memory operands ===\n");
    float mem_operand[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    
    // 测试256位内存操作数
    __m256 mem_expected = _mm256_castsi256_ps(_mm256_set1_epi32(0xFFFFFFFF));
    passed += test_case("vcmpps256 memory operand", &a, mem_operand, 0x00, 
                       &mem_expected, 1, TEST_MEM);
    total++;
    
    // 测试128位内存操作数
    __m128 a128 = _mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f);
    __m128 mem_expected128 = _mm_castsi128_ps(_mm_set1_epi32(0xFFFFFFFF));
    passed += test_case("vcmpps128 memory operand", &a128, mem_operand, 0x00, 
                       &mem_expected128, 0, TEST_MEM);
    total++;

    // ========== 扩展特殊值测试到所有32种条件 ==========
    printf("\n=== Testing special values for all conditions ===\n");
    for (int imm = 0; imm < 32; imm++) {
        char name[64];
        snprintf(name, sizeof(name), "vcmpps special values imm=0x%02X", imm);
        
        // 256位特殊值测试
        __m256 special_expected = _mm256_castsi256_ps(_mm256_setr_epi32(
            0xFFFFFFFF,  // 0.0 == -0.0 -> true
            0xFFFFFFFF,  // -0.0 == 0.0 -> true
            0x00000000,  // inf != neg_inf -> false
            0x00000000,  // neg_inf != inf -> false
            0x00000000,  // NaN != NaN -> false
            0x00000000,  // 1.0 != 4.0 -> false
            0x00000000,  // 2.0 != 5.0 -> false
            0x00000000   // 3.0 != 6.0 -> false
        ));
        
        passed += test_case(name, &special_a, &special_b, imm, 
                           &special_expected, 1, TEST_REG);
        total++;
        
        // 128位特殊值测试
        __m128 special_a128 = _mm_setr_ps(zero, neg_zero, inf, nan);
        __m128 special_b128 = _mm_setr_ps(neg_zero, zero, neg_inf, nan);
        __m128 special_expected128 = _mm_castsi128_ps(_mm_setr_epi32(
            0xFFFFFFFF,  // 0.0 == -0.0 -> true
            0xFFFFFFFF,  // -0.0 == 0.0 -> true
            0x00000000,  // inf != neg_inf -> false
            0x00000000   // NaN != NaN -> false
        ));
        
        char name128[64];
        snprintf(name128, sizeof(name128), "vcmpps128 special values imm=0x%02X", imm);
        passed += test_case(name128, &special_a128, &special_b128, imm, 
                           &special_expected128, 0, TEST_REG);
        total++;
    }

    // 测试总结（改进输出格式）
    printf("\n\n=== TEST SUMMARY ===\n");
    printf("Total tests: %d\n", total);
    printf("Passed tests: %d\n", passed);
    printf("Failed tests: %d\n", total - passed);
    printf("====================\n");
    
    return passed == total ? 0 : 1;
}
