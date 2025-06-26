#include "avx.h"
#include <stdio.h>
#include <string.h>

static void print_result(const char* label, const uint8_t* data, size_t size) {
    printf("%s: ", label);
    for (size_t i = 0; i < size; i++) {
        if (i < size) { // 确保不越界访问
            printf("%02X ", data[i]);
        } else {
            printf("00 "); // 越界时填充0
        }
    }
    printf("\n");
}

static int verify_result(const uint8_t* result, const uint8_t* expected, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (result[i] != expected[i]) {
            return 0;
        }
    }
    return 1;
}

static void test_vpalignr_128() {
    printf("\n--- Testing 128-bit vpalignr ---\n");

    // 测试数据
    ALIGNED(16) uint8_t src1[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    ALIGNED(16) uint8_t src2[16] = {
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };

    // 测试不同偏移量 (VPALIGNR指令只使用低4位偏移量，有效范围0-15)
    for (int offset = 0; offset < 32; offset++) {
        ALIGNED(16) uint8_t result[16] = {0};
        ALIGNED(16) uint8_t expected[16] = {0};
        
        // 计算预期结果
        memset(expected, 0, 16);
        if(offset < 16) {
            // 如果偏移量小于16，从src2开始复制
            int copy_len = 16 - offset;
            memcpy(expected, src2 + offset, copy_len);
            if (copy_len < 16) {
                memcpy(expected + copy_len, src1, 16 - copy_len);
            }
        } else {
            // 如果偏移量大于等于16，从src1开始复制
            int copy_len = offset - 16;
            memcpy(expected, src1 + copy_len, 16 - copy_len);
        }
        
        __m128i a = _mm_load_si128((__m128i*)src1);
        __m128i b = _mm_load_si128((__m128i*)src2);
        __m128i res;
        
#define TEST_VPALIGNR_128(offset) \
    asm volatile ( \
        "vpalignr $" #offset ", %2, %1, %0" \
        : "=x"(res) \
        : "x"(a), "x"(b) \
    );
    
    switch(offset) {
        case 0: TEST_VPALIGNR_128(0); break;
        case 1: TEST_VPALIGNR_128(1); break;
        case 2: TEST_VPALIGNR_128(2); break;
        case 3: TEST_VPALIGNR_128(3); break;
        case 4: TEST_VPALIGNR_128(4); break;
        case 5: TEST_VPALIGNR_128(5); break;
        case 6: TEST_VPALIGNR_128(6); break;
        case 7: TEST_VPALIGNR_128(7); break;
        case 8: TEST_VPALIGNR_128(8); break;
        case 9: TEST_VPALIGNR_128(9); break;
        case 10: TEST_VPALIGNR_128(10); break;
        case 11: TEST_VPALIGNR_128(11); break;
        case 12: TEST_VPALIGNR_128(12); break;
        case 13: TEST_VPALIGNR_128(13); break;
        case 14: TEST_VPALIGNR_128(14); break;
        case 15: TEST_VPALIGNR_128(15); break;
        case 16: TEST_VPALIGNR_128(16); break;
        case 17: TEST_VPALIGNR_128(17); break;
        case 18: TEST_VPALIGNR_128(18); break;
        case 19: TEST_VPALIGNR_128(19); break;
        case 20: TEST_VPALIGNR_128(20); break;
        case 21: TEST_VPALIGNR_128(21); break;
        case 22: TEST_VPALIGNR_128(22); break;
        case 23: TEST_VPALIGNR_128(23); break;
        case 24: TEST_VPALIGNR_128(24); break;
        case 25: TEST_VPALIGNR_128(25); break;
        case 26: TEST_VPALIGNR_128(26); break;
        case 27: TEST_VPALIGNR_128(27); break;
        case 28: TEST_VPALIGNR_128(28); break;
        case 29: TEST_VPALIGNR_128(29); break;
        case 30: TEST_VPALIGNR_128(30); break;
        case 31: TEST_VPALIGNR_128(31); break;
        case 32: TEST_VPALIGNR_128(32); break;
        default: break;
    }
        
        _mm_store_si128((__m128i*)result, res);
        
        printf("\nOffset %d:\n", offset);
        print_result("Src1", src1, 16);
        print_result("Src2", src2, 16);

        print_result("Result", result, 16);
        print_result("Expected", expected, 16);
        
        if (verify_result(result, expected, 16)) {
            printf("TEST PASSED\n");
        } else {
            printf("TEST FAILED\n");
        }
    }
}

static void test_vpalignr_256() {
    printf("\n--- Testing 256-bit vpalignr ---\n");

    // 测试数据
    ALIGNED(32) uint8_t src1[32] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };
    ALIGNED(32) uint8_t src2[32] = {
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
    };

    // 测试不同偏移量 (VPALIGNR指令只使用低4位偏移量，有效范围0-15)
    for (int offset = 0; offset < 33; offset++) {
        ALIGNED(32) uint8_t result[32] = {0};
        ALIGNED(32) uint8_t expected[32] = {0};
        
        // 计算预期结果 (256位版本处理两个128位lane)
        memset(expected, 0, 32);
        if(offset < 16) {
            int copy_len = 16 - offset;
            memcpy(expected, src2 + offset, copy_len);
            memcpy(expected +16, src2 +16 + offset, copy_len);
            if (copy_len < 16) {
                memcpy(expected + copy_len, src1, 16 - copy_len);
                memcpy(expected + 16 + copy_len, src1 + 16, 16 - copy_len);
            }
        } else {
            // 如果偏移量大于等于16，从src1开始复制
            int copy_len = offset - 16;
            memcpy(expected, src1 + copy_len, 16 - copy_len);
            memcpy(expected + 16, src1 + 16 + copy_len, 16 - copy_len);
        }
        
        
        __m256i a = _mm256_load_si256((__m256i*)src1);
        __m256i b = _mm256_load_si256((__m256i*)src2);
        __m256i res;
        
#define TEST_VPALIGNR_256(offset) \
    asm volatile ( \
        "vpalignr $" #offset ", %2, %1, %0" \
        : "=x"(res) \
        : "x"(a), "x"(b) \
    );
    
    switch(offset) {
        case 0: TEST_VPALIGNR_256(0); break;
        case 1: TEST_VPALIGNR_256(1); break;
        case 2: TEST_VPALIGNR_256(2); break;
        case 3: TEST_VPALIGNR_256(3); break;
        case 4: TEST_VPALIGNR_256(4); break;
        case 5: TEST_VPALIGNR_256(5); break;
        case 6: TEST_VPALIGNR_256(6); break;
        case 7: TEST_VPALIGNR_256(7); break;
        case 8: TEST_VPALIGNR_256(8); break;
        case 9: TEST_VPALIGNR_256(9); break;
        case 10: TEST_VPALIGNR_256(10); break;
        case 11: TEST_VPALIGNR_256(11); break;
        case 12: TEST_VPALIGNR_256(12); break;
        case 13: TEST_VPALIGNR_256(13); break;
        case 14: TEST_VPALIGNR_256(14); break;
        case 15: TEST_VPALIGNR_256(15); break;
        case 16: TEST_VPALIGNR_256(16); break;
        case 17: TEST_VPALIGNR_256(17); break;
        case 18: TEST_VPALIGNR_256(18); break;
        case 19: TEST_VPALIGNR_256(19); break;
        case 20: TEST_VPALIGNR_256(20); break;
        case 21: TEST_VPALIGNR_256(21); break;
        case 22: TEST_VPALIGNR_256(22); break;
        case 23: TEST_VPALIGNR_256(23); break;
        case 24: TEST_VPALIGNR_256(24); break;
        case 25: TEST_VPALIGNR_256(25); break;
        case 26: TEST_VPALIGNR_256(26); break;
        case 27: TEST_VPALIGNR_256(27); break;
        case 28: TEST_VPALIGNR_256(28); break;
        case 29: TEST_VPALIGNR_256(29); break;
        case 30: TEST_VPALIGNR_256(30); break;
        case 31: TEST_VPALIGNR_256(31); break;
        case 32: TEST_VPALIGNR_256(32); break;
        default: break;
    }
        
        _mm256_store_si256((__m256i*)result, res);
        
        printf("\nOffset %d:\n", offset);
        print_result("Src1", src1, 32);
        print_result("Src2", src2, 32);
        print_result("Result", result, 32);
        print_result("Expected", expected, 32);
        
        if (verify_result(result, expected, 32)) {
            printf("TEST PASSED\n");
        } else {
            printf("TEST FAILED\n");
        }
    }
}

