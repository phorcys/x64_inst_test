#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 测试PINSTRW指令的不同操作数组合
typedef union {
    uint64_t mmx[2];
    uint32_t dword[4];
    uint16_t word[8];
    uint8_t byte[16];
} MMXReg;

void print_xmm(const char* name, MMXReg reg) {
    printf("%s: ", name);
    for (int i = 0; i < 8; i++) {
        printf("%04X ", reg.word[i]);
    }
    printf("\n");
}

int main() {
    int fail = 0;
    MMXReg src = { .word = {0x1111,0x2222,0x3333,0x4444,0x5555,0x6666,0x7777,0x8888} };
    MMXReg dst = { .word = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF} };
    uint16_t mem16 = 0xABCD;
    uint32_t mem32 = 0x12345678;
    uint64_t mem64 = 0x9ABCDEF012345678;

    // 初始化寄存器值
    uint32_t eax_value = 0xCAFEBABE;
    
        // 输出结果比对
    printf("=== PINSTRW Test ===\n");
    print_xmm("Original", src);
    print_xmm("Modified", dst);


    // 寄存器到MMX测试（使用32位寄存器的低16位）
    asm volatile (
        "movq %[src], %%mm0\n\t"
        "pinsrw $3, %k[regval], %%mm0\n\t"  // 使用32位寄存器
        "movq %%mm0, %[dst]\n\t"
        : [dst] "=m" (dst.mmx[0])
        : [src] "m" (src.mmx[0]),
          [regval] "a" (eax_value)       // 约束到EAX寄存器
        :
    );
    // 寄存器操作验证（位置3）
    if (dst.word[3] != (eax_value & 0xFFFF)) {
        printf("[Register] Error at position 3! Expected %04X, got %04X  full %016llx\n",
            eax_value & 0xFFFF, dst.word[3], dst.mmx[0]);
        fail++;
    }


    // 内存操作数测试（多种位宽）
    asm volatile (
        "pinsrw $2, %1, %0\n\t"        // 16位内存操作数
        : "=y" (dst.mmx[0])
        : "m" (*(uint16_t*)&mem16)
        : "memory"
    );
        // 16位内存验证（位置2）
    if (dst.word[2] != mem16) {
        printf("[16-bit Mem] Error at position 2! Expected %04X, got %04X   full %016llx\n",
            mem16, dst.word[2], dst.mmx[0]);
        fail++;
    }
    
    // 测试32位内存访问（取低16位）
    asm volatile (
        "pinsrw $1, %1, %0\n\t"        // 32位内存操作数
        : "+y" (dst.mmx[0])
        : "m" (*(uint32_t*)&mem32)
        : "memory"
    );
    
        // 32位内存验证（位置4，取低16位）
    if (dst.word[1] != (mem32 & 0xFFFF)) {
        printf("[32-bit Mem] Error at position 1! Expected %04X, got %04X  full %016llx\n",
            (uint16_t)(mem32 & 0xFFFF), dst.word[1], dst.mmx[0]);
        fail++;
    }
    // 测试64位内存访问（取低16位）
    asm volatile (
        "pinsrw $0, %1, %0\n\t"        // 64位内存操作数
        : "+y" (dst.mmx[0])
        : "m" (*(uint64_t*)&mem64)
        : "memory"
    );




    
    // 64位内存验证（位置6，取低16位）
    if (dst.word[0] != (mem64 & 0xFFFF)) {
        printf("[64-bit Mem] Error at position 0! Expected %04X, got %04X  full %016llx\n",
            (uint16_t)(mem64 & 0xFFFF), dst.word[0], dst.mmx[0]);
        fail++;
    }
    
    printf("\nTest Summary: %s - %d failures\n", 
           fail ? "FAIL" : "PASS", fail);
    return fail ? 1 : 0;
}
