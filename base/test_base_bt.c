#include "base.h"
#include <stdint.h>
#include <stdio.h>

void test_bt_reg_imm() {
    printf("\nTesting BT with register and immediate operand\n");
    
    uint64_t values[] = {
        0x0000000000000000, 
        0x8000000000000000, 
        0x0000000100000000,
        0x0000800000000000,
        0xAAAAAAAAAAAAAAAA,
        0x5555555555555555,
        0x123456789ABCDEF0,
        0xFEDCBA9876543210
    };
    
    int bit_offsets[] = {0, 1, 31, 32, 33, 63, 7, 15, 23, 55};
    
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        uint64_t val = values[i];
        
        for (size_t j = 0; j < sizeof(bit_offsets)/sizeof(bit_offsets[0]); j++) {
            int bit_offset = bit_offsets[j];
            uint8_t result;
            uint64_t expected = (val >> bit_offset) & 1;
            
            // Use immediate value for BT instruction
            switch (bit_offset) {
                case 0:
                    asm volatile ("bt $0, %[val]\n\tsetc %[res]" : [res] "=r" (result) : [val] "r" (val) : "cc");
                    break;
                case 1:
                    asm volatile ("bt $1, %[val]\n\tsetc %[res]" : [res] "=r" (result) : [val] "r" (val) : "cc");
                    break;
                case 7:
                    asm volatile ("bt $7, %[val]\n\tsetc %[res]" : [res] "=r" (result) : [val] "r" (val) : "cc");
                    break;
                case 15:
                    asm volatile ("bt $15, %[val]\n\tsetc %[res]" : [res] "=r" (result) : [val] "r" (val) : "cc");
                    break;
                case 23:
                    asm volatile ("bt $23, %[val]\n\tsetc %[res]" : [res] "=r" (result) : [val] "r" (val) : "cc");
                    break;
                case 31:
                    asm volatile ("bt $31, %[val]\n\tsetc %[res]" : [res] "=r" (result) : [val] "r" (val) : "cc");
                    break;
                case 32:
                    asm volatile ("bt $32, %[val]\n\tsetc %[res]" : [res] "=r" (result) : [val] "r" (val) : "cc");
                    break;
                case 33:
                    asm volatile ("bt $33, %[val]\n\tsetc %[res]" : [res] "=r" (result) : [val] "r" (val) : "cc");
                    break;
                case 55:
                    asm volatile ("bt $55, %[val]\n\tsetc %[res]" : [res] "=r" (result) : [val] "r" (val) : "cc");
                    break;
                case 63:
                    asm volatile ("bt $63, %[val]\n\tsetc %[res]" : [res] "=r" (result) : [val] "r" (val) : "cc");
                    break;
                default:
                    result = 0;
                    break;
            }
            
            printf("BT(0x%016lx, %d) = %d [%s]\n",
                   val, bit_offset, result, 
                   result == expected ? "PASS" : "FAIL");
        }
    }
}

void test_bt_mem_imm() {
    printf("\nTesting BT with memory and immediate operand\n");
    
    uint64_t values[] = {
        0x0000000000000000, 
        0x8000000000000000, 
        0x0000000100000000,
        0x0000800000000000,
        0xAAAAAAAAAAAAAAAA,
        0x5555555555555555,
        0x123456789ABCDEF0,
        0xFEDCBA9876543210
    };
    
    int bit_offsets[] = {0, 1, 31, 32, 33, 63, 7, 15, 23, 55};
    
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        uint64_t val = values[i];
        volatile uint64_t mem_val = val;
        
        for (size_t j = 0; j < sizeof(bit_offsets)/sizeof(bit_offsets[0]); j++) {
            int bit_offset = bit_offsets[j];
            uint8_t result;
            uint64_t expected = (val >> bit_offset) & 1;
            
            // Use immediate value for BT instruction
            switch (bit_offset) {
                case 0:
                    asm volatile ("btq $0, %[mem]\n\tsetc %[res]" : [res] "=r" (result) : [mem] "m" (mem_val) : "cc");
                    break;
                case 1:
                    asm volatile ("btq $1, %[mem]\n\tsetc %[res]" : [res] "=r" (result) : [mem] "m" (mem_val) : "cc");
                    break;
                case 7:
                    asm volatile ("btq $7, %[mem]\n\tsetc %[res]" : [res] "=r" (result) : [mem] "m" (mem_val) : "cc");
                    break;
                case 15:
                    asm volatile ("btq $15, %[mem]\n\tsetc %[res]" : [res] "=r" (result) : [mem] "m" (mem_val) : "cc");
                    break;
                case 23:
                    asm volatile ("btq $23, %[mem]\n\tsetc %[res]" : [res] "=r" (result) : [mem] "m" (mem_val) : "cc");
                    break;
                case 31:
                    asm volatile ("btq $31, %[mem]\n\tsetc %[res]" : [res] "=r" (result) : [mem] "m" (mem_val) : "cc");
                    break;
                case 32:
                    asm volatile ("btq $32, %[mem]\n\tsetc %[res]" : [res] "=r" (result) : [mem] "m" (mem_val) : "cc");
                    break;
                case 33:
                    asm volatile ("btq $33, %[mem]\n\tsetc %[res]" : [res] "=r" (result) : [mem] "m" (mem_val) : "cc");
                    break;
                case 55:
                    asm volatile ("btq $55, %[mem]\n\tsetc %[res]" : [res] "=r" (result) : [mem] "m" (mem_val) : "cc");
                    break;
                case 63:
                    asm volatile ("btq $63, %[mem]\n\tsetc %[res]" : [res] "=r" (result) : [mem] "m" (mem_val) : "cc");
                    break;
                default:
                    result = 0;
                    break;
            }
            
            printf("BT(mem=0x%016lx, %d) = %d [%s]\n",
                   val, bit_offset, result, 
                   result == expected ? "PASS" : "FAIL");
        }
    }
}

int main() {
    test_bt_reg_imm();
    test_bt_mem_imm();
    
    printf("\nBT tests completed.\n");
    return 0;
}
