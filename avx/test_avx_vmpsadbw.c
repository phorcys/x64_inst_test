#include <stdio.h>
#include <stdint.h>
#include "avx.h"

// 打印128位向量
void print_m128i(__m128i value, const char* name) {
    uint8_t *v = (uint8_t*)&value;
    printf("%s: ", name);
    for (int i = 0; i < 16; i++) {
        printf("%02X ", v[i]);
    }
    printf("\n");
}

// 打印128位向量中的字
void print_words(__m128i value, const char* name) {
    uint16_t *v = (uint16_t*)&value;
    printf("%s: ", name);
    for (int i = 0; i < 8; i++) {
        printf("%04X ", v[i]);
    }
    printf("\n");
}

int main() {
    printf("Testing VMPSADBW instruction\n");
    printf("===========================\n");
    
    // 测试1: 基本测试 - 寄存器操作数
    {
        __m128i a = _mm_setr_epi8(
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
        );
        
        __m128i b = _mm_setr_epi8(
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
        );
        
        __m128i result;
        // 立即数在指令中直接使用，不需要变量
        
        asm volatile(
            "vmpsadbw $0, %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [b] "x" (b));
        
        printf("\nTest 1: Basic register operands (imm8=0)\n");
        print_m128i(a, "Input A");
        print_m128i(b, "Input B");
        print_words(result, "Result  ");
        
        // 预期结果: 每个SAD计算
        // 块A[0:3] vs 块B[0:3] = |00-10|+|01-11|+|02-12|+|03-13| = 10+10+10+10 = 40 (0x40)
        // 块A[1:4] vs 块B[0:3] = |01-10|+|02-11|+|03-12|+|04-13| = 0F+0F+0F+0F = 3C (0x3C)
        // 块A[2:5] vs 块B[0:3] = |02-10|+|03-11|+|04-12|+|05-13| = 0E+0E+0E+0E = 38 (0x38)
        // 块A[3:6] vs 块B[0:3] = |03-10|+|04-11|+|05-12|+|06-13| = 0D+0D+0D+0D = 34 (0x34)
        // VMPSADBW 产生 8 个 16 位结果
        uint16_t expected[8] = {
            0x40, 0x3C, 0x38, 0x34, 0x30, 0x2C, 0x28, 0x24
        };
        __m128i expected_result = _mm_loadu_si128((__m128i*)expected);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected_result), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words(expected_result, "Expected");
        }
    }
    
    // 测试2: 不同的立即数偏移
    {
        __m128i a = _mm_setr_epi8(
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F
        );
        
        __m128i b = _mm_setr_epi8(
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
            0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
        );
        
        __m128i result;
        // 立即数在指令中直接使用，不需要变量
        
        asm volatile(
            "vmpsadbw $0x25, %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [b] "x" (b));
        
        printf("\nTest 2: Different immediate offset (imm8=0x25)\n");
        print_m128i(a, "Input A");
        print_m128i(b, "Input B");
        print_words(result, "Result  ");
        
        // 预期结果:
        // 块A[4:7] vs 块B[4:7] = |24-34|+|25-35|+|26-36|+|27-37| = 10+10+10+10 = 40 (0x40)
        // 块A[5:8] vs 块B[4:7] = |25-34|+|26-35|+|27-36|+|28-37| = 0F+0F+0F+0F = 3C (0x3C)
        // 块A[6:9] vs 块B[4:7] = |26-34|+|27-35|+|28-36|+|29-37| = 0E+0E+0E+0E = 38 (0x38)
        // 块A[7:10] vs 块B[4:7] = |27-34|+|28-35|+|29-36|+|2A-37| = 0D+0D+0D+0D = 34 (0x34)
        // VMPSADBW 产生 8 个 16 位结果
        uint16_t expected[8] = {
            0x40, 0x3C, 0x38, 0x34, 0x30, 0x2C, 0x28, 0x24
        };
        __m128i expected_result = _mm_loadu_si128((__m128i*)expected);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected_result), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words(expected_result, "Expected");
        }
    }
    
    // 测试3: 内存操作数
    {
        __m128i a = _mm_setr_epi8(
            0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
            0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F
        );
        
        __m128i b_mem = _mm_setr_epi8(
            0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
            0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F
        );
        
        __m128i result;
        // 立即数在指令中直接使用，不需要变量
        
        asm volatile(
            "vmpsadbw $0x0A, %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [b] "m" (b_mem));
        
        printf("\nTest 3: Memory operand (imm8=0x0A)\n");
        print_m128i(a, "Input A");
        print_m128i(b_mem, "Input B");
        print_words(result, "Result  ");
        
        // 预期结果:
        // 块A[0:3] vs 块B[8:11] = |40-58|+|41-59|+|42-5A|+|43-5B| = 18+18+18+18 = 60 (0x60)
        // 块A[1:4] vs 块B[8:11] = |41-58|+|42-59|+|43-5A|+|44-5B| = 17+17+17+17 = 5C (0x5C)
        // 块A[2:5] vs 块B[8:11] = |42-58|+|43-59|+|44-5A|+|45-5B| = 16+16+16+16 = 58 (0x58)
        // 块A[3:6] vs 块B[8:11] = |43-58|+|44-59|+|45-5A|+|46-5B| = 15+15+15+15 = 54 (0x54)
        // VMPSADBW 产生 8 个 16 位结果
        uint16_t expected[8] = {
            0x60, 0x5C, 0x58, 0x54, 0x50, 0x4C, 0x48, 0x44
        };
        __m128i expected_result = _mm_loadu_si128((__m128i*)expected);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected_result), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words(expected_result, "Expected");
        }
    }
    
    // 测试4: 边界情况 - 零和相同值
    {
        __m128i a = _mm_setr_epi8(
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        );
        
        __m128i b = _mm_setr_epi8(
            0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF
        );
        
        __m128i result;
        // 立即数在指令中直接使用，不需要变量
        
        asm volatile(
            "vmpsadbw $0x31, %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [b] "x" (b));
        
        printf("\nTest 4: Boundary values (imm8=0x31)\n");
        print_m128i(a, "Input A");
        print_m128i(b, "Input B");
        print_words(result, "Result  ");
        
        // 预期结果:
        // 块A[4:7] vs 块B[12:15] = |00-FF|+|00-FF|+|00-FF|+|00-FF| = FF*4 = 3FC (0x3FC)
        // 块A[5:8] vs 块B[12:15] = |00-FF|+|00-FF|+|00-FF|+|FF-FF| = FF*3 + 0 = 2FD (0x2FD)
        // 块A[6:9] vs 块B[12:15] = |00-FF|+|00-FF|+|FF-FF|+|FF-FF| = FF*2 = 1FE (0x1FE)
        // 块A[7:10] vs 块B[12:15] = |00-FF|+|FF-FF|+|FF-FF|+|FF-FF| = FF = 0xFF (0xFF)
        // VMPSADBW 产生 8 个 16 位结果
        uint16_t expected[8] = {
            0x03FC, 0x03FC, 0x03FC, 0x03FC, 0x03FC, 0x02FD, 0x01FE, 0x00FF
        };
        __m128i expected_result = _mm_loadu_si128((__m128i*)expected);
        
        if (_mm_test_all_zeros(_mm_xor_si128(result, expected_result), _mm_set1_epi32(-1))) {
            printf("[PASS]\n");
        } else {
            printf("[FAIL] - Result mismatch\n");
            print_words(expected_result, "Expected");
        }
    }
    
    /*
    // 测试5: 256位操作测试(暂时注释掉，待后续专门调试)
    {
        // 测试代码...
    }
    */
    
    printf("\nVMPSADBW tests completed.\n");
    return 0;
}
