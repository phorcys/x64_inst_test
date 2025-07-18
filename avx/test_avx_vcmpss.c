#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// ========== 比较函数宏 ==========
#define DEFINE_CMPSS_FUNC(imm) \
static void vcmpss_##imm(__m128 *dst, __m128 a, __m128 b) { \
    asm volatile("vcmpss $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "x"(b)); \
}

// 为所有32种条件定义函数
DEFINE_CMPSS_FUNC(0x00) DEFINE_CMPSS_FUNC(0x01) DEFINE_CMPSS_FUNC(0x02) DEFINE_CMPSS_FUNC(0x03)
DEFINE_CMPSS_FUNC(0x04) DEFINE_CMPSS_FUNC(0x05) DEFINE_CMPSS_FUNC(0x06) DEFINE_CMPSS_FUNC(0x07)
DEFINE_CMPSS_FUNC(0x08) DEFINE_CMPSS_FUNC(0x09) DEFINE_CMPSS_FUNC(0x0A) DEFINE_CMPSS_FUNC(0x0B)
DEFINE_CMPSS_FUNC(0x0C) DEFINE_CMPSS_FUNC(0x0D) DEFINE_CMPSS_FUNC(0x0E) DEFINE_CMPSS_FUNC(0x0F)
DEFINE_CMPSS_FUNC(0x10) DEFINE_CMPSS_FUNC(0x11) DEFINE_CMPSS_FUNC(0x12) DEFINE_CMPSS_FUNC(0x13)
DEFINE_CMPSS_FUNC(0x14) DEFINE_CMPSS_FUNC(0x15) DEFINE_CMPSS_FUNC(0x16) DEFINE_CMPSS_FUNC(0x17)
DEFINE_CMPSS_FUNC(0x18) DEFINE_CMPSS_FUNC(0x19) DEFINE_CMPSS_FUNC(0x1A) DEFINE_CMPSS_FUNC(0x1B)
DEFINE_CMPSS_FUNC(0x1C) DEFINE_CMPSS_FUNC(0x1D) DEFINE_CMPSS_FUNC(0x1E) DEFINE_CMPSS_FUNC(0x1F)

// 函数指针类型和数组
typedef void (*vcmpss_func_t)(__m128*, __m128, __m128);
static vcmpss_func_t vcmpss_funcs[32] = {
    vcmpss_0x00, vcmpss_0x01, vcmpss_0x02, vcmpss_0x03,
    vcmpss_0x04, vcmpss_0x05, vcmpss_0x06, vcmpss_0x07,
    vcmpss_0x08, vcmpss_0x09, vcmpss_0x0A, vcmpss_0x0B,
    vcmpss_0x0C, vcmpss_0x0D, vcmpss_0x0E, vcmpss_0x0F,
    vcmpss_0x10, vcmpss_0x11, vcmpss_0x12, vcmpss_0x13,
    vcmpss_0x14, vcmpss_0x15, vcmpss_0x16, vcmpss_0x17,
    vcmpss_0x18, vcmpss_0x19, vcmpss_0x1A, vcmpss_0x1B,
    vcmpss_0x1C, vcmpss_0x1D, vcmpss_0x1E, vcmpss_0x1F
};

// ========== 内存操作数支持 ==========
#define DEFINE_CMPSS_MEM_FUNC(imm) \
static void vcmpss_mem_##imm(__m128 *dst, __m128 a, float *mem) { \
    asm volatile("vcmpss $"#imm", %2, %1, %0" \
                 : "=x"(*dst) \
                 : "x"(a), "m"(*mem)); \
}

