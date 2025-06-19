#include "avx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// 饱和加法辅助函数
static int16_t sat_add(int16_t a, int16_t b) {
    int32_t res = (int32_t)a + (int32_t)b;
    if (res > 0x7FFF) return 0x7FFF;
    if (res < -0x8000) return -0x8000;
    return (int16_t)res;
}

// VPMADDUBSW测试函数
void test_vpmaddubsw() {
    printf("=== Testing VPMADDUBSW ===\n");

    // 测试128位操作
    {
        printf("-- Testing 128-bit operation --\n");
        
        // 初始化测试数据
        uint8_t src1[16] = {0xFF, 0x80, 0x7F, 0x00, 0x11, 0x22, 0x33, 0x44,
                            0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC};
        int8_t src2[16] = {0x7F, (int8_t)0x80, 0x7F, (int8_t)0x80, 0x11, (int8_t)0xDE, 0x33, (int8_t)0xBC,
                          (int8_t)0xAB, 0x66, (int8_t)0x89, 0x88, (int8_t)0x67, (int8_t)0xAA, (int8_t)0x45, 0xCC};
        int16_t dst[8] = {0};

        // 预期结果计算（考虑饱和）
        int16_t expected[8] = {
            sat_add((int16_t)(255 * 127), (int16_t)(128 * -128)),  // 0xFF*0x7F + 0x80*0x80
            sat_add((int16_t)(127 * 127), (int16_t)(0 * -128)),    // 0x7F*0x7F + 0x00*0x80
            sat_add((int16_t)(17 * 17), (int16_t)(34 * -34)),      // 0x11*0x11 + 0x22*0xDE
            sat_add((int16_t)(51 * 51), (int16_t)(68 * -68)),      // 0x33*0x33 + 0x44*0xBC
            sat_add((int16_t)(85 * -85), (int16_t)(102 * 102)),    // 0x55*0xAB + 0x66*0x66
            sat_add((int16_t)(119 * -119), (int16_t)(136 * -120)),  // 0x77*0x89 + 0x88*0x88
            sat_add((int16_t)(153 * 103), (int16_t)(170 * -86)),    // 0x99*0x67 + 0xAA*0xAA
            sat_add((int16_t)(187 * 69), (int16_t)(204 * -52))      // 0xBB*0x45 + 0xCC*0xCC
        };

        // 使用内联汇编执行指令并打印中间结果
        printf("Input data:\n");
        printf("Src1: ");
        for(int i=0; i<16; i++) printf("%02x ", src1[i]);
        printf("\nSrc2: ");
        for(int i=0; i<16; i++) printf("%02x ", (uint8_t)src2[i]);
        printf("\n");
        
        __asm__ __volatile__(
            "vmovdqu %1, %%xmm0\n\t"
            "vmovdqu %2, %%xmm1\n\t"
            "vpmaddubsw %%xmm1, %%xmm0, %%xmm2\n\t"
            "vmovdqu %%xmm2, %0\n\t"
            : "=m" (dst)
            : "m" (src1), "m" (src2)
            : "xmm0", "xmm1", "xmm2", "memory"
        );

        // 打印结果
        print_xmm("Result", *(__m128i*)dst);
        printf("Expected: ");
        for(int i=0; i<8; i++) {
            printf("%d ", expected[i]);
        }
        printf("\n");

        // 验证结果
        for (int i = 0; i < 8; i++) {
            if (dst[i] != expected[i]) {
                printf("Mismatch at element %d: got %d, expected %d\n", 
                      i, dst[i], expected[i]);
            }
        }
    }

    printf("\n=== VPMADDUBSW test completed ===\n");
}

int main() {
    test_vpmaddubsw();
    return 0;
}
