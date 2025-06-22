#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <math.h>

// 使用宏定义实现所有32种比较条件
#define DEFINE_CMP_FUNC(imm) \
static void vcmpsd_##imm(__m128d *dst, __m128d a, __m128d b) { \
    asm volatile("vcmpsd $"#imm", %2, %1, %0" \
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
typedef void (*vcmpsd_func_t)(__m128d*, __m128d, __m128d);

// 函数指针数组
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

// 统一的调用函数
static void vcmpsd(__m128d *dst, __m128d a, __m128d b, int imm) {
    if (imm >= 0 && imm < 32) {
        vcmpsd_funcs[imm](dst, a, b);
    } else {
        *dst = _mm_setzero_pd();
    }
}

// 测试单个比较操作
static int test_case(const char *name, __m128d a, __m128d b, int imm, __m128d expected) {
    __m128d res = _mm_setzero_pd();
    vcmpsd(&res, a, b, imm);
    
    // 打印输入和输出
    printf("\nTest: %s (imm=0x%02X)\n", name, imm);
    print_double_vec_hex("Operand A", (double*)&a, 2);
    print_double_vec_hex("Operand B", (double*)&b, 2);
    print_double_vec_hex("Expected", (double*)&expected, 2);
    print_double_vec_hex("Result  ", (double*)&res, 2);
    
    // 比较结果 (只比较低64位)
    uint64_t res_low = *((uint64_t*)&res);
    uint64_t exp_low = *((uint64_t*)&expected);
    uint64_t res_high = *((uint64_t*)&res + 1);
    uint64_t a_high = *((uint64_t*)&a + 1);
    
    int ret = (res_low == exp_low) && (res_high == a_high);
    printf("Result: %s\n", ret ? "PASS" : "FAIL");
    return ret;
}

int main() {
    int total = 0, passed = 0;
    
    // 定义测试值
    double nan = NAN;
    double inf = INFINITY;
    double zero = 0.0;
    double neg_zero = -0.0;
    
    // 特殊值测试向量
    __m128d special_a = _mm_setr_pd(zero, nan);
    __m128d special_b = _mm_setr_pd(neg_zero, nan);
    
    // 测试特殊值: NaN、零值
    uint64_t mask = 0xFFFFFFFFFFFFFFFF; // 全1掩码
    __m128d expected = _mm_setr_pd(*(double*)&mask, special_a[1]);
    passed += test_case("vcmpsd zero values", 
                       special_a, special_b, 0x00,
                       expected); // 0.0 == -0.0 应为真
    total++;
    
    // 测试NaN比较
    passed += test_case("vcmpsd NaN comparison", 
                       _mm_setr_pd(nan, 1.0),
                       _mm_setr_pd(nan, 2.0),
                       0x00,
                       _mm_setr_pd(0.0, 0.0)); // NaN比较不应相等
    total++;
    
    // 测试无穷大比较
    passed += test_case("vcmpsd inf comparison", 
                       _mm_setr_pd(INFINITY, 1.0),
                       _mm_setr_pd(INFINITY, 2.0),
                       0x00,
                       _mm_setr_pd(*(double*)&mask, 0.0)); // 无穷大相等
    total++;
    
    // 测试负无穷大比较
    passed += test_case("vcmpsd neg_inf comparison", 
                       _mm_setr_pd(-INFINITY, 1.0),
                       _mm_setr_pd(-INFINITY, 2.0),
                       0x00,
                       _mm_setr_pd(*(double*)&mask, 0.0)); // 负无穷大相等
    total++;
    
    // 测试零和NaN比较
    passed += test_case("vcmpsd zero vs NaN", 
                       _mm_setr_pd(0.0, 1.0),
                       _mm_setr_pd(nan, 2.0),
                       0x03, // UNORD
                       _mm_setr_pd(*(double*)&mask, 0.0)); // 应为真
    total++;
    
    // 测试所有32种比较条件
    for (int imm = 0; imm < 32; imm++) {
        char name[64];
        snprintf(name, sizeof(name), "vcmpsd imm=0x%02X", imm);
        
        // 设置测试值
        __m128d a = _mm_setr_pd(1.0, 2.0); // 高位保留测试值
        __m128d b = _mm_setr_pd(3.0, 1.0);
        
        // 计算预期结果
        double ai = a[0];
        double bi = b[0];
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
        uint64_t mask = cmp_result ? 0xFFFFFFFFFFFFFFFF : 0x0;
        __m128d expected = _mm_setr_pd(*(double*)&mask, a[1]);
        
        passed += test_case(name, a, b, imm, expected);
        total++;
    }
    
    // 特殊值测试：NaN、零值
    passed += test_case("vcmpsd NaN special", 
                       _mm_setr_pd(nan, 1.0),
                       _mm_setr_pd(nan, 2.0),
                       0x00,
                       _mm_setr_pd(0.0, 0.0)); // NaN比较不应相等
    total++;
    
    // 测试总结
    printf("\nSummary: %d/%d tests passed\n", passed, total);
    return passed == total ? 0 : 1;
}
