#include "base.h"
#include <stdint.h>
#include <stdio.h>

void test_and_reg_reg() {
    printf("\nTesting AND reg, reg\n");
    // 8-bit tests
    uint8_t a, b, res8;
    uint32_t eflags;
    
    uint8_t test_cases8[][2] = {
        {0x00, 0x00}, {0xFF, 0x0F}, {0xAA, 0x55}, {0x0F, 0xF0}
    };
    
    for (size_t i = 0; i < sizeof(test_cases8)/sizeof(test_cases8[0]); i++) {
        a = test_cases8[i][0];
        b = test_cases8[i][1];
        
        CLEAR_FLAGS;
        asm volatile (
            "movb %[a_val], %%al\n\t"
            "andb %[b_val], %%al\n\t"
            "pushfq\n\t"
            "popq %%rbx"
            : "=a" (res8), "=b" (eflags)
            : [a_val] "r" (a), [b_val] "r" (b)
            : "cc"
        );

        printf("AND 0x%02x, 0x%02x = 0x%02x\n", a, b, res8);
        print_eflags_cond(eflags, X_CF|X_OF|X_SF|X_ZF|X_PF);
    }

    // 32-bit tests
    uint32_t a32, b32, res32;
    uint32_t test_cases32[][2] = {
        {0x00000000, 0x00000000}, {0xFFFFFFFF, 0x0000FFFF}, 
        {0xAAAAAAAA, 0x55555555}, {0x0F0F0F0F, 0xF0F0F0F0}
    };
    
    for (size_t i = 0; i < sizeof(test_cases32)/sizeof(test_cases32[0]); i++) {
        a32 = test_cases32[i][0];
        b32 = test_cases32[i][1];
        
        CLEAR_FLAGS;
        asm volatile (
            "movl %[a_val], %%eax\n\t"
            "andl %[b_val], %%eax\n\t"
            "pushfq\n\t"
            "popq %%rbx"
            : "=a" (res32), "=b" (eflags)
            : [a_val] "r" (a32), [b_val] "r" (b32)
            : "cc"
        );

        printf("AND 0x%08x, 0x%08x = 0x%08x\n", a32, b32, res32);
        print_eflags_cond(eflags, X_CF|X_OF|X_SF|X_ZF|X_PF);
    }
}

void test_and_reg_mem() {
    printf("\nTesting AND reg, mem\n");
    uint32_t mem_vals[] = {0x00000000, 0xFFFFFFFF, 0xAAAAAAAA, 0x55555555};
    uint32_t reg_vals[] = {0x00000000, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA};
    
    for (size_t i = 0; i < sizeof(mem_vals)/sizeof(mem_vals[0]); i++) {
        for (size_t j = 0; j < sizeof(reg_vals)/sizeof(reg_vals[0]); j++) {
            uint32_t mem_val = mem_vals[i];
            uint32_t reg_val = reg_vals[j];
            uint32_t res;
            uint32_t eflags;
            
            CLEAR_FLAGS;
            asm volatile (
                "movl %[mem_val], %%eax\n\t"
                "movl %%eax, %[mem]\n\t"
                "movl %[reg_val], %%ebx\n\t"
                "andl %[mem], %%ebx\n\t"
                "pushfq\n\t"
                "popq %%rcx"
                : "=b" (res), "=c" (eflags), [mem] "=m" (mem_val)
                : [mem_val] "r" (mem_val), [reg_val] "r" (reg_val)
                : "rax", "cc"
            );

            printf("AND reg(0x%08x), mem(0x%08x) = 0x%08x\n", reg_val, mem_val, res);
            print_eflags_cond(eflags, X_CF|X_OF|X_SF|X_ZF|X_PF);
        }
    }
}

void test_and_mem_reg() {
    printf("\nTesting AND mem, reg\n");
    uint32_t mem_vals[] = {0x00000000, 0xFFFFFFFF, 0xAAAAAAAA, 0x55555555};
    uint32_t reg_vals[] = {0x00000000, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA};
    
    for (size_t i = 0; i < sizeof(mem_vals)/sizeof(mem_vals[0]); i++) {
        for (size_t j = 0; j < sizeof(reg_vals)/sizeof(reg_vals[0]); j++) {
            uint32_t mem_val = mem_vals[i];
            uint32_t reg_val = reg_vals[j];
            uint32_t res;
            uint32_t eflags;
            
            CLEAR_FLAGS;
            asm volatile (
                "movl %[reg_val], %%ebx\n\t"
                "andl %%ebx, %[mem]\n\t"
                "movl %[mem], %%eax\n\t"
                "pushfq\n\t"
                "popq %%rcx"
                : "=a" (res), "=c" (eflags), [mem] "+m" (mem_val)
                : [reg_val] "r" (reg_val)
                : "rbx", "cc"
            );

            printf("AND mem(0x%08x), reg(0x%08x) = 0x%08x\n", mem_val, reg_val, res);
            print_eflags_cond(eflags, X_CF|X_OF|X_SF|X_ZF|X_PF);
        }
    }
}

