#include "base.h"
#include <stdint.h>
#include <stdio.h>

void test_btc_reg_reg() {
    printf("\nTesting BTC with register and register operand\n");
    
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
    
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        uint64_t val = values[i];
        
        for (uint64_t bit_offset = 0; bit_offset < 64; bit_offset++) {
            uint8_t result;
            uint64_t expected_cf = (val >> bit_offset) & 1;
            uint64_t expected_val = val ^ (1ULL << bit_offset);
            uint64_t original_val = val;
            
            asm volatile (
                "btc %[bit], %[val]\n\t"
                "setc %[res]"
                : [res] "=r" (result), [val] "+r" (val)
                : [bit] "r" (bit_offset)
                : "cc"
            );
            
            if (result != expected_cf || val != expected_val) {
                printf("Error: BTC(0x%016lx, %lu) = %d, CF: %lu, expected CF: %lu, result: 0x%016lx, expected: 0x%016lx\n",
                       original_val, bit_offset, result, (uint64_t)result, (uint64_t)expected_cf, 
                       val, expected_val);
            }
        }
    }
    printf("Register-register BTC tests completed\n");
}

void test_btc_reg_imm() {
    printf("\nTesting BTC with register and immediate operand\n");
    
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
            uint64_t expected_cf = (val >> bit_offset) & 1;
            uint64_t expected_val = val ^ (1ULL << bit_offset);
            uint64_t original_val = val;
            
            // Use immediate value for BTC instruction
            switch (bit_offset) {
                case 0:
                    asm volatile ("btc $0, %[val]\n\tsetc %[res]" : [res] "=r" (result), [val] "+r" (val) : : "cc");
                    break;
                case 1:
                    asm volatile ("btc $1, %[val]\n\tsetc %[res]" : [res] "=r" (result), [val] "+r" (val) : : "cc");
                    break;
                case 7:
                    asm volatile ("btc $7, %[val]\n\tsetc %[res]" : [res] "=r" (result), [val] "+r" (val) : : "cc");
                    break;
                case 15:
                    asm volatile ("btc $15, %[val]\n\tsetc %[res]" : [res] "=r" (result), [val] "+r" (val) : : "cc");
                    break;
                case 23:
                    asm volatile ("btc $23, %[val]\n\tsetc %[res]" : [res] "=r" (result), [val] "+r" (val) : : "cc");
                    break;
                case 31:
                    asm volatile ("btc $31, %[val]\n\tsetc %[res]" : [res] "=r" (result), [val] "+r" (val) : : "cc");
                    break;
                case 32:
                    asm volatile ("btc $32, %[val]\n\tsetc %[res]" : [res] "=r" (result), [val] "+r" (val) : : "cc");
                    break;
                case 33:
                    asm volatile ("btc $33, %[val]\n\tsetc %[res]" : [res] "=r" (result), [val] "+r" (val) : : "cc");
                    break;
                case 55:
                    asm volatile ("btc $55, %[val]\n\tsetc %[res]" : [res] "=r" (result), [val] "+r" (val) : : "cc");
                    break;
                case 63:
                    asm volatile ("btc $63, %[val]\n\tsetc %[res]" : [res] "=r" (result), [val] "+r" (val) : : "cc");
                    break;
                default:
                    result = 0;
                    break;
            }
            
            printf("BTC(0x%016lx, %d) = %d [CF: %s], result: 0x%016lx [%s]\n",
                   original_val, bit_offset, result,
                   result == expected_cf ? "PASS" : "FAIL",
                   val,
                   val == expected_val ? "PASS" : "FAIL");
        }
    }
}

void test_btc_mem_reg() {
    printf("\nTesting BTC with memory and register operand\n");
    
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
            uint64_t expected_cf = (val >> bit_offset) & 1;
            uint64_t expected_val = val ^ (1ULL << bit_offset);
            volatile uint64_t mem_val = val; // Reset memory for each test
            
            asm volatile (
                "btc %[bit], %[mem]\n\t"
                "setc %[res]"
                : [res] "=r" (result), [mem] "+m" (mem_val)
                : [bit] "r" (bit_offset)
                : "cc"
            );
            
            printf("BTC(mem=0x%016lx, %d) = %d [CF: %s], result: 0x%016lx [%s]\n",
                   val, bit_offset, result,
                   result == expected_cf ? "PASS" : "FAIL",
                   mem_val,
                   mem_val == expected_val ? "PASS" : "FAIL");
        }
    }
}

void test_btc_mem_imm() {
    printf("\nTesting BTC with memory and immediate operand\n");
    
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
            uint64_t expected_cf = (val >> bit_offset) & 1;
            uint64_t expected_val = val ^ (1ULL << bit_offset);
            volatile uint64_t mem_val = val; // Reset memory for each test
            
            // Use immediate value for BTC instruction
            switch (bit_offset) {
                case 0:
                    asm volatile ("btcq $0, %[mem]\n\tsetc %[res]" : [res] "=r" (result), [mem] "+m" (mem_val) : : "cc");
                    break;
                case 1:
                    asm volatile ("btcq $1, %[mem]\n\tsetc %[res]" : [res] "=r" (result), [mem] "+m" (mem_val) : : "cc");
                    break;
                case 7:
                    asm volatile ("btcq $7, %[mem]\n\tsetc %[res]" : [res] "=r" (result), [mem] "+m" (mem_val) : : "cc");
                    break;
                case 15:
                    asm volatile ("btcq $15, %[mem]\n\tsetc %[res]" : [res] "=r" (result), [mem] "+m" (mem_val) : : "cc");
                    break;
                case 23:
                    asm volatile ("btcq $23, %[mem]\n\tsetc %[res]" : [res] "=r" (result), [mem] "+m" (mem_val) : : "cc");
                    break;
                case 31:
                    asm volatile ("btcq $31, %[mem]\n\tsetc %[res]" : [res] "=r" (result), [mem] "+m" (mem_val) : : "cc");
                    break;
                case 32:
                    asm volatile ("btcq $32, %[mem]\n\tsetc %[res]" : [res] "=r" (result), [mem] "+m" (mem_val) : : "cc");
                    break;
                case 33:
                    asm volatile ("btcq $33, %[mem]\n\tsetc %[res]" : [res] "=r" (result), [mem] "+m" (mem_val) : : "cc");
                    break;
                case 55:
                    asm volatile ("btcq $55, %[mem]\n\tsetc %[res]" : [res] "=r" (result), [mem] "+m" (mem_val) : : "cc");
                    break;
                case 63:
                    asm volatile ("btcq $63, %[mem]\n\tsetc %[res]" : [res] "=r" (result), [mem] "+m" (mem_val) : : "cc");
                    break;
                default:
                    result = 0;
                    break;
            }
            
            printf("BTC(mem=0x%016lx, %d) = %d [CF: %s], result: 0x%016lx [%s]\n",
                   val, bit_offset, result,
                   result == expected_cf ? "PASS" : "FAIL",
                   mem_val,
                   mem_val == expected_val ? "PASS" : "FAIL");
        }
    }
}

int main() {
    test_btc_reg_reg();
    test_btc_reg_imm();
    test_btc_mem_reg();
    test_btc_mem_imm();
    
    printf("\nBTC tests completed.\n");
    return 0;
}
