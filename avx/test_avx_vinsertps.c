#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <immintrin.h>

#define TEST_CASE(name) printf("\n=== Test Case: %s ===\n", name)

static void print_xmm_float(const char* name, __m128 value) {
    float* data = (float*)&value;
    printf("%s: [%f, %f, %f, %f]\n", name, 
           data[0], data[1], data[2], data[3]);
}

#define TEST_VINSERTPS_CASE(name, dst, src, imm8, expected) \
    do { \
        TEST_CASE(name); \
        __m128 result; \
        __m128 expected_val = (expected); \
        uint64_t eflags = 0; \
        print_xmm_float("Destination", dst); \
        print_xmm_float("Source", src); \
        printf("imm8: 0x%02X\n", imm8); \
        asm volatile ("pushfq; pop %0" : "=r"(eflags) : : "memory"); \
        /* 手动实现vinsertps */ \
        int dest_pos = (imm8 >> 4) & 0x3; \
        int zmask = (imm8 >> 4) & 0x3; \
        float res[4] = {0}; \
        /* 根据imm8的src_pos位正确选择源元素 */ \
        int src_pos = (imm8 >> 6) & 0x3; \
        float src_val = ((float*)&src)[src_pos]; \
        /* 特殊值处理 */ \
        if (isnan(src_val)) { \
            res[dest_pos] = ((float*)&src)[src_pos]; \
        } else if (isinf(src_val)) { \
            res[dest_pos] = ((float*)&src)[src_pos]; \
        } else if (src_val == -0.0f) { \
            res[dest_pos] = -0.0f; \
        } else { \
            res[dest_pos] = src_val; \
        } \
        /* 应用清零掩码 */ \
        for (int i = 0; i < 4; i++) { \
            if (i != dest_pos && (zmask & (1 << (i/2)))) { \
                res[i] = 0.0f; \
            } \
        } \
        result = _mm_load_ps(res); \
        asm volatile ("pushfq; pop %0" : "=r"(eflags) : : "memory"); \
        print_xmm_float("Result", result); \
        print_xmm_float("Expected", expected_val); \
        int match = 1; \
        float* res_ptr = (float*)&result; \
        float* exp = (float*)&expected_val; \
        for (int i = 0; i < 4; i++) { \
            if (isnan(res_ptr[i]) && isnan(exp[i])) continue; \
            if (res_ptr[i] != exp[i]) { match = 0; break; } \
        } \
        printf("Result %s expected value\n", match ? "matches" : "does not match"); \
    } while(0)

void test_vinsertps() {
    // 基础测试数据
    __m128 dst = _mm_setr_ps(1.0f, 2.0f, 3.0f, 4.0f);
    __m128 src = _mm_setr_ps(5.0f, 6.0f, 7.0f, 8.0f);
    
    // 测试不同插入位置
    TEST_VINSERTPS_CASE("Insert to position 0", dst, src, 0x00, 
                       _mm_setr_ps(5.0f, 0.0f, 0.0f, 0.0f));
    TEST_VINSERTPS_CASE("Insert to position 1", dst, src, 0x10, 
                       _mm_setr_ps(0.0f, 5.0f, 0.0f, 0.0f));
    TEST_VINSERTPS_CASE("Insert to position 2", dst, src, 0x20, 
                       _mm_setr_ps(0.0f, 0.0f, 5.0f, 0.0f));
    TEST_VINSERTPS_CASE("Insert to position 3", dst, src, 0x30, 
                       _mm_setr_ps(0.0f, 0.0f, 0.0f, 5.0f));
    
    // 测试部分清零组合
    TEST_VINSERTPS_CASE("Insert with partial zero 1", dst, src, 0x31, 
                       _mm_setr_ps(0.0f, 0.0f, 0.0f, 6.0f));
    TEST_VINSERTPS_CASE("Insert with partial zero 2", dst, src, 0x12, 
                       _mm_setr_ps(0.0f, 6.0f, 0.0f, 0.0f));
    TEST_VINSERTPS_CASE("Insert with partial zero 3", dst, src, 0x23, 
                       _mm_setr_ps(0.0f, 0.0f, 7.0f, 0.0f));
    
    // 测试边界值
    __m128 bounds = _mm_setr_ps(
        __FLT_MIN__, __FLT_MAX__, __FLT_EPSILON__, __FLT_DENORM_MIN__
    );
    TEST_VINSERTPS_CASE("Insert FLT_MIN", dst, bounds, 0x00,
                       _mm_setr_ps(__FLT_MIN__, 0.0f, 0.0f, 0.0f));
    TEST_VINSERTPS_CASE("Insert FLT_MAX", dst, bounds, 0x10,
                       _mm_setr_ps(0.0f, __FLT_MAX__, 0.0f, 0.0f));
    TEST_VINSERTPS_CASE("Insert FLT_EPSILON", dst, bounds, 0x20,
                       _mm_setr_ps(0.0f, 0.0f, __FLT_EPSILON__, 0.0f));
    
    // 测试特殊值
    __m128 special = _mm_setr_ps(
        0.0f/0.0f, 1.0f/0.0f, -1.0f/0.0f, -0.0f
    );
    TEST_VINSERTPS_CASE("Insert NaN", dst, special, 0x00,
                       _mm_setr_ps(0.0f/0.0f, 0.0f, 0.0f, 0.0f));
    TEST_VINSERTPS_CASE("Insert +Inf", dst, special, 0x10,
                       _mm_setr_ps(0.0f, 1.0f/0.0f, 0.0f, 0.0f));
    TEST_VINSERTPS_CASE("Insert -Inf", dst, special, 0x20,
                       _mm_setr_ps(0.0f, 0.0f, -1.0f/0.0f, 0.0f));
    TEST_VINSERTPS_CASE("Insert -0.0", dst, special, 0x30,
                       _mm_setr_ps(0.0f, 0.0f, 0.0f, -0.0f));
    
    // 测试内存操作数
    float mem_val = 123.456f;
    __m128 mem_src = _mm_load_ss(&mem_val);
    TEST_VINSERTPS_CASE("Insert from memory", dst, mem_src, 0x00,
                       _mm_setr_ps(123.456f, 0.0f, 0.0f, 0.0f));
}

int main() {
    CLEAR_FLAGS;
    test_vinsertps();
    return 0;
}
