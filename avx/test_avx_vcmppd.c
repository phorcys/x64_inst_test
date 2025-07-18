#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// ========== 256位比较函数宏 ==========
#define DEFINE_CMP256_FUNC(imm) \
static void vcmppd256_##imm(__m256d *dst, __m256d a, __m256d b) { \
    asm volatile("vcmppd $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "x"(b)); \
}

// 为所有32种条件定义256位函数
DEFINE_CMP256_FUNC(0x00) DEFINE_CMP256_FUNC(0x01) DEFINE_CMP256_FUNC(0x02) DEFINE_CMP256_FUNC(0x03)
DEFINE_CMP256_FUNC(0x04) DEFINE_CMP256_FUNC(0x05) DEFINE_CMP256_FUNC(0x06) DEFINE_CMP256_FUNC(0x07)
DEFINE_CMP256_FUNC(0x08) DEFINE_CMP256_FUNC(0x09) DEFINE_CMP256_FUNC(0x0A) DEFINE_CMP256_FUNC(0x0B)
DEFINE_CMP256_FUNC(0x0C) DEFINE_CMP256_FUNC(0x0D) DEFINE_CMP256_FUNC(0x0E) DEFINE_CMP256_FUNC(0x0F)
DEFINE_CMP256_FUNC(0x10) DEFINE_CMP256_FUNC(0x11) DEFINE_CMP256_FUNC(0x12) DEFINE_CMP256_FUNC(0x13)
DEFINE_CMP256_FUNC(0x14) DEFINE_CMP256_FUNC(0x15) DEFINE_CMP256_FUNC(0x16) DEFINE_CMP256_FUNC(0x17)
DEFINE_CMP256_FUNC(0x18) DEFINE_CMP256_FUNC(0x19) DEFINE_CMP256_FUNC(0x1A) DEFINE_CMP256_FUNC(0x1B)
DEFINE_CMP256_FUNC(0x1C) DEFINE_CMP256_FUNC(0x1D) DEFINE_CMP256_FUNC(0x1E) DEFINE_CMP256_FUNC(0x1F)

// 256位函数指针类型和数组
typedef void (*vcmppd256_func_t)(__m256d*, __m256d, __m256d);
static vcmppd256_func_t vcmppd256_funcs[32] = {
    vcmppd256_0x00, vcmppd256_0x01, vcmppd256_0x02, vcmppd256_0x03,
    vcmppd256_0x04, vcmppd256_0x05, vcmppd256_0x06, vcmppd256_0x07,
    vcmppd256_0x08, vcmppd256_0x09, vcmppd256_0x0A, vcmppd256_0x0B,
    vcmppd256_0x0C, vcmppd256_0x0D, vcmppd256_0x0E, vcmppd256_0x0F,
    vcmppd256_0x10, vcmppd256_0x11, vcmppd256_0x12, vcmppd256_0x13,
    vcmppd256_0x14, vcmppd256_0x15, vcmppd256_0x16, vcmppd256_0x17,
    vcmppd256_0x18, vcmppd256_0x19, vcmppd256_0x1A, vcmppd256_0x1B,
    vcmppd256_0x1C, vcmppd256_0x1D, vcmppd256_0x1E, vcmppd256_0x1F
};

// ========== 128位比较函数宏 ==========
#define DEFINE_CMP128_FUNC(imm) \
static void vcmppd128_##imm(__m128d *dst, __m128d a, __m128d b) { \
    asm volatile("vcmppd $"#imm", %2, %1, %0" \
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

// 128位函数指针类型和数组
typedef void (*vcmppd128_func_t)(__m128d*, __m128d, __m128d);
static vcmppd128_func_t vcmppd128_funcs[32] = {
    vcmppd128_0x00, vcmppd128_0x01, vcmppd128_0x02, vcmppd128_0x03,
    vcmppd128_0x04, vcmppd128_0x05, vcmppd128_0x06, vcmppd128_0x07,
    vcmppd128_0x08, vcmppd128_0x09, vcmppd128_0x0A, vcmppd128_0x0B,
    vcmppd128_0x0C, vcmppd128_0x0D, vcmppd128_0x0E, vcmppd128_0x0F,
    vcmppd128_0x10, vcmppd128_0x11, vcmppd128_0x12, vcmppd128_0x13,
    vcmppd128_0x14, vcmppd128_0x15, vcmppd128_0x16, vcmppd128_0x17,
    vcmppd128_0x18, vcmppd128_0x19, vcmppd128_0x1A, vcmppd128_0x1B,
    vcmppd128_0x1C, vcmppd128_0x1D, vcmppd128_0x1E, vcmppd128_0x1F
};

// ========== 内存操作数支持 ==========
#define DEFINE_CMP256_MEM_FUNC(imm) \
static void vcmppd256_mem_##imm(__m256d *dst, __m256d a, __m256d *mem) { \
    asm volatile("vcmppd $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "m"(*mem)); \
}

#define DEFINE_CMP128_MEM_FUNC(imm) \
static void vcmppd128_mem_##imm(__m128d *dst, __m128d a, __m128d *mem) { \
    asm volatile("vcmppd $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "m"(*mem)); \
}

// 为所有32种条件定义内存操作数函数
DEFINE_CMP256_MEM_FUNC(0x00) DEFINE_CMP256_MEM_FUNC(0x01) DEFINE_CMP256_MEM_FUNC(0x02) DEFINE_CMP256_MEM_FUNC(0x03)
DEFINE_CMP256_MEM_FUNC(0x04) DEFINE_CMP256_MEM_FUNC(0x05) DEFINE_CMP256_MEM_FUNC(0x06) DEFINE_CMP256_MEM_FUNC(0x07)
DEFINE_CMP256_MEM_FUNC(0x08) DEFINE_CMP256_MEM_FUNC(0x09) DEFINE_CMP256_MEM_FUNC(0x0A) DEFINE_CMP256_MEM_FUNC(0x0B)
DEFINE_CMP256_MEM_FUNC(0x0C) DEFINE_CMP256_MEM_FUNC(0x0D) DEFINE_CMP256_MEM_FUNC(0x0E) DEFINE_CMP256_MEM_FUNC(0x0F)
DEFINE_CMP256_MEM_FUNC(0x10) DEFINE_CMP256_MEM_FUNC(0x11) DEFINE_CMP256_MEM_FUNC(0x12) DEFINE_CMP256_MEM_FUNC(0x13)
DEFINE_CMP256_MEM_FUNC(0x14) DEFINE_CMP256_MEM_FUNC(0x15) DEFINE_CMP256_MEM_FUNC(0x16) DEFINE_CMP256_MEM_FUNC(0x17)
DEFINE_CMP256_MEM_FUNC(0x18) DEFINE_CMP256_MEM_FUNC(0x19) DEFINE_CMP256_MEM_FUNC(0x1A) DEFINE_CMP256_MEM_FUNC(0x1B)
DEFINE_CMP256_MEM_FUNC(0x1C) DEFINE_CMP256_MEM_FUNC(0x1D) DEFINE_CMP256_MEM_FUNC(0x1E) DEFINE_CMP256_MEM_FUNC(0x1F)

DEFINE_CMP128_MEM_FUNC(0x00) DEFINE_CMP128_MEM_FUNC(0x01) DEFINE_CMP128_MEM_FUNC(0x02) DEFINE_CMP128_MEM_FUNC(0x03)
DEFINE_CMP128_MEM_FUNC(0x04) DEFINE_CMP128_MEM_FUNC(0x05) DEFINE_CMP128_MEM_FUNC(0x06) DEFINE_CMP128_MEM_FUNC(0x07)
DEFINE_CMP128_MEM_FUNC(0x08) DEFINE_CMP128_MEM_FUNC(0x09) DEFINE_CMP128_MEM_FUNC(0x0A) DEFINE_CMP128_MEM_FUNC(0x0B)
DEFINE_CMP128_MEM_FUNC(0x0C) DEFINE_CMP128_MEM_FUNC(0x0D) DEFINE_CMP128_MEM_FUNC(0x0E) DEFINE_CMP128_MEM_FUNC(0x0F)
DEFINE_CMP128_MEM_FUNC(0x10) DEFINE_CMP128_MEM_FUNC(0x11) DEFINE_CMP128_MEM_FUNC(0x12) DEFINE_CMP128_MEM_FUNC(0x13)
DEFINE_CMP128_MEM_FUNC(0x14) DEFINE_CMP128_MEM_FUNC(0x15) DEFINE_CMP128_MEM_FUNC(0x16) DEFINE_CMP128_MEM_FUNC(0x17)
DEFINE_CMP128_MEM_FUNC(0x18) DEFINE_CMP128_MEM_FUNC(0x19) DEFINE_CMP128_MEM_FUNC(0x1A) DEFINE_CMP128_MEM_FUNC(0x1B)
DEFINE_CMP128_MEM_FUNC(0x1C) DEFINE_CMP128_MEM_FUNC(0x1D) DEFINE_CMP128_MEM_FUNC(0x1E) DEFINE_CMP128_MEM_FUNC(0x1F)

// 内存操作数函数指针类型
typedef void (*vcmppd256_mem_func_t)(__m256d*, __m256d, __m256d*);
static vcmppd256_mem_func_t vcmppd256_mem_funcs[32] = {
    vcmppd256_mem_0x00, vcmppd256_mem_0x01, vcmppd256_mem_0x02, vcmppd256_mem_0x03,
    vcmppd256_mem_0x04, vcmppd256_mem_0x05, vcmppd256_mem_0x06, vcmppd256_mem_0x07,
    vcmppd256_mem_0x08, vcmppd256_mem_0x09, vcmppd256_mem_0x0A, vcmppd256_mem_0x0B,
    vcmppd256_mem_0x0C, vcmppd256_mem_0x0D, vcmppd256_mem_0x0E, vcmppd256_mem_0x0F,
    vcmppd256_mem_0x10, vcmppd256_mem_0x11, vcmppd256_mem_0x12, vcmppd256_mem_0x13,
    vcmppd256_mem_0x14, vcmppd256_mem_0x15, vcmppd256_mem_0x16, vcmppd256_mem_0x17,
    vcmppd256_mem_0x18, vcmppd256_mem_0x19, vcmppd256_mem_0x1A, vcmppd256_mem_0x1B,
    vcmppd256_mem_0x1C, vcmppd256_mem_0x1D, vcmppd256_mem_0x1E, vcmppd256_mem_0x1F
};

typedef void (*vcmppd128_mem_func_t)(__m128d*, __m128d, __m128d*);
static vcmppd128_mem_func_t vcmppd128_mem_funcs[32] = {
    vcmppd128_mem_0x00, vcmppd128_mem_0x01, vcmppd128_mem_0x02, vcmppd128_mem_0x03,
    vcmppd128_mem_0x04, vcmppd128_mem_0x05, vcmppd128_mem_0x06, vcmppd128_mem_0x07,
    vcmppd128_mem_0x08, vcmppd128_mem_0x09, vcmppd128_mem_0x0A, vcmppd128_mem_0x0B,
    vcmppd128_mem_0x0C, vcmppd128_mem_0x0D, vcmppd128_mem_0x0E, vcmppd128_mem_0x0F,
    vcmppd128_mem_0x10, vcmppd128_mem_0x11, vcmppd128_mem_0x12, vcmppd128_mem_0x13,
    vcmppd128_mem_0x14, vcmppd128_mem_0x15, vcmppd128_mem_0x16, vcmppd128_mem_0x17,
    vcmppd128_mem_0x18, vcmppd128_mem_0x19, vcmppd128_mem_0x1A, vcmppd128_mem_0x1B,
    vcmppd128_mem_0x1C, vcmppd128_mem_0x1D, vcmppd128_mem_0x1E, vcmppd128_mem_0x1F
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
        __m256d res = _mm256_setzero_pd();
        __m256d a_reg = _mm256_loadu_pd((double*)a);
        __m256d b_reg;
        __m256d expected_reg = _mm256_loadu_pd((double*)expected);
        
        if (test_type == TEST_REG) {
            b_reg = _mm256_loadu_pd((double*)b);
            vcmppd256_funcs[imm](&res, a_reg, b_reg);
        } else { // TEST_MEM
            vcmppd256_mem_funcs[imm](&res, a_reg, (__m256d*)b);
        }
        
        // 比较结果
        __m256i res_i = _mm256_castpd_si256(res);
        __m256i expected_i = _mm256_castpd_si256(expected_reg);
        int ret = _mm256_testc_si256(res_i, expected_i) && 
                  _mm256_testc_si256(expected_i, res_i);
        
        if (!ret) {
            printf("❌ FAIL: %s\n", name);
            printf("  A: ");
            for (int k = 0; k < (width ? 4 : 2); k++) printf("%f ", ((double*)a)[k]);
            printf("\n  B: ");
            for (int k = 0; k < (width ? 4 : 2); k++) 
                printf("%f ", test_type == TEST_REG ? ((double*)b)[k] : (width ? ((double*)b)[k] : ((double*)b)[0]));
            printf("\n  Expected: ");
            for (int k = 0; k < (width ? 4 : 2); k++) printf("0x%016lX ", ((uint64_t*)expected)[k]);
            printf("\n  Got:      ");
            for (int k = 0; k < (width ? 4 : 2); k++) printf("0x%016lX ", ((uint64_t*)&res)[k]);
            printf("\n");
        } else {
            printf("✅ PASS: %s\n", name);
        }
        return ret;
    } 
    else { // 128位测试
        __m128d res = _mm_setzero_pd();
        __m128d a_reg = _mm_loadu_pd((double*)a);
        __m128d b_reg;
        __m128d expected_reg = _mm_loadu_pd((double*)expected);
        
        if (test_type == TEST_REG) {
            b_reg = _mm_loadu_pd((double*)b);
            vcmppd128_funcs[imm](&res, a_reg, b_reg);
        } else { // TEST_MEM
            vcmppd128_mem_funcs[imm](&res, a_reg, (__m128d*)b);
        }
        
        // 比较结果
        __m128i res_i = _mm_castpd_si128(res);
        __m128i expected_i = _mm_castpd_si128(expected_reg);
        int ret = _mm_testc_si128(res_i, expected_i) && 
                  _mm_testc_si128(expected_i, res_i);
        
        if (!ret) {
            printf("❌ FAIL: %s\n", name);
            printf("  A: ");
            for (int k = 0; k < 2; k++) printf("%f ", ((double*)a)[k]);
            printf("\n  B: ");
            for (int k = 0; k < 2; k++) 
                printf("%f ", test_type == TEST_REG ? ((double*)b)[k] : ((double*)b)[0]);
            printf("\n  Expected: ");
            for (int k = 0; k < 2; k++) printf("0x%016lX ", ((uint64_t*)expected)[k]);
            printf("\n  Got:      ");
            for (int k = 0; k < 2; k++) printf("0x%016lX ", ((uint64_t*)&res)[k]);
            printf("\n");
        } else {
            printf("✅ PASS: %s\n", name);
        }
        return ret;
    }
}

int main() {
    int total = 0, passed = 0;
    
    // 定义测试值
    double nan = NAN;
    double inf = INFINITY;
    double neg_inf = -INFINITY;
    double zero = 0.0;
    double neg_zero = -0.0;
    
    // 基础测试向量
    __m256d a = _mm256_setr_pd(1.0, 2.0, 3.0, 4.0);
    __m256d b = _mm256_setr_pd(1.0, 3.0, 2.0, 5.0);
    
    // 特殊值测试向量
    __m256d special_a = _mm256_setr_pd(zero, neg_zero, inf, nan);
    __m256d special_b = _mm256_setr_pd(neg_zero, zero, neg_inf, nan);
    
    // 特殊值预期结果
    __m256d special_expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
        0xFFFFFFFFFFFFFFFF,  // 0.0 == -0.0 -> true
        0xFFFFFFFFFFFFFFFF,  // -0.0 == 0.0 -> true
        0x0000000000000000,  // inf != neg_inf -> false
        0x0000000000000000   // NaN != NaN -> false
    ));
    
    passed += test_case("vcmppd QNaN/SNaN special", 
                       &special_a, &special_b, 0x00,
                       &special_expected, 1, TEST_REG);
    total++;
    
    // 零值测试
    __m256d zero_test_a = _mm256_setr_pd(zero, neg_zero, zero, 1.0);
    __m256d zero_test_b = _mm256_setr_pd(neg_zero, zero, 1.0, neg_zero);
    
    __m256d zero_expected = _mm256_castsi256_pd(_mm256_setr_epi64x(
        0xFFFFFFFFFFFFFFFF,  // +0.0 == -0.0
        0xFFFFFFFFFFFFFFFF,  // -0.0 == +0.0
        0x0000000000000000,  // 0.0 != 1.0
        0x0000000000000000   // 1.0 != -0.0
    ));
    
    passed += test_case("vcmppd zero values", 
                       &zero_test_a, &zero_test_b, 0x00,
                       &zero_expected, 1, TEST_REG);
    total++;
    
    // 测试所有32种比较条件
    for (int imm = 0; imm < 32; imm++) {
        char name[64];
        snprintf(name, sizeof(name), "vcmppd imm=0x%02X", imm);
        
        __m256d expected = _mm256_setzero_pd();
    
        // 根据比较条件设置预期结果
        int64_t exp_int[4];
        for (int i = 0; i < 4; i++) {
            double ai = ((double*)&a)[i];
            double bi = ((double*)&b)[i];
            int cmp_result = 0;
            
            // 简化条件判断逻辑
            int unordered = isnan(ai) || isnan(bi);
            int ordered = !unordered;
            int equal = (ai == bi);
            int less = (ai < bi);
            int greater = (ai > bi);
            (void)unordered; (void)ordered; (void)equal; (void)less; (void)greater; // 防止未使用警告
            
            switch(imm) {
                case 0x00: cmp_result = equal; break;
                case 0x01: cmp_result = less && ordered; break;
                case 0x02: cmp_result = (less || equal) && ordered; break;
                case 0x03: cmp_result = unordered; break;
                case 0x04: cmp_result = !equal; break;
                case 0x05: cmp_result = !less || unordered; break;
                case 0x06: cmp_result = !(less || equal) || unordered; break;
                case 0x07: cmp_result = ordered; break;
                case 0x08: cmp_result = (ai == bi) || (isnan(ai) || isnan(bi)); break;
                case 0x09: cmp_result = !(greater || equal) || unordered; break;
                case 0x0A: cmp_result = !greater || unordered; break;
                case 0x0B: cmp_result = 0; break;
                case 0x0C: cmp_result = !equal && ordered; break;
                case 0x0D: cmp_result = (greater || equal) && ordered; break;
                case 0x0E: cmp_result = greater && ordered; break;
                case 0x0F: cmp_result = 1; break;
                case 0x10: cmp_result = equal && ordered; break;
                case 0x11: cmp_result = less && ordered; break;
                case 0x12: cmp_result = (less || equal) && ordered; break;
                case 0x13: cmp_result = unordered; break;
                case 0x14: cmp_result = !equal || unordered; break;
                case 0x15: cmp_result = !less || unordered; break;
                case 0x16: cmp_result = !(less || equal) || unordered; break;
                case 0x17: cmp_result = ordered; break;
                case 0x18: cmp_result = equal || unordered; break;
                case 0x19: cmp_result = !(greater || equal) || unordered; break;
                case 0x1A: cmp_result = !greater || unordered; break;
                case 0x1B: cmp_result = 0; break;
                case 0x1C: cmp_result = !equal && ordered; break;
                case 0x1D: cmp_result = (greater || equal) && ordered; break;
                case 0x1E: cmp_result = greater && ordered; break;
                case 0x1F: cmp_result = 1; break;
            }
            
            exp_int[i] = cmp_result ? 0xFFFFFFFFFFFFFFFF : 0x0000000000000000;
        }
        __m256i expected_i = _mm256_loadu_si256((__m256i*)exp_int);
        expected = _mm256_castsi256_pd(expected_i);
        
        passed += test_case(name, &a, &b, imm, &expected, 1, TEST_REG);
        total++;
    }
    
    // 边界值测试向量
    double boundary_values[] = {
        0.0, -0.0, INFINITY, -INFINITY, 
        NAN, 1.0, -1.0, 2.0
    };
    
    // 生成所有可能的双操作数组合 (8x8=64种组合)
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            __m256d bound_a = _mm256_set1_pd(boundary_values[i]);
            __m256d bound_b = _mm256_set1_pd(boundary_values[j]);
            
            // 测试所有32种条件
            for (int imm = 0; imm < 32; imm++) {
                char name[128];
                snprintf(name, sizeof(name), "vcmppd BOUNDARY [%d:%d] imm=0x%02X", 
                         i, j, imm);
                
                // 计算预期结果
                __m256d expected = _mm256_setzero_pd();
                int64_t exp_int[4];
                for (int k = 0; k < 4; k++) {
                    double ai = boundary_values[i];
                    double bi = boundary_values[j];
                    int cmp_result = 0;
                    
                    // 根据imm值计算预期结果
                    int unordered = isnan(ai) || isnan(bi);
                    int ordered = !unordered;
                    int equal = (ai == bi);
                    int less = (ai < bi);
                    int greater = (ai > bi);
                    
                    // 根据条件计算预期结果
                    switch(imm) {
                        case 0x00: cmp_result = equal; break;
                        case 0x01: cmp_result = less && ordered; break;
                        case 0x02: cmp_result = (less || equal) && ordered; break;
                        case 0x03: cmp_result = unordered; break;
                        case 0x04: cmp_result = !equal; break;
                        case 0x05: cmp_result = !less || unordered; break;
                        case 0x06: cmp_result = !(less || equal) || unordered; break;
                        case 0x07: cmp_result = ordered; break;
                        case 0x08: cmp_result = (ai == bi) || (isnan(ai) || isnan(bi)); break;
                        case 0x09: cmp_result = !(greater || equal) || unordered; break;
                        case 0x0A: cmp_result = !greater || unordered; break;
                        case 0x0B: cmp_result = 0; break;
                        case 0x0C: cmp_result = !equal && ordered; break;
                        case 0x0D: cmp_result = (greater || equal) && ordered; break;
                        case 0x0E: cmp_result = greater && ordered; break;
                        case 0x0F: cmp_result = 1; break;
                        case 0x10: cmp_result = equal && ordered; break;
                        case 0x11: cmp_result = less && ordered; break;
                        case 0x12: cmp_result = (less || equal) && ordered; break;
                        case 0x13: cmp_result = unordered; break;
                        case 0x14: cmp_result = !equal || unordered; break;
                        case 0x15: cmp_result = !less || unordered; break;
                        case 0x16: cmp_result = !(less || equal) || unordered; break;
                        case 0x17: cmp_result = ordered; break;
                        case 0x18: cmp_result = equal || unordered; break;
                        case 0x19: cmp_result = !(greater || equal) || unordered; break;
                        case 0x1A: cmp_result = !greater || unordered; break;
                        case 0x1B: cmp_result = 0; break;
                        case 0x1C: cmp_result = !equal && ordered; break;
                        case 0x1D: cmp_result = (greater || equal) && ordered; break;
                        case 0x1E: cmp_result = greater && ordered; break;
                        case 0x1F: cmp_result = 1; break;
                    }
                    
                    exp_int[k] = cmp_result ? 0xFFFFFFFFFFFFFFFF : 0x0000000000000000;
                }
                __m256i expected_i = _mm256_loadu_si256((__m256i*)exp_int);
                expected = _mm256_castsi256_pd(expected_i);
                
                passed += test_case(name, &bound_a, &bound_b, imm, &expected, 1, TEST_REG);
                total++;
            }
        }
    }
    
    // 内存操作数测试
    printf("\n=== Testing memory operands ===\n");
    __m256d a_eq = _mm256_set1_pd(1.0);
    __m256d mem_operand = _mm256_set1_pd(1.0);
    __m256d expected_eq = _mm256_castsi256_pd(_mm256_set1_epi64x(0xFFFFFFFFFFFFFFFF));
    __m256d expected_neq = _mm256_castsi256_pd(_mm256_set1_epi64x(0x0000000000000000));
    
    passed += test_case("vcmppd256 mem operand EQ", &a_eq, &mem_operand, 0x00,
                       &expected_eq, 1, TEST_MEM);
    total++;
    
    passed += test_case("vcmppd256 mem operand NEQ", &a_eq, &mem_operand, 0x04,
                       &expected_neq, 1, TEST_MEM);
    total++;
    
    // 128位内存操作数测试
    __m128d a128_eq = _mm_set1_pd(1.0);
    __m128d mem_operand128 = _mm_set1_pd(1.0);
    __m128d expected128_eq = _mm_castsi128_pd(_mm_set1_epi64x(0xFFFFFFFFFFFFFFFF));
    __m128d expected128_neq = _mm_castsi128_pd(_mm_set1_epi64x(0x0000000000000000));
    
    passed += test_case("vcmppd128 mem operand EQ", &a128_eq, &mem_operand128, 0x00,
                       &expected128_eq, 0, TEST_MEM);
    total++;
    
    passed += test_case("vcmppd128 mem operand NEQ", &a128_eq, &mem_operand128, 0x04,
                       &expected128_neq, 0, TEST_MEM);
    total++;

    // 测试总结
    printf("\n\n=== TEST SUMMARY ===\n");
    printf("Total tests: %d\n", total);
    printf("Passed tests: %d\n", passed);
    printf("Failed tests: %d\n", total - passed);
    printf("====================\n");
    
    return passed == total ? 0 : 1;
}
