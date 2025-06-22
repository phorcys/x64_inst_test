#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// 打印 128 位向量中的双字值
static void print_vector_128d(const char *name, __m128i vec) {
    uint32_t buffer[4];
    _mm_storeu_si128((__m128i_u*)buffer, vec);
    printf("%s: ", name);
    for (int i = 0; i < 4; i++) {
        printf("%08X ", buffer[i]);
    }
    printf("\n");
}

// 打印 256 位向量中的双字值
static void print_vector_256d(const char *name, __m256i vec) {
    uint32_t buffer[8];
    _mm256_storeu_si256((__m256i_u*)buffer, vec);
    printf("%s: ", name);
    for (int i = 0; i < 8; i++) {
        printf("%08X ", buffer[i]);
    }
    printf("\n");
}

// 测试 128 位寄存器-寄存器操作
static void test_rr_128(const char *name, __m128i a, __m128i b) {
    __m128i result;
    asm volatile(
        "vpcmpeqd %2, %1, %0"
        : "=x"(result)
        : "x"(a), "x"(b)
    );
    
    printf("\nTest (128-bit RR): %s\n", name);
    print_vector_128d("Operand A", a);
    print_vector_128d("Operand B", b);
    print_vector_128d("Result   ", result);
}

// 测试 128 位寄存器-内存操作
static void test_rm_128(const char *name, __m128i a, const __m128i *b) {
    __m128i result;
    asm volatile(
        "vpcmpeqd %2, %1, %0"
        : "=x"(result)
        : "x"(a), "m"(*b)
    );
    
    printf("\nTest (128-bit RM): %s\n", name);
    print_vector_128d("Operand A", a);
    print_vector_128d("Operand B", *b);
    print_vector_128d("Result   ", result);
}

// 测试 256 位寄存器-寄存器操作
static void test_rr_256(const char *name, __m256i a, __m256i b) {
    __m256i result;
    asm volatile(
        "vpcmpeqd %2, %1, %0"
        : "=x"(result)
        : "x"(a), "x"(b)
    );
    
    printf("\nTest (256-bit RR): %s\n", name);
    print_vector_256d("Operand A", a);
    print_vector_256d("Operand B", b);
    print_vector_256d("Result   ", result);
}

// 测试 256 位寄存器-内存操作
static void test_rm_256(const char *name, __m256i a, const __m256i *b) {
    __m256i result;
    asm volatile(
        "vpcmpeqd %2, %1, %0"
        : "=x"(result)
        : "x"(a), "m"(*b)
    );
    
    printf("\nTest (256-bit RM): %s\n", name);
    print_vector_256d("Operand A", a);
    print_vector_256d("Operand B", *b);
    print_vector_256d("Result   ", result);
}

int main() {
    // 128 位测试数据
    ALIGNED(16) uint32_t data1_128[4] = {
        0x00000000, 0x11111111, 0x22222222, 0x33333333
    };
    
    ALIGNED(16) uint32_t data2_128[4] = {
        0x00000000, 0x11111111, 0x00000000, 0x33333333
    };
    
    ALIGNED(16) uint32_t data3_128[4] = {
        0xFFFFFFFF, 0xEEEEEEEE, 0xDDDDDDDD, 0xCCCCCCCC
    };
    
    // 256 位测试数据
    ALIGNED(32) uint32_t data1_256[8] = {
        0x00000000, 0x11111111, 0x22222222, 0x33333333,
        0x44444444, 0x55555555, 0x66666666, 0x77777777
    };
    
    ALIGNED(32) uint32_t data2_256[8] = {
        0x00000000, 0x11111111, 0x00000000, 0x33333333,
        0x44444444, 0x00000000, 0x66666666, 0x00000000
    };
    
    ALIGNED(32) uint32_t data3_256[8] = {
        0xFFFFFFFF, 0xEEEEEEEE, 0xDDDDDDDD, 0xCCCCCCCC,
        0xBBBBBBBB, 0xAAAAAAAA, 0x99999999, 0x88888888
    };

    // 加载测试数据到向量寄存器
    __m128i vec1_128 = _mm_load_si128((const __m128i*)data1_128);
    __m128i vec2_128 = _mm_load_si128((const __m128i*)data2_128);
    __m128i vec3_128 = _mm_load_si128((const __m128i*)data3_128);
    
    __m256i vec1_256 = _mm256_load_si256((const __m256i*)data1_256);
    __m256i vec2_256 = _mm256_load_si256((const __m256i*)data2_256);
    __m256i vec3_256 = _mm256_load_si256((const __m256i*)data3_256);

    // 运行测试
    printf("Starting VPCMPEQD tests...\n");
    
    // 128 位测试
    test_rr_128("Equal vectors", vec1_128, vec1_128);
    test_rr_128("Partial equal", vec1_128, vec2_128);
    test_rr_128("Completely different", vec1_128, vec3_128);
    
    test_rm_128("Equal vectors (mem)", vec1_128, (const __m128i*)data1_128);
    test_rm_128("Partial equal (mem)", vec1_128, (const __m128i*)data2_128);
    test_rm_128("Completely different (mem)", vec1_128, (const __m128i*)data3_128);
    
    // 256 位测试
    test_rr_256("Equal vectors", vec1_256, vec1_256);
    test_rr_256("Partial equal", vec1_256, vec2_256);
    test_rr_256("Completely different", vec1_256, vec3_256);
    
    test_rm_256("Equal vectors (mem)", vec1_256, (const __m256i*)data1_256);
    test_rm_256("Partial equal (mem)", vec1_256, (const __m256i*)data2_256);
    test_rm_256("Completely different (mem)", vec1_256, (const __m256i*)data3_256);
    
    printf("All VPCMPEQD tests completed.\n");
    return 0;
}
