#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef union {
    uint64_t mmx[2];
    uint16_t word[8];
    uint32_t dword[4];
} MMXReg;

void print_mmx(const char* name, MMXReg reg) {
    printf("%s: ", name);
    for (int i = 0; i < 8; i++) {
        printf("%04X ", reg.word[i]);
    }
    printf("\n");
}

int main() {
    int fail = 0;
    MMXReg src = { .word = {0x1234,0x5678,0x9ABC,0xDEF0,0xCAFE,0xBABE,0xDEAD,0xBEEF} };
    uint16_t mem16 = 0;
    uint32_t mem32 = 0;
    uint64_t mem64 = 0;
    uint32_t eax_value = 0;
    printf("=== PEXTRW Test ===\n");
    print_mmx("Source", src);


    // 测试寄存器到寄存器
    asm volatile (
        "pextrw $3, %[src], %[reg]\n\t"
        : [reg] "=r" (eax_value)
        : [src] "y" (src.mmx[0])
        : "mm0"
    );
    // 验证寄存器到寄存器
    if ((eax_value & 0xFFFF) != src.word[3]) {
        printf("[Reg2Reg] Error! Expected %04X, got %04X\n",
            src.word[3], eax_value & 0xFFFF);
        fail++;
    }
   

    printf("\nTest Summary: %s - %d failures\n", 
           fail ? "FAIL" : "PASS", fail);

    return fail ? 1 : 0;
}
