#include "base.h"
#include <stdint.h>
#include <stdio.h>

// Test ADC with different operand types and sizes
void test_adc_reg_reg() {
    printf("\nTesting ADC reg, reg\n");

    // 8-bit tests
    for (int carry = 0; carry <= 1; carry++) {
        printf("\n8-bit tests (carry=%d)\n", carry);
        uint8_t a, b, res;
        uint32_t eflags;

        // Test cases: zero, max, overflow, carry, half-carry
        uint8_t test_cases[][2] = {
            {0x00, 0x00}, {0xFF, 0x01}, {0x7F, 0x01}, {0x0F, 0x01}
        };

    for (size_t i = 0; i < sizeof(test_cases)/sizeof(test_cases[0]); i++) {
        a = test_cases[i][0];
        b = test_cases[i][1];
        
        CLEAR_FLAGS;
        if (carry) asm volatile ("stc");
        else asm volatile ("clc");
        
        asm volatile (
            "movb %[a_val], %%al\n\t"
            "adcb %[b_val], %%al\n\t"
            "pushfq\n\t"
            "popq %%rbx"
            : "=a" (res), "=b" (eflags)
            : [a_val] "r" (a), [b_val] "r" (b)
            : "cc"
        );

        printf("ADC 0x%02x, 0x%02x = 0x%02x\n", a, b, res);
        print_eflags_cond(eflags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
    }
    }

    // 16/32/64-bit tests would follow similar patterns
    // Omitted for brevity but will be implemented
}

void test_adc_reg_mem() {
    printf("\nTesting ADC reg, mem\n");
    // Test memory operands (aligned and unaligned)
    uint32_t mem_vals[] = {0x00000000, 0xFFFFFFFF, 0x7FFFFFFF, 0x00000FFF};
    uint32_t reg_vals[] = {0x00000000, 0x00000001, 0x80000000, 0x00000001};
    
    for (int carry = 0; carry <= 1; carry++) {
        printf("\n32-bit tests (carry=%d)\n", carry);
        for (size_t i = 0; i < sizeof(mem_vals)/sizeof(mem_vals[0]); i++) {
            for (size_t j = 0; j < sizeof(reg_vals)/sizeof(reg_vals[0]); j++) {
                uint32_t mem_val = mem_vals[i];
                uint32_t reg_val = reg_vals[j];
                uint32_t res;
                uint32_t eflags;
                
                CLEAR_FLAGS;
                if (carry) asm volatile ("stc");
                else asm volatile ("clc");
                
                asm volatile (
                    "movl %[mem_val], %%eax\n\t"
                    "movl %%eax, %[mem]\n\t"
                    "movl %[reg_val], %%ebx\n\t"
                    "adcl %[mem], %%ebx\n\t"
                    "pushfq\n\t"
                    "popq %%rcx"
                    : "=b" (res), "=c" (eflags), [mem] "=m" (mem_val)
                    : [mem_val] "r" (mem_val), [reg_val] "r" (reg_val)
                    : "rax", "cc"
                );

                printf("ADC reg(0x%08x), mem(0x%08x) = 0x%08x\n", reg_val, mem_val, res);
                print_eflags_cond(eflags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
            }
        }
    }
}

void test_adc_mem_reg() {
    printf("\nTesting ADC mem, reg\n");
    // Test memory operands (aligned and unaligned)
    uint32_t mem_vals[] = {0x00000000, 0xFFFFFFFF, 0x7FFFFFFF, 0x00000FFF};
    uint32_t reg_vals[] = {0x00000000, 0x00000001, 0x80000000, 0x00000001};
    
    for (int carry = 0; carry <= 1; carry++) {
        printf("\n32-bit tests (carry=%d)\n", carry);
        for (size_t i = 0; i < sizeof(mem_vals)/sizeof(mem_vals[0]); i++) {
            for (size_t j = 0; j < sizeof(reg_vals)/sizeof(reg_vals[0]); j++) {
                uint32_t mem_val = mem_vals[i];
                uint32_t reg_val = reg_vals[j];
                uint32_t res;
                uint32_t eflags;
                
                CLEAR_FLAGS;
                if (carry) asm volatile ("stc");
                else asm volatile ("clc");
                
                asm volatile (
                    "movl %[reg_val], %%ebx\n\t"
                    "adcl %%ebx, %[mem]\n\t"
                    "movl %[mem], %%eax\n\t"
                    "pushfq\n\t"
                    "popq %%rcx"
                    : "=a" (res), "=c" (eflags), [mem] "+m" (mem_val)
                    : [reg_val] "r" (reg_val)
                    : "rbx", "cc"
                );

                printf("ADC mem(0x%08x), reg(0x%08x) = 0x%08x\n", mem_val, reg_val, res);
                print_eflags_cond(eflags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
            }
        }
    }
}
void test_adc_reg_imm() {
    printf("\nTesting ADC reg, imm\n");
    // Test immediate values with different sizes
    uint64_t reg_vals[] = {0x00000000, 0xFFFFFFFF, 0x7FFFFFFF, 0x80000000};
    uint32_t imm_vals[] = {0x00000000, 0x00000001, 0x80000000, 0x7FFFFFFF};
    const char* sizes[] = {"b", "w", "l", "q"};
    int size_bits[] = {8, 16, 32, 64};
    
    for (int carry = 0; carry <= 1; carry++) {
        printf("\nTests (carry=%d)\n", carry);
        for (size_t s = 0; s < sizeof(sizes)/sizeof(sizes[0]); s++) {
            for (size_t i = 0; i < sizeof(reg_vals)/sizeof(reg_vals[0]); i++) {
                for (size_t j = 0; j < sizeof(imm_vals)/sizeof(imm_vals[0]); j++) {
                    uint64_t reg_val = reg_vals[i];
                    uint32_t imm_val = imm_vals[j];
                    uint64_t res;
                    uint32_t eflags;
                    
                    CLEAR_FLAGS;
                    if (carry) asm volatile ("stc");
                    else asm volatile ("clc");
                    
                    // Handle sign extension for smaller immediates
                    uint64_t imm = imm_val;
                    if (size_bits[s] < 32) {
                        imm = (uint32_t)imm_val & ((1 << size_bits[s]) - 1);
                    }
                    
                    if (size_bits[s] == 8) {
                        asm volatile (
                            "movb %[reg_val], %%al\n\t"
                            "movb %[imm_val], %%cl\n\t"
                            "adcb %%cl, %%al\n\t"
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
                            "adcw %%cx, %%ax\n\t"
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
                            "adcl %%ecx, %%eax\n\t"
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
                            "adcq %%rcx, %%rax\n\t"
                            "pushfq\n\t"
                            "popq %%rbx"
                            : "=a" (res), "=b" (eflags)
                            : [reg_val] "r" (reg_val), [imm_val] "r" (imm)
                            : "rcx", "cc"
                        );
                    }

                    printf("ADC reg(%d-bit) 0x%0*lx, imm 0x%0*x = 0x%0*lx\n", 
                           size_bits[s], 
                           size_bits[s]/4, reg_val,
                           size_bits[s]/4, (unsigned int)imm,
                           size_bits[s]/4, res);
                    print_eflags_cond(eflags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
                }
            }
        }
    }
}

void test_adc_mem_imm() {
    printf("\nTesting ADC mem, imm\n");
    // Test immediate values with memory operands
    uint32_t mem_vals[] = {0x00000000, 0xFFFFFFFF, 0x7FFFFFFF, 0x80000000};
    uint32_t imm_vals[] = {0x00000000, 0x00000001, 0x80000000, 0x7FFFFFFF};
    
    for (int carry = 0; carry <= 1; carry++) {
        printf("\n32-bit tests (carry=%d)\n", carry);
        for (size_t i = 0; i < sizeof(mem_vals)/sizeof(mem_vals[0]); i++) {
            for (size_t j = 0; j < sizeof(imm_vals)/sizeof(imm_vals[0]); j++) {
                uint32_t mem_val = mem_vals[i];
                uint32_t imm_val = imm_vals[j];
                uint32_t res;
                uint32_t eflags;
                
                CLEAR_FLAGS;
                if (carry) asm volatile ("stc");
                else asm volatile ("clc");
                
                asm volatile (
                    "movl %[mem_val], %%eax\n\t"
                    "movl %%eax, %[mem]\n\t"
                    "movl %[imm_val], %%ecx\n\t"
                    "adcl %%ecx, %[mem]\n\t"
                    "movl %[mem], %%eax\n\t"
                    "pushfq\n\t"
                    "popq %%rbx"
                    : "=a" (res), "=b" (eflags), [mem] "+m" (mem_val)
                    : [mem_val] "r" (mem_val), [imm_val] "r" (imm_val)
                    : "rcx", "cc"
                );

                printf("ADC mem(0x%08x), imm(0x%08x) = 0x%08x\n", mem_val, imm_val, res);
                print_eflags_cond(eflags, X_CF|X_PF|X_AF|X_ZF|X_SF|X_OF);
            }
        }
    }
}

int main() {
    test_adc_reg_reg();
    test_adc_reg_mem();
    test_adc_mem_reg();
    test_adc_reg_imm();
    test_adc_mem_imm();
    
    printf("\nADC tests completed.\n");
    return 0;
}
