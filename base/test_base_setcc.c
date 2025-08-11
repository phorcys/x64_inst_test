#include <stdio.h>
#include <stdint.h>
#include "base.h"

int main() {
    printf("SETcc instruction test\n");
    printf("======================\n");
    
    int errors = 0;
    uint8_t result;
    uint64_t flags;
    
    // Helper macro to test SETcc instructions
    #define TEST_SETCC(cc, condition_str, setup_flags, expected) \
    do { \
        __asm__ __volatile__ ( \
            setup_flags \
            "set" #cc " %[res]\n\t" \
            : [res] "=r" (result) \
            : \
            : "cc" \
        ); \
        if (result != expected) { \
            printf("Error: SET" #cc " (" condition_str ") expected %d, got %d\n", expected, result); \
            errors++; \
        } else { \
            printf("SET" #cc " (" condition_str "): PASS\n"); \
        } \
    } while(0)
    
    // Test SETE (ZF=1)
    TEST_SETCC(e, "equal", "xor %%eax, %%eax\n\tcmp $0, %%eax\n\t", 1);
    
    // Test SETNE (ZF=0)
    TEST_SETCC(ne, "not equal", "mov $1, %%eax\n\tcmp $0, %%eax\n\t", 1);
    
    // Test SETG (ZF=0 and SF=OF)
    TEST_SETCC(g, "greater (signed)", "mov $5, %%eax\n\tcmp $2, %%eax\n\t", 1);
    
    // Test SETL (SF != OF)
    TEST_SETCC(l, "less (signed)", "mov $2, %%eax\n\tcmp $5, %%eax\n\t", 1);
    
    // Test SETA (CF=0 and ZF=0)
    TEST_SETCC(a, "above (unsigned)", "mov $5, %%eax\n\tcmp $2, %%eax\n\t", 1);
    
    // Test SETB (CF=1)
    TEST_SETCC(b, "below (unsigned)", "mov $2, %%eax\n\tcmp $5, %%eax\n\t", 1);
    
    // Test SETS (SF=1)
    TEST_SETCC(s, "sign", "mov $0xFFFFFFFF, %%eax\n\tadd $1, %%eax\n\t", 0);
    
    // Test SETNS (SF=0)
    TEST_SETCC(ns, "no sign", "mov $5, %%eax\n\t", 1);
    
    // Test SETO (OF=1)
    TEST_SETCC(o, "overflow", "mov $0x7FFFFFFF, %%eax\n\tadd $1, %%eax\n\t", 1);
    
    // Test SETNO (OF=0)
    TEST_SETCC(no, "no overflow", "mov $5, %%eax\n\tadd $1, %%eax\n\t", 1);
    
    // Test SETP (PF=1) - 0x03 has 2 bits set (even parity)
    TEST_SETCC(p, "parity", "mov $0x03, %%eax\n\t test %%eax, %%eax\n\t", 1);
    
    // Test SETNP (PF=0) - 0x01 has 1 bit set (odd parity)
    TEST_SETCC(np, "no parity", "mov $0x01, %%eax\n\t test %%eax, %%eax\n\t", 1);
    
    // Test memory operand
    uint8_t mem_result = 0;
    __asm__ __volatile__ (
        "mov $5, %%eax\n\t"
        "cmp $2, %%eax\n\t"
        "setg %[mem]"
        : [mem] "=m" (mem_result)
        :
        : "eax", "cc"
    );
    if (mem_result != 1) {
        printf("Error: SETG with memory operand expected 1, got %d\n", mem_result);
        errors++;
    } else {
        printf("SETG with memory operand: PASS\n");
    }

    printf("\nSETcc test completed\n");
    printf("====================\n");
    printf("Total tests: 13\n");
    printf("Passed: %d\n", 13 - errors);
    printf("Failed: %d\n", errors);
    
    return errors ? 1 : 0;
}
