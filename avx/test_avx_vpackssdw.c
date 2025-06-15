#include "avx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试VPACKSSDW指令
void test_vpackssdw() {
    printf("Testing VPACKSSDW (pack with signed saturation)\n");
    
    // 测试128位操作
    {
        printf("Testing 128-bit operations:\n");
        
        // 寄存器-寄存器操作
        {
            int32_t src1[4] = {0x7FFFFFFF, 0x80000000, 12345678, -9876543};
            int32_t src2[4] = {0x00000000, 0xFFFFFFFF, 5555555, -3333333};
            int16_t dst[8] = {0};
            
            __asm__ __volatile__(
                "vmovdqu %1, %%xmm1\n\t"
                "vmovdqu %2, %%xmm2\n\t"
                "vpackssdw %%xmm2, %%xmm1, %%xmm0\n\t"
                "vmovdqu %%xmm0, %0\n\t"
                : "=m"(dst)
                : "m"(src1), "m"(src2)
                : "xmm0", "xmm1", "xmm2"
            );
            
            printf("Reg-Reg test:\n");
            printf("Src1: ");
            for(int i=0; i<4; i++) printf("%08x ", src1[i]);
            printf("\nSrc2: ");
            for(int i=0; i<4; i++) printf("%08x ", src2[i]);
            printf("\nResult: ");
            for(int i=0; i<8; i++) printf("%04x ", dst[i]);
            printf("\n");
            
            // 检查饱和情况
            if(dst[0] != 0x7FFF || dst[1] != (int16_t)0x8000) {
                printf("Error: Saturation handling failed!\n");
            }
        }
        
        // 寄存器-内存操作
        {
            int32_t src1[4] = {1000000, -1000000, 2147483647, -2147483648};
            int32_t src2[4] ALIGNED(16) = {500000, -500000, 0, -1};
            int16_t dst[8] = {0};
            
            __asm__ __volatile__(
                "vmovdqu %1, %%xmm1\n\t"
                "vpackssdw %2, %%xmm1, %%xmm0\n\t"
                "vmovdqu %%xmm0, %0\n\t"
                : "=m"(dst)
                : "m"(src1), "m"(src2)
                : "xmm0", "xmm1"
            );
            
            printf("\nReg-Mem test:\n");
            printf("Src1: ");
            for(int i=0; i<4; i++) printf("%08x ", src1[i]);
            printf("\nSrc2: ");
            for(int i=0; i<4; i++) printf("%08x ", src2[i]);
            printf("\nResult: ");
            for(int i=0; i<8; i++) printf("%04x ", dst[i]);
            printf("\n");
        }
    }
    
    // 测试256位操作
    {
        printf("\nTesting 256-bit operations:\n");
        
        // 寄存器-寄存器操作
        {
            int32_t src1[8] = {0x7FFFFFFF, 0x80000000, 12345678, -9876543, 
                              5555555, -3333333, 1000000000, -1000000000};
            int32_t src2[8] = {0x00000000, 0xFFFFFFFF, 5555555, -3333333,
                             0x7FFFFFFF, 0x80000000, 500000000, -500000000};
            int16_t dst[16] = {0};
            
            __asm__ __volatile__(
                "vmovdqu %1, %%ymm1\n\t"
                "vmovdqu %2, %%ymm2\n\t"
                "vpackssdw %%ymm2, %%ymm1, %%ymm0\n\t"
                "vmovdqu %%ymm0, %0\n\t"
                : "=m"(dst)
                : "m"(src1), "m"(src2)
                : "ymm0", "ymm1", "ymm2"
            );
            
            printf("Reg-Reg test:\n");
            printf("Src1: ");
            for(int i=0; i<8; i++) printf("%08x ", src1[i]);
            printf("\nSrc2: ");
            for(int i=0; i<8; i++) printf("%08x ", src2[i]);
            printf("\nResult: ");
            for(int i=0; i<16; i++) printf("%04x ", dst[i]);
            printf("\n");
        }
    }
    
    printf("\nVPACKSSDW tests completed.\n");
}

int main() {
    test_vpackssdw();
    return 0;
}