// 为所有32种条件定义内存操作数函数
DEFINE_CMPSS_MEM_FUNC(0x00) DEFINE_CMPSS_MEM_FUNC(0x01) DEFINE_CMPSS_MEM_FUNC(0x02) DEFINE_CMPSS_MEM_FUNC(0x03)
DEFINE_CMPSS_MEM_FUNC(0x04) DEFINE_CMPSS_MEM_FUNC(0x05) DEFINE_CMPSS_MEM_FUNC(0x06) DEFINE_CMPSS_MEM_FUNC(0x07)
DEFINE_CMPSS_MEM_FUNC(0x08) DEFINE_CMPSS_MEM_FUNC(0x09) DEFINE_CMPSS_MEM_FUNC(0x0A) DEFINE_CMPSS_MEM_FUNC(0x0B)
DEFINE_CMPSS_MEM_FUNC(0x0C) DEFINE_CMPSS_MEM_FUNC(0x0D) DEFINE_CMPSS_MEM_FUNC(0x0E) DEFINE_CMPSS_MEM_FUNC(0x0F)
DEFINE_CMPSS_MEM_FUNC(0x10) DEFINE_CMPSS_MEM_FUNC(0x11) DEFINE_CMPSS_MEM_FUNC(0x12) DEFINE_CMPSS_MEM_FUNC(0x13)
DEFINE_CMPSS_MEM_FUNC(0x14) DEFINE_CMPSS_MEM_FUNC(0x15) DEFINE_CMPSS_MEM_FUNC(0x16) DEFINE_CMPSS_MEM_FUNC(0x17)
DEFINE_CMPSS_MEM_FUNC(0x18) DEFINE_CMPSS_MEM_FUNC(0x19) DEFINE_CMPSS_MEM_FUNC(0x1A) DEFINE_CMPSS_MEM_FUNC(0x1B)
DEFINE_CMPSS_MEM_FUNC(0x1C) DEFINE_CMPSS_MEM_FUNC(0x1D) DEFINE_CMPSS_MEM_FUNC(0x1E) DEFINE_CMPSS_MEM_FUNC(0x1F)

// 内存操作数函数指针类型
typedef void (*vcmpss_mem_func_t)(__m128*, __m128, float*);
static vcmpss_mem_func_t vcmpss_mem_funcs[32] = {
    vcmpss_mem_0x00, vcmpss_mem_0x01, vcmpss_mem_0x02, vcmpss_mem_0x03,
    vcmpss_mem_0x04, vcmpss_mem_0x05, vcmpss_mem_0x06, vcmpss_mem_0x07,
    vcmpss_mem_0x08, vcmpss_mem_0x09, vcmpss_mem_0x0A, vcmpss_mem_0x0B,
    vcmpss_mem_0x0C, vcmpss_mem_0x0D, vcmpss_mem_0x0E, vcmpss_mem_0x0F,
    vcmpss_mem_0x10, vcmpss_mem_0x11, vcmpss_mem_0x12, vcmpss_mem_0x13,
    vcmpss_mem_0x14, vcmpss_mem_0x15, vcmpss_mem_0x16, vcmpss_mem_0x17,
    vcmpss_mem_0x18, vcmpss_mem_0x19, vcmpss_mem_0x1A, vcmpss_mem_0x1B,
    vcmpss_mem_0x1C, vcmpss_mem_0x1D, vcmpss_mem_0x1E, vcmpss_mem_0x1F
};

// 测试用例类型枚举
typedef enum {
    TEST_REG,       // 寄存器操作数
    TEST_MEM        // 内存操作数
} test_type_t;

