#include "avx.h"
#include <stdio.h>
#include <stdint.h>

static void test_vpshuflw_reg() {
    printf("Testing VPSHUFLW (register operands)\n");
    printf("----------------------------------\n");
    
    // 测试128位操作
    {
        __m128i src = _mm_setr_epi16(0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008);
        __m128i dst;
        
        // 正常洗牌：imm8=0xE4 (0b11 10 01 00) -> [3, 2, 1, 0] (只洗牌低64位)
        asm volatile("vpshuflw %2, %1, %0" : "=v"(dst) : "v"(src), "i"(0xE4));
        
        __m128i expected = _mm_setr_epi16(0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008);
        
        printf("\nTest case 1: Normal shuffle (imm8=0xE4)\n");
        print_xmm("Source", src);
        print_xmm("Result", dst);
        print_xmm("Expected", expected);
        
        if (cmp_xmm(dst, expected)) {
            printf("Result: PASS\n");
        } else {
            printf("Result: FAIL\n");
        }
    }
    
    // 测试256位操作
    {
        __m256i src = _mm256_setr_epi16(
            0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008,
            0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010
        );
        __m256i dst;
        
        // 复制同一元素：imm8=0x00 (全部复制第0个元素)
        asm volatile("vpshuflw %2, %1, %0" : "=v"(dst) : "v"(src), "i"(0x00));
        
        __m256i expected = _mm256_setr_epi16(
            0x0001, 0x0001, 0x0001, 0x0001, 0x0005, 0x0006, 0x0007, 0x0008,
            0x0009, 0x0009, 0x0009, 0x0009, 0x000D, 0x000E, 0x000F, 0x0010
        );
        
        printf("\nTest case 2: Copy same element (imm8=0x00)\n");
        print_ymm("Source", src);
        print_ymm("Result", dst);
        print_ymm("Expected", expected);
        
        if (cmp_ymm(dst, expected)) {
            printf("Result: PASS\n");
        } else {
            printf("Result: FAIL\n");
        }
    }
    
    // 边界值测试：全0
    {
        __m128i src = _mm_setzero_si128();
        __m128i dst;
        
        asm volatile("vpshuflw %2, %1, %0" : "=v"(dst) : "v"(src), "i"(0x1B));
        
        __m128i expected = _mm_setzero_si128();
        
        printf("\nTest case 3: All zeros\n");
        print_xmm("Source", src);
        print_xmm("Result", dst);
        print_xmm("Expected", expected);
        
        if (cmp_xmm(dst, expected)) {
            printf("Result: PASS\n");
        } else {
            printf("Result: FAIL\n");
        }
    }
    
    // 边界值测试：全1
    {
        __m128i src = _mm_set1_epi16(0xFFFF);
        __m128i dst;
        
        asm volatile("vpshuflw %2, %1, %0" : "=v"(dst) : "v"(src), "i"(0x39));
        
        __m128i expected = _mm_setr_epi16(
            0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
        );
        
        printf("\nTest case 4: All ones\n");
        print_xmm("Source", src);
        print_xmm("Result", dst);
        print_xmm("Expected", expected);
        
        if (cmp_xmm(dst, expected)) {
            printf("Result: PASS\n");
        } else {
            printf("Result: FAIL\n");
        }
    }
    
    // 符号位变化测试
    {
        __m128i src = _mm_setr_epi16(0x7FFF, 0x8000, 0x0000, 0xFFFF, 0x1234, 0x5678, 0x9ABC, 0xDEF0);
        __m128i dst;
        
        // 反转顺序：imm8=0x1B (0b00 01 10 11) -> [3, 2, 1, 0] (只洗牌低64位)
        asm volatile("vpshuflw %2, %1, %0" : "=v"(dst) : "v"(src), "i"(0x1B));
        
        __m128i expected = _mm_setr_epi16(
            0xFFFF, 0x0000, 0x8000, 0x7FFF, 0x1234, 0x5678, 0x9ABC, 0xDEF0
        );
        
        printf("\nTest case 5: Sign bit variations\n");
        print_xmm("Source", src);
        print_xmm("Result", dst);
        print_xmm("Expected", expected);
        
        if (cmp_xmm(dst, expected)) {
            printf("Result: PASS\n");
        } else {
            printf("Result: FAIL\n");
        }
    }
    
    printf("\n");
}

static void test_vpshuflw_mem() {
    printf("Testing VPSHUFLW (memory operands)\n");
    printf("---------------------------------\n");
    
    // 内存操作数测试 (128位)
    {
        uint16_t mem_src[8] = {0x1234, 0x5678, 0x9ABC, 0xDEF0, 0x1111, 0x2222, 0x3333, 0x4444};
        __m128i dst;
        
        // 正常洗牌：imm8=0x1B (0b00 01 10 11) -> [3, 2, 1, 0]
        asm volatile("vpshuflw %2, %1, %0" : "=v"(dst) : "m"(*mem_src), "i"(0x1B));
        
        __m128i expected = _mm_setr_epi16(
            0xDEF0, 0x9ABC, 0x5678, 0x1234, 0x1111, 0x2222, 0x3333, 0x4444
        );
        
        printf("\nTest case 1: Memory operand (128-bit)\n");
        print_xmm("Result", dst);
        print_xmm("Expected", expected);
        
        if (cmp_xmm(dst, expected)) {
            printf("Result: PASS\n");
        } else {
            printf("Result: FAIL\n");
        }
    }
    
    // 内存操作数测试 (256位)
    {
        uint16_t mem_src[16] ALIGNED(32) = {
            0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008,
            0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010
        };
        __m256i dst;
        
        // 复制同一元素：imm8=0x00 (全部复制第0个元素)
        asm volatile("vpshuflw %2, %1, %0" : "=v"(dst) : "m"(*mem_src), "i"(0x00));
        
        __m256i expected = _mm256_setr_epi16(
            0x0001, 0x0001, 0x0001, 0x0001, 0x0005, 0x0006, 0x0007, 0x0008,
            0x0009, 0x0009, 0x0009, 0x0009, 0x000D, 0x000E, 0x000F, 0x0010
        );
        
        printf("\nTest case 2: Memory operand (256-bit)\n");
        print_ymm("Result", dst);
        print_ymm("Expected", expected);
        
        if (cmp_ymm(dst, expected)) {
            printf("Result: PASS\n");
        } else {
            printf("Result: FAIL\n");
        }
    }
    
    printf("\n");
}

int main() {
    // 保存并打印MXCSR寄存器状态
    uint32_t mxcsr_before = get_mxcsr();
    printf("MXCSR before tests: 0x%08X\n", mxcsr_before);
    print_mxcsr(mxcsr_before);
    
    // 运行测试
    test_vpshuflw_reg();
    test_vpshuflw_mem();
    
    // 再次检查MXCSR寄存器状态
    uint32_t mxcsr_after = get_mxcsr();
    printf("\nMXCSR after tests: 0x%08X\n", mxcsr_after);
    print_mxcsr(mxcsr_after);
    
    // 检查MXCSR是否变化
    if (mxcsr_before == mxcsr_after) {
        printf("MXCSR unchanged: PASS\n");
    } else {
        printf("MXCSR changed: 0x%08X -> 0x%08X\n", mxcsr_before, mxcsr_after);
    }
    
    printf("\nAll VPSHUFLW tests completed.\n");
    return 0;
}
