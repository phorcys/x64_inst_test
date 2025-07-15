#include <stdio.h>
#include <stdint.h>
#include "avx.h"

// 定义EFLAGS条件掩码
#define MPSADBW_EFLAGS_COND (X_CF|X_PF|X_ZF|X_SF|X_OF)

// 打印测试标题
static void print_test_header(int test_num, const char* desc, uint8_t imm8) {
    printf("\nTest %d: %s (imm8=0x%02X)\n", test_num, desc, imm8);
}

int main() {
    printf("Testing VMPSADBW instruction\n");
    printf("===========================\n");
    
    uint32_t mxcsr_before, mxcsr_after;
    
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
        
        // 获取执行前状态
        mxcsr_before = get_mxcsr();
        
        asm volatile(
            "vmpsadbw $0, %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [b] "x" (b)
            : "cc");
        
        // 获取执行后状态
        mxcsr_after = get_mxcsr();
        
        print_test_header(1, "Basic register operands", 0);
        print_m128i_hex(a, "Input A");
        print_m128i_hex(b, "Input B");
        print_m128i_hex(result, "Result  ");
        printf("MXCSR before: 0x%08X, after: 0x%08X\n", mxcsr_before, mxcsr_after);
        
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
            print_m128i_hex(expected_result, "Expected");
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
        print_m128i_hex(a, "Input A");
        print_m128i_hex(b, "Input B");
        print_m128i_hex(result, "Result  ");
        
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
            print_m128i_hex(expected_result, "Expected");
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
        print_m128i_hex(a, "Input A");
        print_m128i_hex(b_mem, "Input B");
        print_m128i_hex(result, "Result  ");
        
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
            print_m128i_hex(expected_result, "Expected");
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
        print_m128i_hex(a, "Input A");
        print_m128i_hex(b, "Input B");
        print_m128i_hex(result, "Result  ");
        
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
            print_m128i_hex(expected_result, "Expected");
        }
    }
    
    // 测试5: 256位操作测试
    {
        __m256i a = _mm256_setr_epi8(
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
        );
        
        __m256i b = _mm256_setr_epi8(
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
            0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
        );
        
        __m256i result;
        
        mxcsr_before = get_mxcsr();
        
        asm volatile(
            "vmpsadbw $0x07, %[b], %[a], %[res]"
            : [res] "=x" (result)
            : [a] "x" (a), [b] "x" (b)
            : "cc", "memory");
        
        mxcsr_after = get_mxcsr();
        
        print_test_header(5, "256-bit operation", 0x07);
        print_m256i_hex(a, "Input A");
        print_m256i_hex(b, "Input B");
        print_m256i_hex(result, "Result  ");
        printf("MXCSR before: 0x%08X, after: 0x%08X\n", mxcsr_before, mxcsr_after);
        
        // 验证结果
        // 根据Intel手册，256位VMPSADBW实际上是对两个128位lane分别执行操作
        // 低128位:
        // 块A[0:3] vs 块B[0:3] = |00-20|+|01-21|+|02-22|+|03-23| = 20+20+20+20 = 80 (0x80)
        // 块A[1:4] vs 块B[0:3] = |01-20|+|02-21|+|03-22|+|04-23| = 1F+1F+1F+1F = 7C (0x7C)
        // 块A[2:5] vs 块B[0:3] = |02-20|+|03-21|+|04-22|+|05-23| = 1E+1E+1E+1E = 78 (0x78)
        // 块A[3:6] vs 块B[0:3] = |03-20|+|04-21|+|05-22|+|06-23| = 1D+1D+1D+1D = 74 (0x74)
        // 高128位:
        // 块A[16:19] vs 块B[16:19] = |10-30|+|11-31|+|12-32|+|13-33| = 20+20+20+20 = 80 (0x80)
        // 块A[17:20] vs 块B[16:19] = |11-30|+|12-31|+|13-32|+|14-33| = 1F+1F+1F+1F = 7C (0x7C)
        // 块A[18:21] vs 块B[16:19] = |12-30|+|13-31|+|14-32|+|15-33| = 1E+1E+1E+1E = 78 (0x78)
        // 块A[19:22] vs 块B[16:19] = |13-30|+|14-31|+|15-32|+|16-33| = 1D+1D+1D+1D = 74 (0x74)
        uint16_t expected[16] = {
            0xA0, 0x9C, 0x98, 0x94, 0x90, 0x8C, 0x88, 0x84,
            0x80, 0x7C, 0x78, 0x74, 0x70, 0x6C, 0x68, 0x64
        };
        
        // 修正比较方式 - 使用分lane比较
        __m128i low_expected = _mm_loadu_si128((__m128i*)expected);
        __m128i high_expected = _mm_loadu_si128((__m128i*)(expected + 8));
        __m128i low_result = _mm256_extractf128_si256(result, 0);
        __m128i high_result = _mm256_extractf128_si256(result, 1);
        
        int pass = 1;
        if (!_mm_test_all_zeros(_mm_xor_si128(low_result, low_expected), _mm_set1_epi32(-1))) {
            printf("[FAIL] - Low 128-bit result mismatch\n");
            print_m128i_hex(low_expected, "Expected Low");
            print_m128i_hex(low_result, "Actual Low");
            pass = 0;
        }
        if (!_mm_test_all_zeros(_mm_xor_si128(high_result, high_expected), _mm_set1_epi32(-1))) {
            printf("[FAIL] - High 128-bit result mismatch\n");
            print_m128i_hex(high_expected, "Expected High");
            print_m128i_hex(high_result, "Actual High");
            pass = 0;
        }
        if (pass) {
            printf("[PASS]\n");
        }
    }
    
    // 测试6: 所有立即数组合测试
    #define TEST_IMM8_CASE(n) \
        do { \
            __m128i a = _mm_setr_epi8( \
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, \
                0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F \
            ); \
            __m128i b = _mm_setr_epi8( \
                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, \
                0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F \
            ); \
            __m128i result; \
            mxcsr_before = get_mxcsr(); \
            asm volatile( \
                "vmpsadbw $"#n", %[b], %[a], %[res]" \
                : [res] "=x" (result) \
                : [a] "x" (a), [b] "x" (b) \
                : "cc"); \
            mxcsr_after = get_mxcsr(); \
            print_test_header(6 + n, "All imm8 combinations", n); \
            print_m128i_hex(a, "Input A"); \
            print_m128i_hex(b, "Input B"); \
            print_m128i_hex(result, "Result  "); \
            printf("MXCSR before: 0x%08X, after: 0x%08X\n", mxcsr_before, mxcsr_after); \
        } while(0)

    TEST_IMM8_CASE(0);
    TEST_IMM8_CASE(1);
    TEST_IMM8_CASE(2);
    TEST_IMM8_CASE(3);
    TEST_IMM8_CASE(4);
    TEST_IMM8_CASE(5);
    TEST_IMM8_CASE(6);
    TEST_IMM8_CASE(7);

    #define TEST256_IMM8_CASE(n) \
        do { \
        __m256i a = _mm256_setr_epi8( \
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, \
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, \
            0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F \
        ); \
        __m256i b = _mm256_setr_epi8( \
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, \
            0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, \
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, \
            0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F \
        ); \
        __m256i result; \
            mxcsr_before = get_mxcsr(); \
            asm volatile( \
                "vmpsadbw $"#n", %[b], %[a], %[res]" \
                : [res] "=x" (result) \
                : [a] "x" (a), [b] "x" (b) \
                : "cc"); \
            mxcsr_after = get_mxcsr(); \
            print_test_header(6 + n, "All imm8 combinations", n); \
            print_m256i_hex(a, "Input A"); \
            print_m256i_hex(b, "Input B"); \
            print_m256i_hex(result, "Result  "); \
            printf("MXCSR before: 0x%08X, after: 0x%08X\n", mxcsr_before, mxcsr_after); \
        } while(0)
        // 测试6: 所有立即数组合测试

    TEST256_IMM8_CASE(0);
    TEST256_IMM8_CASE(1);
    TEST256_IMM8_CASE(2);
    TEST256_IMM8_CASE(3);
    TEST256_IMM8_CASE(4);
    TEST256_IMM8_CASE(5);
    TEST256_IMM8_CASE(6);
    TEST256_IMM8_CASE(7);
    TEST256_IMM8_CASE(8);
    TEST256_IMM8_CASE(9);
    TEST256_IMM8_CASE(10);
    TEST256_IMM8_CASE(11);
    TEST256_IMM8_CASE(12);
    TEST256_IMM8_CASE(13);
    TEST256_IMM8_CASE(14);
    TEST256_IMM8_CASE(15);
    TEST256_IMM8_CASE(16);
    TEST256_IMM8_CASE(17);
    TEST256_IMM8_CASE(18);
    TEST256_IMM8_CASE(19);
    TEST256_IMM8_CASE(20);
    TEST256_IMM8_CASE(21);
    TEST256_IMM8_CASE(22);
    TEST256_IMM8_CASE(23);
    TEST256_IMM8_CASE(24);
    TEST256_IMM8_CASE(25);
    TEST256_IMM8_CASE(26);
    TEST256_IMM8_CASE(27);
    TEST256_IMM8_CASE(28);
    TEST256_IMM8_CASE(29);    
    TEST256_IMM8_CASE(30);
    TEST256_IMM8_CASE(31);
    TEST256_IMM8_CASE(32);
    TEST256_IMM8_CASE(33);
    TEST256_IMM8_CASE(34);
    TEST256_IMM8_CASE(35);
    TEST256_IMM8_CASE(36);
    TEST256_IMM8_CASE(37);
    TEST256_IMM8_CASE(38);
    TEST256_IMM8_CASE(39);
    TEST256_IMM8_CASE(40);
    TEST256_IMM8_CASE(41);
    TEST256_IMM8_CASE(42);
    TEST256_IMM8_CASE(43);
    TEST256_IMM8_CASE(44);
    TEST256_IMM8_CASE(45);
    TEST256_IMM8_CASE(46);
    TEST256_IMM8_CASE(47);
    TEST256_IMM8_CASE(48);
    TEST256_IMM8_CASE(49);
    TEST256_IMM8_CASE(50);
    TEST256_IMM8_CASE(51);
    TEST256_IMM8_CASE(52);
    TEST256_IMM8_CASE(53);
    TEST256_IMM8_CASE(54);
    TEST256_IMM8_CASE(55);
    TEST256_IMM8_CASE(56);
    TEST256_IMM8_CASE(57);
    TEST256_IMM8_CASE(58);
    TEST256_IMM8_CASE(59);
    TEST256_IMM8_CASE(60);
    TEST256_IMM8_CASE(61);
    TEST256_IMM8_CASE(62);
    TEST256_IMM8_CASE(63);
    printf("\nVMPSADBW tests completed.\n");
    return 0;
}