// 测试函数
static int test_case(const char *name, float a, float b, int imm, uint32_t expected, 
                     test_type_t test_type) {
    __m128 res = _mm_setzero_ps();
    __m128 a_reg = _mm_set_ss(a);
    uint32_t got;
    
    if (test_type == TEST_REG) {
        __m128 b_reg = _mm_set_ss(b);
        vcmpss_funcs[imm](&res, a_reg, b_reg);
    } else { // TEST_MEM
        vcmpss_mem_funcs[imm](&res, a_reg, &b);
    }
    
    // 提取结果
    got = _mm_extract_ps(res, 0);
    
    int ret = (got == expected);
    
    if (!ret) {
        printf("❌ FAIL: %s\n", name);
        printf("  A: %f (0x%08X)\n", a, *(uint32_t*)&a);
        printf("  B: %f (0x%08X)\n", b, *(uint32_t*)&b);
        printf("  Expected: 0x%08X\n", expected);
        printf("  Got:      0x%08X\n", got);
    } else {
        printf("✅ PASS: %s\n", name);
    }
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
    
    // 测试所有32种比较条件
    for (int imm = 0; imm < 32; imm++) {
        char name[64];
        snprintf(name, sizeof(name), "vcmpss imm=0x%02X", imm);
        
        // 基础测试值
        float a = 1.0f;
        float b = 2.0f;
        uint32_t expected = 0;
        
        // 根据比较条件设置预期结果
        int unordered = isnan(a) || isnan(b);
        int ordered = !unordered;
        int equal = (a == b);
        int less = (a < b);
        int greater = (a > b);
        
        switch(imm) {
            case 0x00: expected = equal ? 0xFFFFFFFF : 0; break;
            case 0x01: expected = (less && ordered) ? 0xFFFFFFFF : 0; break;
            case 0x02: expected = ((less || equal) && ordered) ? 0xFFFFFFFF : 0; break;
            case 0x03: expected = unordered ? 0xFFFFFFFF : 0; break;
            case 0x04: expected = !equal ? 0xFFFFFFFF : 0; break;
            case 0x05: expected = (!less || unordered) ? 0xFFFFFFFF : 0; break;
            case 0x06: expected = (!(less || equal) || unordered) ? 0xFFFFFFFF : 0; break;
            case 0x07: expected = ordered ? 0xFFFFFFFF : 0; break;
            case 0x08: expected = (equal || unordered) ? 0xFFFFFFFF : 0; break;
            case 0x09: expected = (!(greater || equal) || unordered) ? 0xFFFFFFFF : 0; break;
            case 0x0A: expected = (!greater || unordered) ? 0xFFFFFFFF : 0; break;
            case 0x0B: expected = 0; break;
            case 0x0C: expected = (!equal && ordered) ? 0xFFFFFFFF : 0; break;
            case 0x0D: expected = ((greater || equal) && ordered) ? 0xFFFFFFFF : 0; break;
            case 0x0E: expected = (greater && ordered) ? 0xFFFFFFFF : 0; break;
            case 0x0F: expected = 0xFFFFFFFF; break;
            case 0x10: expected = (equal && ordered) ? 0xFFFFFFFF : 0; break;
            case 0x11: expected = (less && ordered) ? 0xFFFFFFFF : 0; break;
            case 0x12: expected = ((less || equal) && ordered) ? 0xFFFFFFFF : 0; break;
            case 0x13: expected = unordered ? 0xFFFFFFFF : 0; break;
            case 0x14: expected = (!equal || unordered) ? 0xFFFFFFFF : 0; break;
            case 0x15: expected = (!less || unordered) ? 0xFFFFFFFF : 0; break;
            case 0x16: expected = (!(less || equal) || unordered) ? 0xFFFFFFFF : 0; break;
            case 0x17: expected = ordered ? 0xFFFFFFFF : 0; break;
            case 0x18: expected = (equal || unordered) ? 0xFFFFFFFF : 0; break;
            case 0x19: expected = (!(greater || equal) || unordered) ? 0xFFFFFFFF : 0; break;
            case 0x1A: expected = (!greater || unordered) ? 0xFFFFFFFF : 0; break;
            case 0x1B: expected = 0; break;
            case 0x1C: expected = (!equal && ordered) ? 0xFFFFFFFF : 0; break;
            case 0x1D: expected = ((greater || equal) && ordered) ? 0xFFFFFFFF : 0; break;
            case 0x1E: expected = (greater && ordered) ? 0xFFFFFFFF : 0; break;
            case 0x1F: expected = 0xFFFFFFFF; break;
        }
        
        passed += test_case(name, a, b, imm, expected, TEST_REG);
        total++;
    }
    
    // 特殊值测试
    float boundary_values[] = {
        0.0f, -0.0f, INFINITY, -INFINITY, 
        NAN, 1.0f, -1.0f, 2.0f
    };
    
    // 生成所有可能的双操作数组合 (8x8=64种组合)
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            float a = boundary_values[i];
            float b = boundary_values[j];
            
            // 测试所有32种条件
            for (int imm = 0; imm < 32; imm++) {
                char name[128];
                snprintf(name, sizeof(name), "vcmpss BOUNDARY [%d:%d] imm=0x%02X", 
                         i, j, imm);
                
                // 计算预期结果
                uint32_t expected = 0;
                
                int unordered = isnan(a) || isnan(b);
                int ordered = !unordered;
                int equal = (a == b);
                int less = (a < b);
                int greater = (a > b);
                
                switch(imm) {
                    case 0x00: expected = equal ? 0xFFFFFFFF : 0; break;
                    case 0x01: expected = (less && ordered) ? 0xFFFFFFFF : 0; break;
                    case 0x02: expected = ((less || equal) && ordered) ? 0xFFFFFFFF : 0; break;
                    case 0x03: expected = unordered ? 0xFFFFFFFF : 0; break;
                    case 0x04: expected = !equal ? 0xFFFFFFFF : 0; break;
                    case 0x05: expected = (!less || unordered) ? 0xFFFFFFFF : 0; break;
                    case 0x06: expected = (!(less || equal) || unordered) ? 0xFFFFFFFF : 0; break;
                    case 0x07: expected = ordered ? 0xFFFFFFFF : 0; break;
                    case 0x08: expected = (equal || unordered) ? 0xFFFFFFFF : 0; break;
                    case 0x09: expected = (!(greater || equal) || unordered) ? 0xFFFFFFFF : 0; break;
                    case 0x0A: expected = (!greater || unordered) ? 0xFFFFFFFF : 0; break;
                    case 0x0B: expected = 0; break;
                    case 0x0C: expected = (!equal && ordered) ? 0xFFFFFFFF : 0; break;
                    case 0x0D: expected = ((greater || equal) && ordered) ? 0xFFFFFFFF : 0; break;
                    case 0x0E: expected = (greater && ordered) ? 0xFFFFFFFF : 0; break;
                    case 0x0F: expected = 0xFFFFFFFF; break;
                    case 0x10: expected = (equal && ordered) ? 0xFFFFFFFF : 0; break;
                    case 0x11: expected = (less && ordered) ? 0xFFFFFFFF : 0; break;
                    case 0x12: expected = ((less || equal) && ordered) ? 0xFFFFFFFF : 0; break;
                    case 0x13: expected = unordered ? 0xFFFFFFFF : 0; break;
                    case 0x14: expected = (!equal || unordered) ? 0xFFFFFFFF : 0; break;
                    case 0x15: expected = (!less || unordered) ? 0xFFFFFFFF : 0; break;
                    case 0x16: expected = (!(less || equal) || unordered) ? 0xFFFFFFFF : 0; break;
                    case 0x17: expected = ordered ? 0xFFFFFFFF : 0; break;
                    case 0x18: expected = (equal || unordered) ? 0xFFFFFFFF : 0; break;
                    case 0x19: expected = (!(greater || equal) || unordered) ? 0xFFFFFFFF : 0; break;
                    case 0x1A: expected = (!greater || unordered) ? 0xFFFFFFFF : 0; break;
                    case 0x1B: expected = 0; break;
                    case 0x1C: expected = (!equal && ordered) ? 0xFFFFFFFF : 0; break;
                    case 0x1D: expected = ((greater || equal) && ordered) ? 0xFFFFFFFF : 0; break;
                    case 0x1E: expected = (greater && ordered) ? 0xFFFFFFFF : 0; break;
                    case 0x1F: expected = 0xFFFFFFFF; break;
                }
                
                passed += test_case(name, a, b, imm, expected, TEST_REG);
                total++;
            }
        }
    }
    
    // 内存操作数测试
    printf("\n=== Testing memory operands ===\n");
    float a_val = 1.0f;
    float b_val = 1.0f;
    passed += test_case("vcmpss mem operand EQ", a_val, b_val, 0x00, 0xFFFFFFFF, TEST_MEM);
    total++;
    
    b_val = 2.0f;
    passed += test_case("vcmpss mem operand NEQ", a_val, b_val, 0x04, 0xFFFFFFFF, TEST_MEM);
    total++;

    // 特殊值测试
    printf("\n=== Testing special values ===\n");
    // 零值测试
    passed += test_case("vcmpss +0.0 == -0.0", zero, neg_zero, 0x00, 0xFFFFFFFF, TEST_REG);
    total++;
    passed += test_case("vcmpss -0.0 == +0.0", neg_zero, zero, 0x00, 0xFFFFFFFF, TEST_REG);
    total++;
    
    // NaN测试
    passed += test_case("vcmpss NaN == NaN", nan, nan, 0x00, 0, TEST_REG);
    total++;
    passed += test_case("vcmpss NaN != NaN", nan, nan, 0x04, 0xFFFFFFFF, TEST_REG);
    total++;
    
    // 无穷大测试
    passed += test_case("vcmpss inf > neg_inf", inf, neg_inf, 0x1E, 0xFFFFFFFF, TEST_REG);
    total++;
    passed += test_case("vcmpss neg_inf < inf", neg_inf, inf, 0x01, 0xFFFFFFFF, TEST_REG);
    total++;

    // 测试总结
    printf("\n\n=== TEST SUMMARY ===\n");
    printf("Total tests: %d\n", total);
    printf("Passed tests: %d\n", passed);
    printf("Failed tests: %d\n", total - passed);
    printf("====================\n");
    
    return passed == total ? 0 : 1;
}
