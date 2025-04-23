#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define TEST_TZCNT16(a, expected) \
    do { \
        uint16_t res; \
        uint64_t eflags_before, eflags_after; \
        __asm__ volatile ( \
            "pushfq\n\t" \
            "pop %q0\n\t" \
            "andq $0xfffffffffffffdff, %q0\n\t" \
            "tzcntw %w2, %w1\n\t" \
            "pushfq\n\t" \
            "pop %q3" \
            : "=r"(eflags_before), "=r"(res), "=r"(eflags_after) \
            : "r"(a)); \
        printf("tzcntw 0x%04x => 0x%04x (expected 0x%04x)\n" \
               "  Flags: CF=%d ZF=%d %s\n", \
               a, res, expected, \
               (int)((eflags_after >> 0) & 1), (int)((eflags_after >> 6) & 1), \
               res == expected ? "PASS" : "FAIL"); \
    } while(0)

#define TEST_TZCNT32(a, expected) \
    do { \
        uint32_t res; \
        uint64_t eflags_before, eflags_after; \
        __asm__ volatile ( \
            "pushfq\n\t" \
            "pop %q0\n\t" \
            "andq $0xfffffffffffffdff, %q0\n\t" \
            "tzcntl %k2, %k1\n\t" \
            "pushfq\n\t" \
            "pop %q3" \
            : "=r"(eflags_before), "=r"(res), "=r"(eflags_after) \
            : "r"(a)); \
        printf("tzcntl 0x%08x => 0x%08x (expected 0x%08x)\n" \
               "  Flags: CF=%d ZF=%d %s\n", \
               a, res, expected, \
               (int)((eflags_after >> 0) & 1), (int)((eflags_after >> 6) & 1), \
               res == expected ? "PASS" : "FAIL"); \
    } while(0)

#define TEST_TZCNT64(a, expected) \
    do { \
        uint64_t res; \
        uint64_t eflags_before, eflags_after; \
        __asm__ volatile ( \
            "pushfq\n\t" \
            "pop %q0\n\t" \
            "andq $0xfffffffffffffdff, %q0\n\t" \
            "tzcntq %q2, %q1\n\t" \
            "pushfq\n\t" \
            "pop %q3" \
            : "=r"(eflags_before), "=r"(res), "=r"(eflags_after) \
            : "r"(a)); \
        printf("tzcntq 0x%016" PRIx64 " => 0x%016" PRIx64 " (expected 0x%016" PRIx64 ")\n" \
               "  Flags: CF=%d ZF=%d %s\n", \
               (uint64_t)a, (uint64_t)res, (uint64_t)expected, \
               (int)((eflags_after >> 0) & 1), (int)((eflags_after >> 6) & 1), \
               res == expected ? "PASS" : "FAIL"); \
    } while(0)

int main() {
    printf("=== Testing TZCNT instruction ===\n");

    // Test 16-bit variants
    printf("\n16-bit tests:\n");
    TEST_TZCNT16(0x0000, 0);  // Should set CF=1, ZF=1
    TEST_TZCNT16(0x0001, 0);   // Should clear CF, ZF=0
    TEST_TZCNT16(0x0002, 1);   // Should clear CF, ZF=0
    TEST_TZCNT16(0x8000, 15);  // Should clear CF, ZF=0
    TEST_TZCNT16(0xAAAA, 1);   // Should clear CF, ZF=0

    // Test 32-bit variants
    printf("\n32-bit tests:\n");
    TEST_TZCNT32(0x00000000, 0);  // Should set CF=1, ZF=1
    TEST_TZCNT32(0x00000001, 0);   // Should clear CF, ZF=0
    TEST_TZCNT32(0x00000002, 1);   // Should clear CF, ZF=0
    TEST_TZCNT32(0x80000000, 31);  // Should clear CF, ZF=0
    TEST_TZCNT32(0xAAAAAAAA, 1);   // Should clear CF, ZF=0

    // Test 64-bit variants
    printf("\n64-bit tests:\n");
    TEST_TZCNT64(0x0000000000000000ULL, 0);  // Should set CF=1, ZF=1
    TEST_TZCNT64(0x0000000000000001ULL, 0);   // Should clear CF, ZF=0
    TEST_TZCNT64(0x0000000000000002ULL, 1);   // Should clear CF, ZF=0
    TEST_TZCNT64(0x8000000000000000ULL, 63);  // Should clear CF, ZF=0
    TEST_TZCNT64(0xAAAAAAAAAAAAAAAAULL, 1);   // Should clear CF, ZF=0

    // Test memory operands
    uint16_t mem16 = 0x0008;
    uint32_t mem32 = 0x00000010;
    uint64_t mem64 = 0x0000000000000020ULL;

    printf("\nMemory operand tests:\n");
    uint16_t res16;
    __asm__ volatile ("tzcntw %1, %0" : "=r"(res16) : "m"(mem16));
    printf("tzcntw [mem16] 0x%04x  => 0x%04x (expected 0x0003) %s\n", mem16,
           res16, res16 == 3 ? "PASS" : "FAIL");

    uint32_t res32;
    __asm__ volatile ("tzcntl %1, %0" : "=r"(res32) : "m"(mem32));
    printf("tzcntl [mem32] 0x%08x => 0x%08x (expected 0x00000004) %s\n",mem32,
           res32, res32 == 4 ? "PASS" : "FAIL");

    uint64_t res64;
    __asm__ volatile ("tzcntq %q1, %q0" : "=r"(res64) : "m"(mem64));
    printf("tzcntq [mem64] 0x%016x => 0x%016" PRIx64 " (expected 0x0000000000000005) %s\n", mem64,
           res64, res64 == 5 ? "PASS" : "FAIL");

    return 0;
}
