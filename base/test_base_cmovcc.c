#include <stdio.h>
#include <stdint.h>
#include "base.h"

// Remove the custom eflags functions since they're defined in base.h

// Enhanced macro to test CMOV conditions
#define TEST_CMOV_CONDITION(cc, condition_str, true_flags, false_flags) \
    do { \
        int passed = 0, total = 0; \
        printf("\nTesting CMOV%s (%s):\n", #cc, condition_str); \
        printf("----------------------------\n"); \
        \
        /* Test 16-bit register to register */ \
        for (int i = 0; i < 2; i++) { \
            int condition = i == 0; \
            uint64_t flags = condition ? true_flags : false_flags; \
            uint16_t src = 0x1234; \
            uint16_t dest = 0x5678; \
            uint16_t expected = condition ? src : dest; \
            set_eflags(flags); \
            uint64_t flags_before = get_eflags(); \
            asm volatile ("cmov" #cc " %1, %0" : "+r"(dest) : "r"(src)); \
            uint64_t flags_after = get_eflags(); \
            total++; \
            if (dest == expected && flags_after == flags_before) { \
                passed++; \
            } else { \
                printf("  16-bit %s: FAIL\n", condition ? "true" : "false"); \
                printf("    Expected: 0x%04X, Got: 0x%04X\n", expected, dest); \
                printf("    Flags before: 0x%04lX, After: 0x%04lX\n", flags_before & 0xFFFF, flags_after & 0xFFFF); \
            } \
        } \
        \
        /* Test 32-bit register to register */ \
        for (int i = 0; i < 2; i++) { \
            int condition = i == 0; \
            uint64_t flags = condition ? true_flags : false_flags; \
            uint32_t src = 0x12345678; \
            uint32_t dest = 0x87654321; \
            uint32_t expected = condition ? src : dest; \
            set_eflags(flags); \
            uint64_t flags_before = get_eflags(); \
            asm volatile ("cmov" #cc " %1, %0" : "+r"(dest) : "r"(src)); \
            uint64_t flags_after = get_eflags(); \
            total++; \
            if (dest == expected && flags_after == flags_before) { \
                passed++; \
            } else { \
                printf("  32-bit %s: FAIL\n", condition ? "true" : "false"); \
                printf("    Expected: 0x%08X, Got: 0x%08X\n", expected, dest); \
                printf("    Flags before: 0x%08lX, After: 0x%08lX\n", flags_before & 0xFFFFFFFF, flags_after & 0xFFFFFFFF); \
            } \
        } \
        \
        /* Test 64-bit register to register */ \
        for (int i = 0; i < 2; i++) { \
            int condition = i == 0; \
            uint64_t flags = condition ? true_flags : false_flags; \
            uint64_t src = 0x123456789ABCDEF0; \
            uint64_t dest = 0x0FEDCBA987654321; \
            uint64_t expected = condition ? src : dest; \
            set_eflags(flags); \
            uint64_t flags_before = get_eflags(); \
            asm volatile ("cmov" #cc " %1, %0" : "+r"(dest) : "r"(src)); \
            uint64_t flags_after = get_eflags(); \
            total++; \
            if (dest == expected && flags_after == flags_before) { \
                passed++; \
            } else { \
                printf("  64-bit %s: FAIL\n", condition ? "true" : "false"); \
                printf("    Expected: 0x%016lX, Got: 0x%016lX\n", expected, dest); \
                printf("    Flags before: 0x%016lX, After: 0x%016lX\n", flags_before, flags_after); \
            } \
        } \
        \
        /* Test memory to register */ \
        for (int i = 0; i < 2; i++) { \
            int condition = i == 0; \
            uint64_t flags = condition ? true_flags : false_flags; \
            uint64_t mem_src = 0xDEADBEEFCAFEBABE; \
            uint64_t dest = 0x0123456789ABCDEF; \
            uint64_t expected = condition ? mem_src : dest; \
            set_eflags(flags); \
            uint64_t flags_before = get_eflags(); \
            asm volatile ("cmov" #cc " %1, %0" : "+r"(dest) : "m"(mem_src)); \
            uint64_t flags_after = get_eflags(); \
            total++; \
            if (dest == expected && flags_after == flags_before) { \
                passed++; \
            } else { \
                printf("  Mem-operand %s: FAIL\n", condition ? "true" : "false"); \
                printf("    Expected: 0x%016lX, Got: 0x%016lX\n", expected, dest); \
                printf("    Flags before: 0x%016lX, After: 0x%016lX\n", flags_before, flags_after); \
            } \
        } \
        \
        /* Test same source/destination register */ \
        for (int i = 0; i < 2; i++) { \
            int condition = i == 0; \
            uint64_t flags = condition ? true_flags : false_flags; \
            uint64_t value = 0x1122334455667788; \
            uint64_t expected = value; \
            set_eflags(flags); \
            uint64_t flags_before = get_eflags(); \
            asm volatile ("cmov" #cc " %0, %0" : "+r"(value)); \
            uint64_t flags_after = get_eflags(); \
            total++; \
            if (value == expected && flags_after == flags_before) { \
                passed++; \
            } else { \
                printf("  Same-reg %s: FAIL\n", condition ? "true" : "false"); \
                printf("    Expected: 0x%016lX, Got: 0x%016lX\n", expected, value); \
                printf("    Flags before: 0x%016lX, After: 0x%016lX\n", flags_before, flags_after); \
            } \
        } \
        \
        /* Test boundary values */ \
        uint64_t boundaries[] = {0, UINT64_MAX, 0x8000000000000000, 0x7FFFFFFFFFFFFFFF}; \
        for (int i = 0; i < 4; i++) { \
            uint64_t src = boundaries[i]; \
            uint64_t dest = ~boundaries[i]; \
            set_eflags(true_flags); \
            uint64_t flags_before = get_eflags(); \
            asm volatile ("cmov" #cc " %1, %0" : "+r"(dest) : "r"(src)); \
            uint64_t flags_after = get_eflags(); \
            total++; \
            if (dest == src && flags_after == flags_before) { \
                passed++; \
            } else { \
                printf("  Boundary %d: FAIL\n", i+1); \
                printf("    Expected: 0x%016lX, Got: 0x%016lX\n", src, dest); \
                printf("    Flags before: 0x%016lX, After: 0x%016lX\n", flags_before, flags_after); \
            } \
        } \
        \
        printf("  Passed: %d/%d\n", passed, total); \
    } while(0)

// Test all CMOVcc variants
static void test_cmov_conditions() {
    printf("Testing CMOVcc instructions:\n");
    printf("============================\n\n");

    TEST_CMOV_CONDITION(a, "Above (CF=0 and ZF=0)", 0, X_CF|X_ZF);
    TEST_CMOV_CONDITION(ae, "Above or Equal (CF=0)", 0, X_CF);
    TEST_CMOV_CONDITION(b, "Below (CF=1)", X_CF, 0);
    TEST_CMOV_CONDITION(be, "Below or Equal (CF=1 or ZF=1)", X_CF|X_ZF, 0);
    TEST_CMOV_CONDITION(c, "Carry (CF=1)", X_CF, 0);
    TEST_CMOV_CONDITION(e, "Equal (ZF=1)", X_ZF, 0);
    TEST_CMOV_CONDITION(g, "Greater (ZF=0 and SF=OF)", 0, X_ZF|X_SF);
    TEST_CMOV_CONDITION(ge, "Greater or Equal (SF=OF)", 0, X_SF);
    TEST_CMOV_CONDITION(l, "Less (SF≠OF)", X_SF, 0);
    TEST_CMOV_CONDITION(le, "Less or Equal (ZF=1 or SF≠OF)", X_ZF, 0);
    TEST_CMOV_CONDITION(na, "Not Above (CF=1 or ZF=1)", X_CF|X_ZF, 0);
    TEST_CMOV_CONDITION(nae, "Not Above or Equal (CF=1)", X_CF, 0);
    TEST_CMOV_CONDITION(nb, "Not Below (CF=0)", 0, X_CF);
    TEST_CMOV_CONDITION(nbe, "Not Below or Equal (CF=0 and ZF=0)", 0, X_CF|X_ZF);
    TEST_CMOV_CONDITION(nc, "Not Carry (CF=0)", 0, X_CF);
    TEST_CMOV_CONDITION(ne, "Not Equal (ZF=0)", 0, X_ZF);
    TEST_CMOV_CONDITION(ng, "Not Greater (ZF=1 or SF≠OF)", X_ZF, 0);
    TEST_CMOV_CONDITION(nge, "Not Greater or Equal (SF≠OF)", X_SF, 0);
    TEST_CMOV_CONDITION(nl, "Not Less (SF=OF)", 0, X_SF);
    TEST_CMOV_CONDITION(nle, "Not Less or Equal (ZF=0 and SF=OF)", 0, X_ZF|X_SF);
    TEST_CMOV_CONDITION(no, "Not Overflow (OF=0)", 0, X_OF);
    TEST_CMOV_CONDITION(np, "Not Parity (PF=0)", 0, X_PF);
    TEST_CMOV_CONDITION(ns, "Not Sign (SF=0)", 0, X_SF);
    TEST_CMOV_CONDITION(nz, "Not Zero (ZF=0)", 0, X_ZF);
    TEST_CMOV_CONDITION(o, "Overflow (OF=1)", X_OF, 0);
    TEST_CMOV_CONDITION(p, "Parity (PF=1)", X_PF, 0);
    TEST_CMOV_CONDITION(s, "Sign (SF=1)", X_SF, 0);
    TEST_CMOV_CONDITION(z, "Zero (ZF=1)", X_ZF, 0);
}

int main() {
    test_cmov_conditions();
    printf("\nCMOVcc tests completed.\n");
    return 0;
}