static void test_vpalignr_mem() {
    printf("\n--- Testing vpalignr with memory operand ---\n");

    ALIGNED(16) uint8_t src1[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    ALIGNED(16) uint8_t src2[16] = {
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };

    // 测试不同偏移量 (VPALIGNR指令的立即数范围是0-255，我们测试0-15)
    for (int offset = 0; offset < 33; offset++) {
        ALIGNED(16) uint8_t result[16] = {0};
        ALIGNED(16) uint8_t expected[16] = {0};
        
        // 计算预期结果
        memset(expected, 0, 16);
        if(offset < 16) {
            // 如果偏移量小于16，从src2开始复制
            int copy_len = 16 - offset;
            memcpy(expected, src2 + offset, copy_len);
            if (copy_len < 16) {
                memcpy(expected + copy_len, src1, 16 - copy_len);
            }
        } else {
            // 如果偏移量大于等于16，从src1开始复制
            int copy_len = offset - 16;
            memcpy(expected, src1 + copy_len, 16 - copy_len);
        }
        
        __m128i a = _mm_load_si128((__m128i*)src1);
        __m128i res;
        
#define TEST_VPALIGNR_MEM(offset) \
    asm volatile ( \
        "vpalignr $" #offset ", %1, %0, %0" \
        : "+x"(a) \
        : "m"(*src2) \
    ); \
    res = a;
    
    switch(offset) {
        case 0: TEST_VPALIGNR_MEM(0); break;
        case 1: TEST_VPALIGNR_MEM(1); break;
        case 2: TEST_VPALIGNR_MEM(2); break;
        case 3: TEST_VPALIGNR_MEM(3); break;
        case 4: TEST_VPALIGNR_MEM(4); break;
        case 5: TEST_VPALIGNR_MEM(5); break;
        case 6: TEST_VPALIGNR_MEM(6); break;
        case 7: TEST_VPALIGNR_MEM(7); break;
        case 8: TEST_VPALIGNR_MEM(8); break;
        case 9: TEST_VPALIGNR_MEM(9); break;
        case 10: TEST_VPALIGNR_MEM(10); break;
        case 11: TEST_VPALIGNR_MEM(11); break;
        case 12: TEST_VPALIGNR_MEM(12); break;
        case 13: TEST_VPALIGNR_MEM(13); break;
        case 14: TEST_VPALIGNR_MEM(14); break;
        case 15: TEST_VPALIGNR_MEM(15); break;
        case 16: TEST_VPALIGNR_MEM(16); break;
        case 17: TEST_VPALIGNR_MEM(17); break;
        case 18: TEST_VPALIGNR_MEM(18); break;
        case 19: TEST_VPALIGNR_MEM(19); break;
        case 20: TEST_VPALIGNR_MEM(20); break;
        case 21: TEST_VPALIGNR_MEM(21); break;
        case 22: TEST_VPALIGNR_MEM(22); break;
        case 23: TEST_VPALIGNR_MEM(23); break;
        case 24: TEST_VPALIGNR_MEM(24); break;
        case 25: TEST_VPALIGNR_MEM(25); break;
        case 26: TEST_VPALIGNR_MEM(26); break;
        case 27: TEST_VPALIGNR_MEM(27); break;
        case 28: TEST_VPALIGNR_MEM(28); break;
        case 29: TEST_VPALIGNR_MEM(29); break;
        case 30: TEST_VPALIGNR_MEM(30); break;
        case 31: TEST_VPALIGNR_MEM(31); break;
        case 32: TEST_VPALIGNR_MEM(32); break;
        default: break;
    }
        
        _mm_store_si128((__m128i*)result, res);
        
        printf("\nOffset %d:\n", offset);
        print_result("Src1", src1, 16);
        print_result("Src2", src2, 16);
        print_result("Result", result, 16);
        print_result("Expected", expected, 16);
        
        if (verify_result(result, expected, 16)) {
            printf("TEST PASSED\n");
        } else {
            printf("TEST FAILED\n");
        }
    }
}

int main() {
    printf("--- Testing vpalignr (packed byte alignment) ---\n");
    test_vpalignr_128();
    test_vpalignr_256();
    test_vpalignr_mem();
    return 0;
}
