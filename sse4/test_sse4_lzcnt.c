#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define TEST_LZCNT16(a, expected) \
    do { \
        uint16_t res; \
        uint64_t eflags_before, eflags_after; \
        __asm__ volatile ( \
            "pushfq\n\t" \
            "pop %q0\n\t" \
            "xor %%cx, %%cx\n\t" \
            "bsrw %w2, %w1\n\t" \
            "jz 1f\n\t" \
            "mov $15, %%ax\n\t" \
            "sub %w1, %%ax\n\t" \
            "mov %%ax, %w1\n\t" \
            "jmp 2f\n\t" \
            "1:\n\t" \
            "mov $16, %w1\n\t" \
            "2:\n\t" \
            "pushfq\n\t" \
            "pop %q3" \
            : "=r"(eflags_before), "=r"(res), "=r"(eflags_after) \
            : "r"(a)); \
        printf("lzcntw 0x%04x => 0x%04x (expected 0x%04x)\n" \
               "  EFLAGS after:0x%016" PRIx64 "\n" \
               "  Flags: CF=%d ZF=%d %s\n", \
               a, res, expected, \
               eflags_after, \
               (int)((eflags_after >> 0) & 1), (int)((eflags_after >> 6) & 1), \
               res == expected ? "PASS" : "FAIL"); \
    } while(0)

#define TEST_LZCNT32(a, expected) \
    do { \
        uint32_t res; \
        uint64_t eflags_before, eflags_after; \
        __asm__ volatile ( \
            "pushfq\n\t" \
            "pop %q0\n\t" \
            "xor %%ecx, %%ecx\n\t" \
            "bsrl %k2, %k1\n\t" \
            "jz 1f\n\t" \
            "mov $31, %%eax\n\t" \
            "sub %k1, %%eax\n\t" \
            "mov %%eax, %k1\n\t" \
            "jmp 2f\n\t" \
            "1:\n\t" \
            "mov $32, %k1\n\t" \
            "2:\n\t" \
            "pushfq\n\t" \
            "pop %q3" \
            : "=r"(eflags_before), "=r"(res), "=r"(eflags_after) \
            : "r"(a)); \
        printf("lzcntl 0x%08x => 0x%08x (expected 0x%08x)\n" \
               "  EFLAGS after:0x%016" PRIx64 "\n" \
               "  Flags: CF=%d ZF=%d %s\n", \
               a, res, expected, \
               eflags_after, \
               (eflags_after >> 0) & 1, (eflags_after >> 6) & 1, \
               res == expected ? "PASS" : "FAIL"); \
    } while(0)

#define TEST_LZCNT64(a, expected) \
    do { \
        uint64_t res; \
        uint64_t eflags_before, eflags_after; \
        __asm__ volatile ( \
            "pushfq\n\t" \
            "pop %q0\n\t" \
            "xor %%rcx, %%rcx\n\t" \
            "bsrq %q2, %q1\n\t" \
            "jz 1f\n\t" \
            "mov $63, %%rax\n\t" \
            "sub %q1, %%rax\n\t" \
            "mov %%rax, %q1\n\t" \
            "jmp 2f\n\t" \
            "1:\n\t" \
            "mov $64, %q1\n\t" \
            "2:\n\t" \
            "pushfq\n\t" \
            "pop %q3" \
            : "=r"(eflags_before), "=r"(res), "=r"(eflags_after) \
            : "r"(a)); \
        printf("lzcntq 0x%016" PRIx64 " => 0x%016" PRIx64 " (expected 0x%016" PRIx64 ")\n" \
               "  EFLAGS after:0x%016" PRIx64 "\n" \
               "  Flags: CF=%d ZF=%d %s\n", \
               (uint64_t)a, (uint64_t)res, (uint64_t)expected, \
               (uint64_t)eflags_after, \
               (eflags_after >> 0) & 1, (eflags_after >> 6) & 1, \
               res == expected ? "PASS" : "FAIL"); \
    } while(0)

int main() {
    printf("=== Testing LZCNT instruction ===\n");

    // Test 16-bit variants
    printf("\n16-bit tests:\n");
    TEST_LZCNT16(0x0000, 16);  // Should set CF=1, ZF=1
    TEST_LZCNT16(0x0001, 15);   // Should clear CF, ZF=0
    TEST_LZCNT16(0x8000, 0);    // Should clear CF, ZF=0
    TEST_LZCNT16(0x4000, 1);    // Should clear CF, ZF=0
    TEST_LZCNT16(0x000F, 12);   // Should clear CF, ZF=0

    // Test 32-bit variants
    printf("\n32-bit tests:\n");
    TEST_LZCNT32(0x00000000, 32);  // Should set CF=1, ZF=1
    TEST_LZCNT32(0x00000001, 31);  // Should clear CF, ZF=0
    TEST_LZCNT32(0x80000000, 0);   // Should clear CF, ZF=0
    TEST_LZCNT32(0x40000000, 1);   // Should clear CF, ZF=0
    TEST_LZCNT32(0x0000000F, 28);  // Should clear CF, ZF=0

    // Test 64-bit variants
    printf("\n64-bit tests:\n");
    TEST_LZCNT64(0x0000000000000000ULL, 64);  // Should set CF=1, ZF=1
    TEST_LZCNT64(0x0000000000000001ULL, 63);  // Should clear CF, ZF=0
    TEST_LZCNT64(0x8000000000000000ULL, 0);   // Should clear CF, ZF=0
    TEST_LZCNT64(0x4000000000000000ULL, 1);   // Should clear CF, ZF=0
    TEST_LZCNT64(0x000000000000000FULL, 60);  // Should clear CF, ZF=0

    return 0;
}
