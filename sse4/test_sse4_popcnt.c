#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#define TEST_POPCNT16(a, b) \
    do { \
        uint16_t res=0xffff; \
        uint64_t eflags_before, eflags_after; \
        __asm__ volatile ( \
            "pushfq\n\t" \
            "pop %q0\n\t" \
            "andq $0xfffffffffffffdff, %q0\n\t" \
            "popcntw %w2, %w1\n\t" \
            "pushfq\n\t" \
            "pop %q3" \
            : "=r"(eflags_before), "=r"(res), "=r"(eflags_after) \
            : "r"(a)); \
        printf("popcntw 0x%04x => 0x%04x (expected 0x%04x)\n" \
               "  EFLAGS after:0x%016" PRIx64 "\n" \
               "  Flags: CF=%ld PF=%ld AF=%ld ZF=%ld SF=%ld OF=%ld %s\n", \
               a, res, b, \
               eflags_after, \
               (eflags_after >> 0) & 1, (eflags_after >> 2) & 1, \
               (eflags_after >> 4) & 1, (eflags_after >> 6) & 1, \
               (eflags_after >> 7) & 1, (eflags_after >> 11) & 1, \
               res == b ? "PASS" : "FAIL"); \
    } while(0)

#define TEST_POPCNT32(a, b) \
    do { \
        uint32_t res=0xffffffff; \
        uint64_t eflags_before, eflags_after; \
        __asm__ volatile ( \
            "pushfq\n\t" \
            "pop %q0\n\t" \
            "andq $0xfffffffffffffdff, %q0\n\t" \
            "popcntl %k2, %k1\n\t" \
            "pushfq\n\t" \
            "pop %q3" \
            : "=r"(eflags_before), "=r"(res), "=r"(eflags_after) \
            : "r"(a)); \
        printf("popcntl 0x%08x => 0x%08x (expected 0x%08x)\n" \
               "  EFLAGS after:0x%016" PRIx64 "\n" \
               "  Flags: CF=%ld PF=%ld AF=%ld ZF=%ld SF=%ld OF=%ld %s\n", \
               a, res, b, \
               eflags_after, \
               (eflags_after >> 0) & 1, (eflags_after >> 2) & 1, \
               (eflags_after >> 4) & 1, (eflags_after >> 6) & 1, \
               (eflags_after >> 7) & 1, (eflags_after >> 11) & 1, \
               res == b ? "PASS" : "FAIL"); \
    } while(0)

#define TEST_POPCNT64(a, b) \
    do { \
        uint64_t res=0xffffffffffffffffULL; \
        uint64_t eflags_before, eflags_after; \
        __asm__ volatile ( \
            "pushfq\n\t" \
            "pop %q0\n\t" \
            "andq $0xfffffffffffffdff, %q0\n\t" \
            "popcntq %q2, %q1\n\t" \
            "pushfq\n\t" \
            "pop %q3" \
            : "=r"(eflags_before), "=r"(res), "=r"(eflags_after) \
            : "r"(a)); \
        printf("popcntq 0x%016" PRIx64 " => 0x%016" PRIx64 " (expected 0x%016" PRIx64 ")\n" \
               "  EFLAGS after:0x%016" PRIx64 "\n" \
               "  Flags: CF=%ld PF=%ld AF=%ld ZF=%ld SF=%ld OF=%ld %s\n", \
               (uint64_t)a, (uint64_t)res, (uint64_t)b, \
               (uint64_t)eflags_after, \
               (eflags_after >> 0) & 1, (eflags_after >> 2) & 1, \
               (eflags_after >> 4) & 1, (eflags_after >> 6) & 1, \
               (eflags_after >> 7) & 1, (eflags_after >> 11) & 1, \
               res == b ? "PASS" : "FAIL"); \
    } while(0)

int main() {
    printf("=== Testing POPCNT instruction ===\n");

    // Test 16-bit variants
    printf("\n16-bit tests:\n");
    TEST_POPCNT16(0x0000, 0x0000);  // Should set ZF
    TEST_POPCNT16(0xFFFF, 0x0010);  // Should clear ZF
    TEST_POPCNT16(0x5555, 0x0008);  // Should clear ZF
    TEST_POPCNT16(0xAAAA, 0x0008);  // Should clear ZF
    TEST_POPCNT16(0x1234, 0x0005);  // Should clear ZF

    // Test 32-bit variants
    printf("\n32-bit tests:\n");
    TEST_POPCNT32(0x00000000, 0x00000000);  // Should set ZF
    TEST_POPCNT32(0xFFFFFFFF, 0x00000020);  // Should clear ZF
    TEST_POPCNT32(0x55555555, 0x00000010);  // Should clear ZF
    TEST_POPCNT32(0xAAAAAAAA, 0x00000010);  // Should clear ZF
    TEST_POPCNT32(0x12345678, 0x0000000D);  // Should clear ZF

    // Test 64-bit variants
    printf("\n64-bit tests:\n");
    TEST_POPCNT64(0x0000000000000000ULL, 0x0000000000000000ULL);  // Should set ZF
    TEST_POPCNT64(0xFFFFFFFFFFFFFFFFULL, 0x0000000000000040ULL);  // Should clear ZF
    TEST_POPCNT64(0x5555555555555555ULL, 0x0000000000000020ULL);  // Should clear ZF
    TEST_POPCNT64(0xAAAAAAAAAAAAAAAAULL, 0x0000000000000020ULL);  // Should clear ZF
    TEST_POPCNT64(0x0123456789ABCDEFULL, 0x0000000000000020ULL);  // Should clear ZF

    // Test memory operands
    uint16_t mem16 = 0x1234;
    uint32_t mem32 = 0x12345678;
    uint64_t mem64 = 0x0123456789ABCDEF;

    printf("\nMemory operand tests:\n");
    uint16_t res16;
    __asm__ volatile ("popcntw %1, %0" : "=r"(res16) : "m"(mem16));
    printf("popcntw [mem16] => 0x%04x (expected 0x0005) %s\n",
           res16, res16 == 5 ? "PASS" : "FAIL");

    uint32_t res32;
    __asm__ volatile ("popcntl %1, %0" : "=r"(res32) : "m"(mem32));
    printf("popcntl [mem32] => 0x%08x (expected 0x0000000D) %s\n",
           res32, res32 == 0xD ? "PASS" : "FAIL");

    uint64_t res64;
    __asm__ volatile ("popcntq %q1, %q0" : "=r"(res64) : "m"(mem64));
    printf("popcntq [mem64] => 0x%016" PRIx64 " (expected 0x0000000000000020) %s\n",
           res64, res64 == 0x20 ? "PASS" : "FAIL");

    return 0;
}
