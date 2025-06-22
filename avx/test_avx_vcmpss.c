#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 使用宏定义实现所有32种比较条件
#define DEFINE_CMP_FUNC(imm) \
static void vcmpss_##imm(__m128 *dst, __m128 a, __m128 b) { \
    asm volatile("vcmpss $"#imm", %2, %1, %0" \
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
typedef void (*vcmpss_func_t)(__m128*, __m128, __m128);

// 函数指针数组
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

// 统一的调用函数
static void vcmpss(__m128 *dst, __m128 a, __m128 b, int imm) {
    if (imm >= 0 && imm < 32) {
        vcmpss_funcs[imm](dst, a, b);
    } else {
        *dst = _mm_setzero_ps();
    }
}

// 测试单个比较操作
static int test_case(const char *name, __m128 a, __m128 b, int imm, __m128 expected) {
    __m128 res = _mm_setzero_ps();
    vcmpss(&res, a, b, imm);
    
    // 打印输入和输出
    printf("\nTest: %s (imm=0x%02X)\n", name, imm);
    print_hex_float_vec("Operand A", (float*)&a, 4);
    print_hex_float_vec("Operand B", (float*)&b, 4);
    print_hex_float_vec("Expected", (float*)&expected, 4);
    print_hex_float_vec("Result  ", (float*)&res, 4);
    
    // 比较结果 (只比较低32位)
    uint32_t res_low = *((uint32_t*)&res);
    uint32_t exp_low = *((uint32_t*)&expected);
    uint32_t a_high = *((uint32_t*)&a + 1); // 高位应保留a的值
    
    int ret = (res_low == exp_low) && 
              (*((uint32_t*)&res + 1) == *((uint32_t*)&a + 1)) && 
              (*((uint32_t*)&res + 2) == *((uint32_t*)&a + 2)) && 
              (*((uint32_t*)&res + 3) == *((uint32_t*)&a + 3));
    
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
    
    // 特殊值测试向量
    __m128 special_a = _mm_setr_ps(zero, nan, inf, neg_inf);
    __m128 special_b = _mm_setr_ps(neg_zero, nan, inf, neg_inf);
    
    // 测试特殊值: NaN、零值
    uint32_t mask = 0xFFFFFFFF; // 全1掩码
    __m128 expected = _mm_setr_ps(*(float*)&mask, nan, inf, neg_inf);
    passed += test_case("vcmpss zero values", 
                       special_a, special_b, 0x00,
                       expected); // 0.0 == -0.0 应为真
    total++;
    
    // 测试NaN比较
    passed += test_case("vcmpss NaN comparison", 
                       _mm_setr_ps(nan, 1.0f, 2.0f, 3.0f),
                       _mm_setr_ps(nan, 2.0f, 3.0f, 4.0f),
                       0x00,
                       _mm_setr_ps(0.0f, 1.0f, 2.0f, 3.0f)); // NaN比较不应相等
    total++;
    
    // 测试无穷大比较
    passed += test_case("vcmpss inf comparison", 
                       _mm_setr_ps(inf, 1.0f, 2.0f, 3.0f),
                       _mm_setr_ps(inf, 2.0f, 3.0f, 4.0f),
                       0x00,
                       _mm_setr_ps(*(float*)&mask, 1.0f, 2.0f, 3.0f)); // 无穷大相等
    total++;
    
    // 测试负无穷大比较
    passed += test_case("vcmpss neg_inf comparison", 
                       _mm_setr_ps(neg_inf, 1.0f, 2.0f, 3.0f),
                       _mm_setr_ps(neg_inf, 2.0f, 3.0f, 4.0f),
                       0x00,
                       _mm_setr_ps(*(float*)&mask, 1.0f, 2.0f, 3.0f)); // 负无穷大相等
    total++;
    
    // 测试零和NaN比较
    passed += test_case("vcmpss zero vs NaN", 
                       _mm_setr_ps(0.0f, 1.0f, 2.0f, 3.0f),
                       _mm_setr_ps(nan, 2.0f, 3.0f, 4.0f),
                       0x03, // UNORD
                       _mm_setr_ps(*(float*)&mask, 1.0f, 2.0f, 3.0f)); // 应为真
    total++;
    
    // 测试所有32种比较条件
    for (int imm = 0; imm < 32; imm++) {
        char name[64];
        snprintf(name, sizeof(name), "vcmpss imm=0x%02X", imm);
        
        // 设置测试值
        __m128 a = _mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f); // 高位保留测试值
        __m128 b = _mm_setr_ps(3.0f, 1.0f, 2.0f, 4.0f);
        
        // 计算预期结果
        float ai = ((float*)&a)[0];
        float bi = ((float*)&b)[0];
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
        
        // 构建预期结果：低位为掩码，高位保留a的高位
        uint32_t mask = cmp_result ? 0xFFFFFFFF : 0x0;
        __m128 expected = _mm_setr_ps(*(float*)&mask, 
                                      ((float*)&a)[1], 
                                      ((float*)&a)[2], 
                                      ((float*)&a)[3]);
        
        passed += test_case(name, a, b, imm, expected);
        total++;
    }
    
    // 内存操作数测试
    ALIGNED(16) float mem_ops[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    __m128 a_mem = _mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f);
    __m128 res_mem = _mm_setzero_ps();
    
    asm volatile(
        "vcmpss $0, (%1), %2, %0"
        : "=x"(res_mem)
        : "r"(mem_ops), "x"(a_mem)
    );
    
    // 构建预期结果
    uint32_t mem_mask = (1.0f == mem_ops[0]) ? 0xFFFFFFFF : 0x0;
    __m128 exp_mem = _mm_setr_ps(*(float*)&mem_mask, 
                                ((float*)&a_mem)[1], 
                                ((float*)&a_mem)[2], 
                                ((float*)&a_mem)[3]);
    
    printf("\nTest: vcmpss with memory operand\n");
    print_hex_float_vec("Operand A", (float*)&a_mem, 4);
    print_hex_float_vec("Operand B (mem)", mem_ops, 4);
    print_hex_float_vec("Expected", (float*)&exp_mem, 4);
    print_hex_float_vec("Result  ", (float*)&res_mem, 4);
    
    uint32_t mem_res_low = *((uint32_t*)&res_mem);
    int mem_result = (mem_res_low == *(uint32_t*)&mem_mask) && 
                    (*((uint32_t*)&res_mem + 1) == *((uint32_t*)&a_mem + 1)) && 
                    (*((uint32_t*)&res_mem + 2) == *((uint32_t*)&a_mem + 2)) && 
                    (*((uint32_t*)&res_mem + 3) == *((uint32_t*)&a_mem + 3));
    
    printf("Result: %s\n", mem_result ? "PASS" : "FAIL");
    passed += mem_result;
    total++;
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
