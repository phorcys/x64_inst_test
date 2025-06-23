#include "avx.h"
#include <stdio.h>
#include <stdint.h>

// 测试VPMADDUBSW指令
int test_vpmaddubsw() {
    int ret = 0;
    
    // 测试128位版本
    {
        printf("=== Testing VPMADDUBSW (128-bit) ===\n");
        
        // 寄存器-寄存器测试
        {
            __m128i a = _mm_setr_epi8(
                0x00, 0x01, 0x7F, 0x80,  // 0, 1, 127, -128 (signed)
                0xFF, 0xFE, 0x81, 0x7E,  // -1, -2, -127, 126
                0x40, 0xC0, 0x20, 0xE0, // 64, -64, 32, -32
                0x10, 0xF0, 0x08, 0xF8   // 16, -16, 8, -8
            );
            
            __m128i b = _mm_setr_epi8(
                0x00, 0x7F, 0x01, 0xFF,  // 0, 127, 1, -1
                0x80, 0x7F, 0xFF, 0x01,  // -128, 127, -1, 1
                0x40, 0xC0, 0x20, 0xE0,  // 64, -64, 32, -32
                0x10, 0xF0, 0x08, 0xF8    // 16, -16, 8, -8
            );
            
            __m128i result;
            result = _mm_maddubs_epi16(a, b);
            
            // 重新计算预期值
            uint16_t expected_result[8];
            for (int i = 0; i < 8; i++) {
                uint8_t a_lo = ((uint8_t*)&a)[i*2];
                uint8_t a_hi = ((uint8_t*)&a)[i*2+1];
                int8_t b_lo = ((int8_t*)&b)[i*2];
                int8_t b_hi = ((int8_t*)&b)[i*2+1];
                
                int16_t prod_lo = (int16_t)a_lo * b_lo;
                int16_t prod_hi = (int16_t)a_hi * b_hi;
                expected_result[i] = (uint16_t)(prod_lo + prod_hi);
            }
            __m128i expected = _mm_loadu_si128((__m128i*)expected_result);
            
            printf("Testing register-register operation\n");
            print_xmm("a", a);
            print_xmm("b", b);
            print_xmm("result", result);
            print_xmm("expected", expected);
            
            if (!cmp_xmm(result, expected)) {
                printf("Mismatch in register-register test\n");
                ret = 1;
            }
        }
        
        // 寄存器-内存测试
        {
            ALIGNED(16) uint8_t mem[16] = {
                0x00, 0x01, 0x7F, 0x80, 0xFF, 0xFE, 0x81, 0x7E,
                0x40, 0xC0, 0x20, 0xE0, 0x10, 0xF0, 0x08, 0xF8
            };
            
            __m128i a = _mm_setr_epi8(
                0x00, 0x7F, 0x01, 0xFF, 0x80, 0x7F, 0xFF, 0x01,
                0x40, 0xC0, 0x20, 0xE0, 0x10, 0xF0, 0x08, 0xF8
            );
            
            __m128i result;
            result = _mm_maddubs_epi16(a, _mm_loadu_si128((__m128i*)mem));
            
            // 重新计算预期值(128位内存操作数)
            uint16_t expected_result[8];
            for (int i = 0; i < 8; i++) {
                uint8_t a_lo = ((uint8_t*)&a)[i*2];
                uint8_t a_hi = ((uint8_t*)&a)[i*2+1];
                int8_t b_lo = ((int8_t*)mem)[i*2];
                int8_t b_hi = ((int8_t*)mem)[i*2+1];
                
                int16_t prod_lo = (int16_t)a_lo * b_lo;
                int16_t prod_hi = (int16_t)a_hi * b_hi;
                expected_result[i] = (uint16_t)(prod_lo + prod_hi);
            }
            __m128i expected = _mm_loadu_si128((__m128i*)expected_result);
            
            printf("\nTesting register-memory operation\n");
            print_xmm("a", a);
            printf("Memory operand: ");
            for (int i = 0; i < 16; i++) printf("%02x ", mem[i]);
            printf("\n");
            print_xmm("result", result);
            print_xmm("expected", expected);
            
            if (!cmp_xmm(result, expected)) {
                printf("Mismatch in register-memory test\n");
                ret = 1;
            }
        }
    }
    
    // 测试256位版本
    {
        printf("\n=== Testing VPMADDUBSW (256-bit) ===\n");
        
        // 寄存器-寄存器测试
        {
            __m256i a = _mm256_setr_epi8(
                0x00, 0x01, 0x7F, 0x80, 0xFF, 0xFE, 0x81, 0x7E,
                0x40, 0xC0, 0x20, 0xE0, 0x10, 0xF0, 0x08, 0xF8,
                0x00, 0x01, 0x7F, 0x80, 0xFF, 0xFE, 0x81, 0x7E,
                0x40, 0xC0, 0x20, 0xE0, 0x10, 0xF0, 0x08, 0xF8
            );
            
            __m256i b = _mm256_setr_epi8(
                0x00, 0x7F, 0x01, 0xFF, 0x80, 0x7F, 0xFF, 0x01,
                0x40, 0xC0, 0x20, 0xE0, 0x10, 0xF0, 0x08, 0xF8,
                0x00, 0x7F, 0x01, 0xFF, 0x80, 0x7F, 0xFF, 0x01,
                0x40, 0xC0, 0x20, 0xE0, 0x10, 0xF0, 0x08, 0xF8
            );
            
            __m256i result;
            result = _mm256_maddubs_epi16(a, b);
            
            // 重新计算预期值
            uint16_t expected_result[16];
            for (int i = 0; i < 16; i++) {
                uint8_t a_lo = ((uint8_t*)&a)[i*2];
                uint8_t a_hi = ((uint8_t*)&a)[i*2+1];
                int8_t b_lo = ((int8_t*)&b)[i*2];
                int8_t b_hi = ((int8_t*)&b)[i*2+1];
                
                int16_t prod_lo = (int16_t)a_lo * b_lo;
                int16_t prod_hi = (int16_t)a_hi * b_hi;
                expected_result[i] = (uint16_t)(prod_lo + prod_hi);
            }
            __m256i expected = _mm256_loadu_si256((__m256i*)expected_result);
            
            printf("Testing register-register operation\n");
            print_ymm("a", a);
            print_ymm("b", b);
            print_ymm("result", result);
            print_ymm("expected", expected);
            
            if (!cmp_ymm(result, expected)) {
                printf("Mismatch in register-register test\n");
                ret = 1;
            }
        }
        
        // 寄存器-内存测试
        {
            ALIGNED(32) uint8_t mem[32] = {
                0x00, 0x01, 0x7F, 0x80, 0xFF, 0xFE, 0x81, 0x7E,
                0x40, 0xC0, 0x20, 0xE0, 0x10, 0xF0, 0x08, 0xF8,
                0x00, 0x01, 0x7F, 0x80, 0xFF, 0xFE, 0x81, 0x7E,
                0x40, 0xC0, 0x20, 0xE0, 0x10, 0xF0, 0x08, 0xF8
            };
            
            __m256i a = _mm256_setr_epi8(
                0x00, 0x7F, 0x01, 0xFF, 0x80, 0x7F, 0xFF, 0x01,
                0x40, 0xC0, 0x20, 0xE0, 0x10, 0xF0, 0x08, 0xF8,
                0x00, 0x7F, 0x01, 0xFF, 0x80, 0x7F, 0xFF, 0x01,
                0x40, 0xC0, 0x20, 0xE0, 0x10, 0xF0, 0x08, 0xF8
            );
            
            __m256i result;
            result = _mm256_maddubs_epi16(a, _mm256_loadu_si256((__m256i*)mem));
            
            // 重新计算预期值(256位内存操作数)
            uint16_t expected_result[16];
            for (int i = 0; i < 16; i++) {
                uint8_t a_lo = ((uint8_t*)&a)[i*2];
                uint8_t a_hi = ((uint8_t*)&a)[i*2+1];
                int8_t b_lo = ((int8_t*)mem)[i*2];
                int8_t b_hi = ((int8_t*)mem)[i*2+1];
                
                int16_t prod_lo = (int16_t)a_lo * b_lo;
                int16_t prod_hi = (int16_t)a_hi * b_hi;
                expected_result[i] = (uint16_t)(prod_lo + prod_hi);
            }
            __m256i expected = _mm256_loadu_si256((__m256i*)expected_result);
            
            printf("\nTesting register-memory operation\n");
            print_ymm("a", a);
            printf("Memory operand: ");
            for (int i = 0; i < 32; i++) printf("%02x ", mem[i]);
            printf("\n");
            print_ymm("result", result);
            print_ymm("expected", expected);
            
            if (!cmp_ymm(result, expected)) {
                printf("Mismatch in register-memory test\n");
                ret = 1;
            }
        }
    }
    
    return ret;
}

int main() {
    return test_vpmaddubsw();
}
