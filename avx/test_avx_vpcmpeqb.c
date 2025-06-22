#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

// 打印 128 位向量中的字节值
static void print_vector_128b(const char *name, __m128i vec) {
    uint8_t buffer[16];
    _mm_storeu_si128((__m128i_u*)buffer, vec);
    printf("%s: ", name);
    for (int i = 0; i < 16; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

// 打印 256 位向量中的字节值
static void print_vector_256b(const char *name, __m256i vec) {
    uint8_t buffer[32];
    _mm256_storeu_si256((__m256i_u*)buffer, vec);
    printf("%s: ", name);
    for (int i = 0; i < 32; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

// 测试 128 位寄存器-寄存器操作
static void test_rr_128(const char *name, __m128i a, __m128i b) {
    __m128i result;
    asm volatile(
        "vpcmpeqb %2, %1, %0"
        : "=x"(result)
        : "x"(a), "x"(b)
    );
    
    printf("\nTest (128-bit RR): %s\n", name);
    print_vector_128b("Operand A", a);
    print_vector_128b("Operand B", b);
    print_vector_128b("Result   ", result);
}

// 测试 128 位寄存器-内存操作
static void test_rm_128(const char *name, __m128i a, const __m128i *b) {
    __m128i result;
    asm volatile(
        "vpcmpeqb %2, %1, %0"
        : "=x"(result)
        : "x"(a), "m"(*b)
    );
    
    printf("\nTest (128-bit RM): %s\n", name);
    print_vector_128b("Operand A", a);
    print_vector_128b("Operand B", *b);
    print_vector_128b("Result   ", result);
}

// 测试 256 位寄存器-寄存器操作
static void test_rr_256(const char *name, __m256i a, __m256i b) {
    __m256i result;
    asm volatile(
        "vpcmpeqb %2, %1, %0"
        : "=x"(result)
        : "x"(a), "x"(b)
    );
    
    printf("\nTest (256-bit RR): %s\n", name);
    print_vector_256b("Operand A", a);
    print_vector_256b("Operand B", b);
    print_vector_256b("Result   ", result);
}

// 测试 256 位寄存器-内存操作
static void test_rm_256(const char *name, __m256i a, const __m256i *b) {
    __m256i result;
    asm volatile(
        "vpcmpeqb %2, %1, %0"
        : "=x"(result)
        : "x"(a), "m"(*b)
    );
    
    printf("\nTest (256-bit RM): %s\n", name);
    print_vector_256b("Operand A", a);
    print_vector_256b("Operand B", *b);
    print_vector_256b("Result   ", result);
}

int main() {
    // 128 位测试数据
    ALIGNED(16) uint8_t data1_128[16] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };
    
    ALIGNED(16) uint8_t data2_128[16] = {
        0x00, 0x10, 0x22, 0x30, 0x44, 0x50, 0x66, 0x70,
        0x88, 0x90, 0xAA, 0xB0, 0xCC, 0xD0, 0xEE, 0xF0
    };
    
    ALIGNED(16) uint8_t data3_128[16] = {
        0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
        0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00
    };
    
    // 256 位测试数据
    ALIGNED(32) uint8_t data1_256[32] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x10, 0x21, 0x32, 0x43, 0x54, 0x65, 0x76, 0x87,
        0x98, 0xA9, 0xBA, 0xCB, 0xDC, 0xED, 0xFE, 0x0F
    };
    
    ALIGNED(32) uint8_t data2_256[32] = {
        0x00, 0x10, 0x22, 0x30, 0x44, 0x50, 0x66, 0x70,
        0x88, 0x90, 0xAA, 0xB0, 0xCC, 0xD0, 0xEE, 0xF0,
        0x10, 0x20, 0x32, 0x40, 0x54, 0x60, 0x76, 0x80,
        0x98, 0xA0, 0xBA, 0xC0, 0xDC, 0xE0, 0xFE, 0x00
    };
    
    ALIGNED(32) uint8_t data3_256[32] = {
        0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
        0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,
        0x0F, 0xFE, 0xED, 0xDC, 0xCB, 0xBA, 0xA9, 0x98,
        0x87, 0x76, 0x65, 0x54, 0x43, 0x32, 0x21, 0x10
    };

    // 加载测试数据到向量寄存器
    __m128i vec1_128 = _mm_load_si128((const __m128i*)data1_128);
    __m128i vec2_128 = _mm_load_si128((const __m128i*)data2_128);
    __m128i vec3_128 = _mm_load_si128((const __m128i*)data3_128);
    
    __m256i vec1_256 = _mm256_load_si256((const __m256i*)data1_256);
    __m256i vec2_256 = _mm256_load_si256((const __m256i*)data2_256);
    __m256i vec3_256 = _mm256_load_si256((const __m256i*)data3_256);

    // 运行测试
    printf("Starting VPCMPEQB tests...\n");
    
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
    
    printf("All VPCMPEQB tests completed.\n");
    return 0;
}
