#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// ========== 比较函数宏 ==========
#define DEFINE_CMPSD_FUNC(imm) \
static void vcmpsd_##imm(__m128d *dst, __m128d a, __m128d b) { \
    asm volatile("vcmpsd $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "x"(b)); \
}

// 为所有32种条件定义函数
DEFINE_CMPSD_FUNC(0x00) DEFINE_CMPSD_FUNC(0x01) DEFINE_CMPSD_FUNC(0x02) DEFINE_CMPSD_FUNC(0x03)
DEFINE_CMPSD_FUNC(0x04) DEFINE_CMPSD_FUNC(0x05) DEFINE_CMPSD_FUNC(0x06) DEFINE_CMPSD_FUNC(0x07)
DEFINE_CMPSD_FUNC(0x08) DEFINE_CMPSD_FUNC(0x09) DEFINE_CMPSD_FUNC(0x0A) DEFINE_CMPSD_FUNC(0x0B)
DEFINE_CMPSD_FUNC(0x0C) DEFINE_CMPSD_FUNC(0x0D) DEFINE_CMPSD_FUNC(0x0E) DEFINE_CMPSD_FUNC(0x0F)
DEFINE_CMPSD_FUNC(0x10) DEFINE_CMPSD_FUNC(0x11) DEFINE_CMPSD_FUNC(0x12) DEFINE_CMPSD_FUNC(0x13)
DEFINE_CMPSD_FUNC(0x14) DEFINE_CMPSD_FUNC(0x15) DEFINE_CMPSD_FUNC(0x16) DEFINE_CMPSD_FUNC(0x17)
DEFINE_CMPSD_FUNC(0x18) DEFINE_CMPSD_FUNC(0x19) DEFINE_CMPSD_FUNC(0x1A) DEFINE_CMPSD_FUNC(0x1B)
DEFINE_CMPSD_FUNC(0x1C) DEFINE_CMPSD_FUNC(0x1D) DEFINE_CMPSD_FUNC(0x1E) DEFINE_CMPSD_FUNC(0x1F)

// 函数指针类型和数组
typedef void (*vcmpsd_func_t)(__m128d*, __m128d, __m128d);
static vcmpsd_func_t vcmpsd_funcs[32] = {
    vcmpsd_0x00, vcmpsd_0x01, vcmpsd_0x02, vcmpsd_0x03,
    vcmpsd_0x04, vcmpsd_0x05, vcmpsd_0x06, vcmpsd_0x07,
    vcmpsd_0x08, vcmpsd_0x09, vcmpsd_0x0A, vcmpsd_0x0B,
    vcmpsd_0x0C, vcmpsd_0x0D, vcmpsd_0x0E, vcmpsd_0x0F,
    vcmpsd_0x10, vcmpsd_0x11, vcmpsd_0x12, vcmpsd_0x13,
    vcmpsd_0x14, vcmpsd_0x15, vcmpsd_0x16, vcmpsd_0x17,
    vcmpsd_0x18, vcmpsd_0x19, vcmpsd_0x1A, vcmpsd_0x1B,
    vcmpsd_0x1C, vcmpsd_0x1D, vcmpsd_0x1E, vcmpsd_0x1F
};

// ========== 内存操作数支持 ==========
#define DEFINE_CMPSD_MEM_FUNC(imm) \
static void vcmpsd_mem_##imm(__m128d *dst, __m128d a, double *mem) { \
    asm volatile("vcmpsd $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "m"(*mem)); \
}

// 为所有32种条件定义内存操作数函数
DEFINE_CMPSD_MEM_FUNC(0x00) DEFINE_CMPSD_MEM_FUNC(0x01) DEFINE_CMPSD_MEM_FUNC(0x02) DEFINE_CMPSD_MEM_FUNC(0x03)
DEFINE_CMPSD_MEM_FUNC(0x04) DEFINE_CMPSD_MEM_FUNC(0x05) DEFINE_CMPSD_MEM_FUNC(0x06) DEFINE_CMPSD_MEM_FUNC(0x07)
DEFINE_CMPSD_MEM_FUNC(0x08) DEFINE_CMPSD_MEM_FUNC(0x09) DEFINE_CMPSD_MEM_FUNC(0x0A) DEFINE_CMPSD_MEM_FUNC(0x0B)
DEFINE_CMPSD_MEM_FUNC(0x0C) DEFINE_CMPSD_MEM_FUNC(0x0D) DEFINE_CMPSD_MEM_FUNC(0x0E) DEFINE_CMPSD_MEM_FUNC(0x0F)
DEFINE_CMPSD_MEM_FUNC(0x10) DEFINE_CMPSD_MEM_FUNC(0x11) DEFINE_CMPSD_MEM_FUNC(0x12) DEFINE_CMPSD_MEM_FUNC(0x13)
DEFINE_CMPSD_MEM_FUNC(0x14) DEFINE_CMPSD_MEM_FUNC(0x15) DEFINE_CMPSD_MEM_FUNC(0x16) DEFINE_CMPSD_MEM_FUNC(0x17)
DEFINE_CMPSD_MEM_FUNC(0x18) DEFINE_CMPSD_MEM_FUNC(0x19) DEFINE_CMPSD_MEM_FUNC(0x1A) DEFINE_CMPSD_MEM_FUNC(0x1B)
DEFINE_CMPSD_MEM_FUNC(0x1C) DEFINE_CMPSD_MEM_FUNC(0x1D) DEFINE_CMPSD_MEM_FUNC(0x1E) DEFINE_CMPSD_MEM_FUNC(0x1F)

// 内存操作数函数指针类型
typedef void (*vcmpsd_mem_func_t)(__m128d*, __m128d, double*);
static vcmpsd_mem_func_t vcmpsd_mem_funcs[32] = {
    vcmpsd_mem_0x00, vcmpsd_mem_0x01, vcmpsd_mem_0x02, vcmpsd_mem_0x03,
    vcmpsd_mem_0x04, vcmpsd_mem_0x05, vcmpsd_mem_0x06, vcmpsd_mem_0x07,
    vcmpsd_mem_0x08, vcmpsd_mem_0x09, vcmpsd_mem_0x0A, vcmpsd_mem_0x0B,
    vcmpsd_mem_0x0C, vcmpsd_mem_0x0D, vcmpsd_mem_0x0E, vcmpsd_mem_0x0F,
    vcmpsd_mem_0x10, vcmpsd_mem_0x11, vcmpsd_mem_0x12, vcmpsd_mem_0x13,
    vcmpsd_mem_0x14, vcmpsd_mem_0x15, vcmpsd_mem_0x16, vcmpsd_mem_0x17,
    vcmpsd_mem_0x18, vcmpsd_mem_0x19, vcmpsd_mem_0x1A, vcmpsd_mem_0x1B,
    vcmpsd_mem_0x1C, vcmpsd_mem_0x1D, vcmpsd_mem_0x1E, vcmpsd_mem_0x1F
};

// 测试用例类型枚举
typedef enum {
    TEST_REG,       // 寄存器操作数
    TEST_MEM        // 内存操作数
} test_type_t;

// 测试函数
static int test_case(const char *name, double a, double b, int imm, uint64_t expected, 
                     test_type_t test_type) {
    __m128d res = _mm_setzero_pd();
    __m128d a_reg = _mm_set_sd(a);
    uint64_t got;
    
    if (test_type == TEST_REG) {
        __m128d b_reg = _mm_set_sd(b);
        vcmpsd_funcs[imm](&res, a_reg, b_reg);
    } else { // TEST_MEM
        vcmpsd_mem_funcs[imm](&res, a_reg, &b);
    }
    
    // 提取结果
    got = _mm_extract_epi64(_mm_castpd_si128(res), 0);
    
    int ret = (got == expected);
    
    if (!ret) {
        printf("❌ FAIL: %s\n", name);
        printf("  A: %f (0x%016lX)\n", a, *(uint64_t*)&a);
        printf("  B: %f (0x%016lX)\n", b, *(uint64_t*)&b);
        printf("  Expected: 0x%016lX\n", expected);
        printf("  Got:      0x%016lX\n", got);
    } else {
        printf("✅ PASS: %s\n", name);
    }
    return ret;
}

int main() {
    int total = 0, passed = 0;
    
    // 定义测试值
    double nan = NAN;
    double inf = INFINITY;
    double neg_inf = -INFINITY;
    double zero = 0.0;
    double neg_zero = -0.0;
    
    // 测试所有32种比较条件
    for (int imm = 0; imm < 32; imm++) {
        char name[64];
        snprintf(name, sizeof(name), "vcmpsd imm=0x%02X", imm);
        
        // 基础测试值
        double a = 1.0;
        double b = 2.0;
        uint64_t expected = 0;
        
        // 根据比较条件设置预期结果
        int unordered = isnan(a) || isnan(b);
        int ordered = !unordered;
        int equal = (a == b);
        int less = (a < b);
        int greater = (a > b);
        
        switch(imm) {
            case 0x00: expected = equal ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x01: expected = (less && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x02: expected = ((less || equal) && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x03: expected = unordered ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x04: expected = !equal ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x05: expected = (!less || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x06: expected = (!(less || equal) || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x07: expected = ordered ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x08: expected = (equal || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x09: expected = (!(greater || equal) || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x0A: expected = (!greater || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x0B: expected = 0; break;
            case 0x0C: expected = (!equal && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x0D: expected = ((greater || equal) && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x0E: expected = (greater && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x0F: expected = 0xFFFFFFFFFFFFFFFF; break;
            case 0x10: expected = (equal && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x11: expected = (less && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x12: expected = ((less || equal) && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x13: expected = unordered ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x14: expected = (!equal || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x15: expected = (!less || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x16: expected = (!(less || equal) || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x17: expected = ordered ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x18: expected = (equal || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x19: expected = (!(greater || equal) || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x1A: expected = (!greater || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x1B: expected = 0; break;
            case 0x1C: expected = (!equal && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x1D: expected = ((greater || equal) && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x1E: expected = (greater && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
            case 0x1F: expected = 0xFFFFFFFFFFFFFFFF; break;
        }
        
        passed += test_case(name, a, b, imm, expected, TEST_REG);
        total++;
    }
    
    // 特殊值测试
    double boundary_values[] = {
        0.0, -0.0, INFINITY, -INFINITY, 
        NAN, 1.0, -1.0, 2.0
    };
    
    // 生成所有可能的双操作数组合 (8x8=64种组合)
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            double a = boundary_values[i];
            double b = boundary_values[j];
            
            // 测试所有32种条件
            for (int imm = 0; imm < 32; imm++) {
                char name[128];
                snprintf(name, sizeof(name), "vcmpsd BOUNDARY [%d:%d] imm=0x%02X", 
                         i, j, imm);
                
                // 计算预期结果
                uint64_t expected = 0;
                
                int unordered = isnan(a) || isnan(b);
                int ordered = !unordered;
                int equal = (a == b);
                int less = (a < b);
                int greater = (a > b);
                
                switch(imm) {
                    case 0x00: expected = equal ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x01: expected = (less && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x02: expected = ((less || equal) && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x03: expected = unordered ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x04: expected = !equal ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x05: expected = (!less || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x06: expected = (!(less || equal) || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x07: expected = ordered ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x08: expected = (equal || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x09: expected = (!(greater || equal) || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x0A: expected = (!greater || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x0B: expected = 0; break;
                    case 0x0C: expected = (!equal && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x0D: expected = ((greater || equal) && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x0E: expected = (greater && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x0F: expected = 0xFFFFFFFFFFFFFFFF; break;
                    case 0x10: expected = (equal && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x11: expected = (less && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x12: expected = ((less || equal) && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x13: expected = unordered ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x14: expected = (!equal || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x15: expected = (!less || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x16: expected = (!(less || equal) || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x17: expected = ordered ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x18: expected = (equal || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x19: expected = (!(greater || equal) || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x1A: expected = (!greater || unordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x1B: expected = 0; break;
                    case 0x1C: expected = (!equal && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x1D: expected = ((greater || equal) && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x1E: expected = (greater && ordered) ? 0xFFFFFFFFFFFFFFFF : 0; break;
                    case 0x1F: expected = 0xFFFFFFFFFFFFFFFF; break;
                }
                
                passed += test_case(name, a, b, imm, expected, TEST_REG);
                total++;
            }
        }
    }
    
    // 内存操作数测试
    printf("\n=== Testing memory operands ===\n");
    double a_val = 1.0;
    double b_val = 1.0;
    passed += test_case("vcmpsd mem operand EQ", a_val, b_val, 0x00, 0xFFFFFFFFFFFFFFFF, TEST_MEM);
    total++;
    
    b_val = 2.0;
    passed += test_case("vcmpsd mem operand NEQ", a_val, b_val, 0x04, 0xFFFFFFFFFFFFFFFF, TEST_MEM);
    total++;

    // 特殊值测试
    printf("\n=== Testing special values ===\n");
    // 零值测试
    passed += test_case("vcmpsd +0.0 == -0.0", zero, neg_zero, 0x00, 0xFFFFFFFFFFFFFFFF, TEST_REG);
    total++;
    passed += test_case("vcmpsd -0.0 == +0.0", neg_zero, zero, 0x00, 0xFFFFFFFFFFFFFFFF, TEST_REG);
    total++;
    
    // NaN测试
    passed += test_case("vcmpsd NaN == NaN", nan, nan, 0x00, 0, TEST_REG);
    total++;
    passed += test_case("vcmpsd NaN != NaN", nan, nan, 0x04, 0xFFFFFFFFFFFFFFFF, TEST_REG);
    total++;
    
    // 无穷大测试
    passed += test_case("vcmpsd inf > neg_inf", inf, neg_inf, 0x1E, 0xFFFFFFFFFFFFFFFF, TEST_REG);
    total++;
    passed += test_case("vcmpsd neg_inf < inf", neg_inf, inf, 0x01, 0xFFFFFFFFFFFFFFFF, TEST_REG);
    total++;

    // 测试总结
    printf("\n\n=== TEST SUMMARY ===\n");
    printf("Total tests: %d\n", total);
    printf("Passed tests: %d\n", passed);
    printf("Failed tests: %d\n", total - passed);
    printf("====================\n");
    
    return passed == total ? 0 : 1;
}
