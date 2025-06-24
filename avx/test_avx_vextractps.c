#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define TEST_CASE(name) printf("\n=== Test Case: %s ===\n", name)

static void test_vextractps_case(const char* name, 
                               __m128 src,
                               int imm8,
                               float expected) {
    TEST_CASE(name);
    
    float result = 0;
    uint64_t eflags = 0;
    
    // 打印输入值
    float* src_data = (float*)&src;
    printf("Source: [");
    for (int i = 0; i < 4; i++) printf("%f ", src_data[i]);
    printf("], imm8=%d\n", imm8);
    
    // 获取EFLAGS初始状态
    asm volatile ("pushfq; pop %0" : "=r"(eflags) : : "memory");
    
    // 执行VEXTRACTPS指令
    switch(imm8) {
        case 0:
            asm volatile ("vextractps $0, %1, %0" : "=m"(result) : "x"(src) : "memory");
            break;
        case 1:
            asm volatile ("vextractps $1, %1, %0" : "=m"(result) : "x"(src) : "memory");
            break;
        case 2:
            asm volatile ("vextractps $2, %1, %0" : "=m"(result) : "x"(src) : "memory");
            break;
        case 3:
            asm volatile ("vextractps $3, %1, %0" : "=m"(result) : "x"(src) : "memory");
            break;
        default:
            printf("Invalid imm8 value: %d\n", imm8);
            return;
    }
    
    // 获取EFLAGS最终状态
    asm volatile ("pushfq; pop %0" : "=r"(eflags) : : "memory");
    
    // 打印结果
    printf("Result: %f, Expected: %f\n", result, expected);
    
    // 特殊处理NaN比较
    int match;
    if (isnan(result) && isnan(expected)) {
        match = 1;
    } else {
        match = result == expected;
    }
    printf("Result %s expected value\n", 
           match ? "matches" : "does not match");
    
    // 打印EFLAGS状态
    printf("EFLAGS after operation:\n");
    print_eflags_cond(eflags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
}

void test_vextractps() {
    // 测试各种浮点值
    __m128 values = _mm_setr_ps(
        1.0f, -2.5f, 0.0f, 3.1415926f
    );
    
    // 测试各种imm8值
    test_vextractps_case("Extract position 0", values, 0, 1.0f);
    test_vextractps_case("Extract position 1", values, 1, -2.5f);
    test_vextractps_case("Extract position 2", values, 2, 0.0f);
    test_vextractps_case("Extract position 3", values, 3, 3.1415926f);
    
    // 测试边界值
    __m128 bounds = _mm_setr_ps(
        __FLT_MIN__, __FLT_MAX__, __FLT_EPSILON__, __FLT_DENORM_MIN__
    );
    test_vextractps_case("Extract FLT_MIN", bounds, 0, __FLT_MIN__);
    test_vextractps_case("Extract FLT_MAX", bounds, 1, __FLT_MAX__);
    test_vextractps_case("Extract FLT_EPSILON", bounds, 2, __FLT_EPSILON__);
    test_vextractps_case("Extract FLT_DENORM_MIN", bounds, 3, __FLT_DENORM_MIN__);
    
    // 测试特殊值
    __m128 special = _mm_setr_ps(
        0.0f/0.0f, 1.0f/0.0f, -1.0f/0.0f, -0.0f
    );
    test_vextractps_case("Extract NaN", special, 0, 0.0f/0.0f);
    test_vextractps_case("Extract +Inf", special, 1, 1.0f/0.0f);
    test_vextractps_case("Extract -Inf", special, 2, -1.0f/0.0f);
    test_vextractps_case("Extract -0.0", special, 3, -0.0f);
}

int main() {
    CLEAR_FLAGS;
    test_vextractps();
    return 0;
}
