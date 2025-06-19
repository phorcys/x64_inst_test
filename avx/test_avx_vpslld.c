#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 定义128位向量类型（4个32位整数）
typedef union {
    __m128i v;
    int32_t i[4];
} v4si;

// 定义256位向量类型（8个32位整数）
typedef union {
    __m256i v;
    int32_t i[8];
} v8si;

void test_vpslld_128() {
    printf("=== Testing 128-bit VPSLLD instruction ===\n");
    
    // 测试用例1: 基本逻辑左移（立即数）
    {
        v4si src = { .i = {0x12345678, 0x87654321, 0x00000001, 0xFFFFFFFF} };
        v4si result;
        uint8_t shift = 4;
        
        __asm__ __volatile__ (
            "vpslld $4, %1, %0"
            : "=x" (result.v)
            : "x" (src.v)
        );
        
        v4si expected = {
            .i = {
                src.i[0] << shift,
                src.i[1] << shift,
                src.i[2] << shift,
                src.i[3] << shift
            }
        };
        
        printf("Test 1: VPSLLD(immediate shift %d)\n", shift);
        printf("  Input:    [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               src.i[0], src.i[1], src.i[2], src.i[3]);
        printf("  Result:   [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               result.i[0], result.i[1], result.i[2], result.i[3]);
        printf("  Expected: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               expected.i[0], expected.i[1], expected.i[2], expected.i[3]);
        
        if (memcmp(&result, &expected, sizeof(v4si))) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例2: 移位0（立即数）
    {
        v4si src = { .i = {0x12345678, 0x87654321, 0x00000001, 0xFFFFFFFF} };
        v4si result;
        uint8_t shift = 0;
        
        __asm__ __volatile__ (
            "vpslld $0, %1, %0"
            : "=x" (result.v)
            : "x" (src.v)
        );
        
        v4si expected = src; // 直接赋值整个联合体
        
        printf("Test 2: VPSLLD(immediate shift %d)\n", shift);
        printf("  Input:    [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               src.i[0], src.i[1], src.i[2], src.i[3]);
        printf("  Result:   [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               result.i[0], result.i[1], result.i[2], result.i[3]);
        printf("  Expected: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               expected.i[0], expected.i[1], expected.i[2], expected.i[3]);
        
        if (memcmp(&result, &expected, sizeof(v4si))) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例3: 边界情况（移位31）
    {
        v4si src = { .i = {0x00000001, 0x00000002, 0x00000004, 0x00000008} };
        v4si result;
        uint8_t shift = 31;
        
        __asm__ __volatile__ (
            "vpslld $31, %1, %0"
            : "=x" (result.v)
            : "x" (src.v)
        );
        
        v4si expected = {
            .i = {
                src.i[0] << shift,
                src.i[1] << shift,
                src.i[2] << shift,
                src.i[3] << shift
            }
        };
        
        printf("Test 3: VPSLLD(immediate shift %d)\n", shift);
        printf("  Input:    [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               src.i[0], src.i[1], src.i[2], src.i[3]);
        printf("  Result:   [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               result.i[0], result.i[1], result.i[2], result.i[3]);
        printf("  Expected: [0x%08X, 0x%08X, 0x%08X, 0x%08X]\n", 
               expected.i[0], expected.i[1], expected.i[2], expected.i[3]);
        
        if (memcmp(&result, &expected, sizeof(v4si))) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
}

void test_vpslld_256() {
    printf("=== Testing 256-bit VPSLLD instruction ===\n");
    
    // 测试用例1: 基本逻辑左移（立即数）
    {
        v8si src = { .i = {
            0x12345678, 0x87654321, 0x00000001, 0xFFFFFFFF,
            0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00
        } };
        v8si result;
        uint8_t shift = 8;
        
        __asm__ __volatile__ (
            "vpslld $8, %1, %0"
            : "=x" (result.v)
            : "x" (src.v)
        );
        
        v8si expected = {
            .i = {
                src.i[0] << shift,
                src.i[1] << shift,
                src.i[2] << shift,
                src.i[3] << shift,
                src.i[4] << shift,
                src.i[5] << shift,
                src.i[6] << shift,
                src.i[7] << shift
            }
        };
        
        printf("Test 1: VPSLLD(immediate shift %d)\n", shift);
        printf("  Input:    [");
        for (int i = 0; i < 8; i++) printf("0x%08X%s", src.i[i], i<7 ? ", " : "");
        printf("]\n");
        printf("  Result:   [");
        for (int i = 0; i < 8; i++) printf("0x%08X%s", result.i[i], i<7 ? ", " : "");
        printf("]\n");
        printf("  Expected: [");
        for (int i = 0; i < 8; i++) printf("0x%08X%s", expected.i[i], i<7 ? ", " : "");
        printf("]\n");
        
        if (memcmp(&result, &expected, sizeof(v8si))) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
    
    // 测试用例2: 移位0（立即数）
    {
        v8si src = { .i = {
            0x12345678, 0x87654321, 0x00000001, 0xFFFFFFFF,
            0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00
        } };
        v8si result;
        uint8_t shift = 0;
        
        __asm__ __volatile__ (
            "vpslld $0, %1, %0"
            : "=x" (result.v)
            : "x" (src.v)
        );
        
        v8si expected = src; // 直接赋值整个联合体
        
        printf("Test 2: VPSLLD(immediate shift %d)\n", shift);
        printf("  Input:    [");
        for (int i = 0; i < 8; i++) printf("0x%08X%s", src.i[i], i<7 ? ", " : "");
        printf("]\n");
        printf("  Result:   [");
        for (int i = 0; i < 8; i++) printf("0x%08X%s", result.i[i], i<7 ? ", " : "");
        printf("]\n");
        printf("  Expected: [");
        for (int i = 0; i < 8; i++) printf("0x%08X%s", expected.i[i], i<7 ? ", " : "");
        printf("]\n");
        
        if (memcmp(&result, &expected, sizeof(v8si))) {
            printf("  [ERROR] Result mismatch!\n");
        } else {
            printf("  [OK]\n");
        }
    }
}

int main() {
    test_vpslld_128();
    test_vpslld_256();
    printf("All VPSLLD tests completed successfully.\n");
    return 0;
}
