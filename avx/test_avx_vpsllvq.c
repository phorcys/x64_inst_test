#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// VPSLLVQ指令测试
void test_vpsllvq() {
    printf("=== Testing VPSLLVQ (Variable Shift Left Logical Quadword) ===\n");
    
    // 测试数据
    uint64_t src_data[8] = {
        0x0123456789ABCDEF, 0xFEDCBA9876543210,
        0x5555555555555555, 0xAAAAAAAAAAAAAAAA,
        0x7FFFFFFFFFFFFFFF, 0x8000000000000000,
        0x0000000000000001, 0xFFFFFFFFFFFFFFFF
    };
    
    // 移位量数据
    uint64_t shift_data[8] = {
        0, 1, 63, 64, // 边界值
        32, 16, 65, 0  // 其他值
    };
    
    // 128位测试
    printf("\n--- 128-bit Tests ---\n");
    __m128i src128, shift128, dst128;
    uint64_t expected128[2];
    
    // 加载128位源数据和移位量
    src128 = _mm_loadu_si128((__m128i*)src_data);
    shift128 = _mm_loadu_si128((__m128i*)shift_data);
    
    // 测试1: 寄存器到寄存器
    asm volatile ("vpsllvq %1, %2, %0" : "=x"(dst128) : "x"(shift128), "x"(src128));
    printf("Test 1: VPSLLVQ xmm, xmm, xmm\n");
    print_xmm("Source", src128);
    print_xmm("Shift Counts", shift128);
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 2; i++) {
        uint64_t shift = shift_data[i];
        if (shift >= 64) {
            expected128[i] = 0;
        } else {
            expected128[i] = src_data[i] << shift;
        }
    }
    __m128i exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 256位测试
    printf("\n--- 256-bit Tests ---\n");
    __m256i src256, shift256, dst256;
    uint64_t expected256[4];
    
    // 加载256位源数据和移位量
    src256 = _mm256_loadu_si256((__m256i*)src_data);
    shift256 = _mm256_loadu_si256((__m256i*)shift_data);
    
    // 测试2: 寄存器到寄存器
    asm volatile ("vpsllvq %1, %2, %0" : "=x"(dst256) : "x"(shift256), "x"(src256));
    printf("\nTest 2: VPSLLVQ ymm, ymm, ymm\n");
    print_ymm("Source", src256);
    print_ymm("Shift Counts", shift256);
    print_ymm("Result", dst256);
    
    // 计算预期值
    for (int i = 0; i < 4; i++) {
        uint64_t shift = shift_data[i];
        if (shift >= 64) {
            expected256[i] = 0;
        } else {
            expected256[i] = src_data[i] << shift;
        }
    }
    __m256i exp256 = _mm256_loadu_si256((__m256i*)expected256);
    printf("Comparison: %s\n", cmp_ymm(dst256, exp256) ? "PASS" : "FAIL");
    
    // 测试3: 内存操作数
    __m128i dst128_mem;
    asm volatile ("vpsllvq %1, %2, %0" : "=x"(dst128_mem) : "m"(shift_data), "x"(src128));
    printf("\nTest 3: VPSLLVQ xmm, xmm, [mem]\n");
    print_xmm("Result (mem)", dst128_mem);
    printf("Comparison: %s\n", cmp_xmm(dst128_mem, exp128) ? "PASS" : "FAIL");
    
    // 边界测试
    printf("\n--- Boundary Tests ---\n");
    
    // 测试4: 移位量为0
    __m128i shift_zero = _mm_set1_epi64x(0);
    asm volatile ("vpsllvq %1, %2, %0" : "=x"(dst128) : "x"(shift_zero), "x"(src128));
    printf("\nTest 4: Shift count 0 (no change)\n");
    print_xmm("Result", dst128);
    printf("Comparison: %s\n", cmp_xmm(dst128, src128) ? "PASS" : "FAIL");
    
    // 测试5: 移位量为63
    __m128i shift_63 = _mm_set1_epi64x(63);
    asm volatile ("vpsllvq %1, %2, %0" : "=x"(dst128) : "x"(shift_63), "x"(src128));
    printf("\nTest 5: Shift count 63 (max value)\n");
    print_xmm("Result", dst128);
    
    // 计算预期值
    for (int i = 0; i < 2; i++) {
        expected128[i] = src_data[i] << 63;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
    
    // 测试6: 移位量为64（应该为0）
    __m128i shift_64 = _mm_set1_epi64x(64);
    asm volatile ("vpsllvq %1, %2, %0" : "=x"(dst128) : "x"(shift_64), "x"(src128));
    printf("\nTest 6: Shift count 64 (should be 0)\n");
    print_xmm("Result", dst128);
    
    for (int i = 0; i < 2; i++) {
        expected128[i] = 0;
    }
    exp128 = _mm_loadu_si128((__m128i*)expected128);
    printf("Comparison: %s\n", cmp_xmm(dst128, exp128) ? "PASS" : "FAIL");
}

int main() {
    test_vpsllvq();
    return 0;
}