void test_and_reg_imm() {
    printf("\nTesting AND reg, imm\n");
    uint64_t reg_vals[] = {0x00000000, 0xFFFFFFFF, 0xAAAAAAAA, 0x55555555};
    uint32_t imm_vals[] = {0x00000000, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA};
    const char* sizes[] = {"b", "w", "l", "q"};
    int size_bits[] = {8, 16, 32, 64};
    
    for (size_t s = 0; s < sizeof(sizes)/sizeof(sizes[0]); s++) {
        for (size_t i = 0; i < sizeof(reg_vals)/sizeof(reg_vals[0]); i++) {
            for (size_t j = 0; j < sizeof(imm_vals)/sizeof(imm_vals[0]); j++) {
                uint64_t reg_val = reg_vals[i];
                uint32_t imm_val = imm_vals[j];
                uint64_t res;
                uint32_t eflags;
                
                CLEAR_FLAGS;
                uint64_t imm = imm_val;
                if (size_bits[s] < 32) {
                    imm = (uint32_t)imm_val & ((1 << size_bits[s]) - 1);
                }
                
                if (size_bits[s] == 8) {
                    asm volatile (
                        "movb %[reg_val], %%al\n\t"
                        "movb %[imm_val], %%cl\n\t"
                        "andb %%cl, %%al\n\t"
                        "pushfq\n\t"
                        "popq %%rbx"
                        : "=a" (res), "=b" (eflags)
                        : [reg_val] "r" ((uint8_t)reg_val), [imm_val] "r" ((uint8_t)imm)
                        : "rcx", "cc"
                    );
                } else if (size_bits[s] == 16) {
                    asm volatile (
                        "movw %[reg_val], %%ax\n\t"
                        "movw %[imm_val], %%cx\n\t"
                        "andw %%cx, %%ax\n\t"
                        "pushfq\n\t"
                        "popq %%rbx"
                        : "=a" (res), "=b" (eflags)
                        : [reg_val] "r" ((uint16_t)reg_val), [imm_val] "r" ((uint16_t)imm)
                        : "rcx", "cc"
                    );
                } else if (size_bits[s] == 32) {
                    asm volatile (
                        "movl %[reg_val], %%eax\n\t"
                        "movl %[imm_val], %%ecx\n\t"
                        "andl %%ecx, %%eax\n\t"
                        "pushfq\n\t"
                        "popq %%rbx"
                        : "=a" (res), "=b" (eflags)
                        : [reg_val] "r" ((uint32_t)reg_val), [imm_val] "r" ((uint32_t)imm)
                        : "rcx", "cc"
                    );
                } else { // 64-bit
                    asm volatile (
                        "movq %[reg_val], %%rax\n\t"
                        "movq %[imm_val], %%rcx\n\t"
                        "andq %%rcx, %%rax\n\t"
                        "pushfq\n\t"
                        "popq %%rbx"
                        : "=a" (res), "=b" (eflags)
                        : [reg_val] "r" (reg_val), [imm_val] "r" (imm)
                        : "rcx", "cc"
                    );
                }

                printf("AND reg(%d-bit) 0x%0*lx, imm 0x%0*x = 0x%0*lx\n", 
                       size_bits[s], 
                       size_bits[s]/4, reg_val,
                       size_bits[s]/4, (unsigned int)imm,
                       size_bits[s]/4, res);
                print_eflags_cond(eflags, X_CF|X_OF|X_SF|X_ZF|X_PF);
            }
        }
    }
}

void test_and_mem_imm() {
    printf("\nTesting AND mem, imm\n");
    uint32_t mem_vals[] = {0x00000000, 0xFFFFFFFF, 0xAAAAAAAA, 0x55555555};
    uint32_t imm_vals[] = {0x00000000, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA};
    
    for (size_t i = 0; i < sizeof(mem_vals)/sizeof(mem_vals[0]); i++) {
        for (size_t j = 0; j < sizeof(imm_vals)/sizeof(imm_vals[0]); j++) {
            uint32_t mem_val = mem_vals[i];
            uint32_t imm_val = imm_vals[j];
            uint32_t res;
            uint32_t eflags;
            
            CLEAR_FLAGS;
            asm volatile (
                "movl %[mem_val], %%eax\n\t"
                "movl %%eax, %[mem]\n\t"
                "movl %[imm_val], %%ecx\n\t"
                "andl %%ecx, %[mem]\n\t"
                "movl %[mem], %%eax\n\t"
                "pushfq\n\t"
                "popq %%rbx"
                : "=a" (res), "=b" (eflags), [mem] "+m" (mem_val)
                : [mem_val] "r" (mem_val), [imm_val] "r" (imm_val)
                : "rcx", "cc"
            );

            printf("AND mem(0x%08x), imm(0x%08x) = 0x%08x\n", mem_val, imm_val, res);
            print_eflags_cond(eflags, X_CF|X_OF|X_SF|X_ZF|X_PF);
        }
    }
}

int main() {
    test_and_reg_reg();
    test_and_reg_mem();
    test_and_mem_reg();
    test_and_reg_imm();
    test_and_mem_imm();
    
    printf("\nAND tests completed.\n");
    return 0;
}
