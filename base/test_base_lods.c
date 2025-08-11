#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base.h"

static void test_lodsb() {
    uint8_t buffer[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t result;
    uintptr_t rsi_before, rsi_after;

    printf("Testing LODSB (aligned, forward):\n");
    CLEAR_FLAGS;
    asm volatile ("cld");
    rsi_before = (uintptr_t)buffer;
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsb\n\t"
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    printf("  Value: 0x%02X, Expected: 0x%02X - %s\n", result, buffer[0], 
           result == buffer[0] ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before + 1,
//            rsi_after == rsi_before + 1 ? "PASS" : "FAIL");

    printf("Testing LODSB (unaligned, forward):\n");
    CLEAR_FLAGS;
    asm volatile ("cld");
    rsi_before = (uintptr_t)(buffer + 1);
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsb\n\t"
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    printf("  Value: 0x%02X, Expected: 0x%02X - %s\n", result, buffer[1], 
           result == buffer[1] ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before + 1,
//            rsi_after == rsi_before + 1 ? "PASS" : "FAIL");

    printf("Testing LODSB (aligned, backward):\n");
    CLEAR_FLAGS;
    asm volatile ("std");
    rsi_before = (uintptr_t)(buffer + 7);
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsb\n\t"
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    printf("  Value: 0x%02X, Expected: 0x%02X - %s\n", result, buffer[7], 
           result == buffer[7] ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before - 1,
//            rsi_after == rsi_before - 1 ? "PASS" : "FAIL");
    asm volatile ("cld"); // Reset direction flag

    printf("\n");
}

static void test_lodsw() {
    uint16_t buffer[4] = {0x0102, 0x0304, 0x0506, 0x0708};
    uint16_t result;
    uintptr_t rsi_before, rsi_after;

    printf("Testing LODSW (aligned, forward):\n");
    CLEAR_FLAGS;
    asm volatile ("cld");
    rsi_before = (uintptr_t)buffer;
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsw\n\t"
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    printf("  Value: 0x%04X, Expected: 0x%04X - %s\n", result, buffer[0], 
           result == buffer[0] ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before + 2,
//            rsi_after == rsi_before + 2 ? "PASS" : "FAIL");

    printf("Testing LODSW (unaligned, forward):\n");
    CLEAR_FLAGS;
    asm volatile ("cld");
    rsi_before = (uintptr_t)((uint8_t*)buffer + 1);
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsw\n\t"
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    uint16_t expected = (buffer[0] >> 8) | (buffer[1] << 8);
    printf("  Value: 0x%04X, Expected: 0x%04X - %s\n", result, expected, 
           result == expected ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before + 2,
//            rsi_after == rsi_before + 2 ? "PASS" : "FAIL");

    printf("Testing LODSW (aligned, backward):\n");
    CLEAR_FLAGS;
    asm volatile ("std");
    rsi_before = (uintptr_t)(buffer + 3);
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsw\n\t"
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    printf("  Value: 0x%04X, Expected: 0x%04X - %s\n", result, buffer[3], 
           result == buffer[3] ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before - 2,
//            rsi_after == rsi_before - 2 ? "PASS" : "FAIL");
    asm volatile ("cld"); // Reset direction flag

    printf("\n");
}

static void test_lodsd() {
    uint32_t buffer[4] = {0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10};
    uint32_t result;
    uintptr_t rsi_before, rsi_after;

    printf("Testing LODSD (aligned, forward):\n");
    CLEAR_FLAGS;
    asm volatile ("cld");
    rsi_before = (uintptr_t)buffer;
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsl\n\t"
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    printf("  Value: 0x%08X, Expected: 0x%08X - %s\n", result, buffer[0], 
           result == buffer[0] ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before + 4,
//            rsi_after == rsi_before + 4 ? "PASS" : "FAIL");

    printf("Testing LODSD (unaligned, forward):\n");
    CLEAR_FLAGS;
    asm volatile ("cld");
    rsi_before = (uintptr_t)((uint8_t*)buffer + 1);
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsl\n\t"
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    uint32_t expected = (buffer[0] >> 8) | (buffer[1] << 24);
    printf("  Value: 0x%08X, Expected: 0x%08X - %s\n", result, expected, 
           result == expected ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before + 4,
//            rsi_after == rsi_before + 4 ? "PASS" : "FAIL");

    printf("Testing LODSD (aligned, backward):\n");
    CLEAR_FLAGS;
    asm volatile ("std");
    rsi_before = (uintptr_t)(buffer + 3);
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsl\n\t"
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    printf("  Value: 0x%08X, Expected: 0x%08X - %s\n", result, buffer[3], 
           result == buffer[3] ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before - 4,
//            rsi_after == rsi_before - 4 ? "PASS" : "FAIL");
    asm volatile ("cld"); // Reset direction flag

    printf("\n");
}

static void test_lodsq() {
    uint64_t buffer[4] = {
        0x0102030405060708, 
        0x090A0B0C0D0E0F10,
        0x1112131415161718,
        0x191A1B1C1D1E1F20
    };
    uint64_t result;
    uintptr_t rsi_before, rsi_after;

    printf("Testing LODSQ (aligned, forward):\n");
    CLEAR_FLAGS;
    asm volatile ("cld");
    rsi_before = (uintptr_t)buffer;
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsq\n\t"  // This was already correct, but we'll include for clarity
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    printf("  Value: 0x%016lX, Expected: 0x%016lX - %s\n", result, buffer[0], 
           result == buffer[0] ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before + 8,
//            rsi_after == rsi_before + 8 ? "PASS" : "FAIL");

    printf("Testing LODSQ (unaligned, forward):\n");
    CLEAR_FLAGS;
    asm volatile ("cld");
    rsi_before = (uintptr_t)((uint8_t*)buffer + 3);
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsq\n\t"
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    uint64_t expected = (buffer[0] >> 24) | (buffer[1] << 40);
    printf("  Value: 0x%016lX, Expected: 0x%016lX - %s\n", result, expected, 
           result == expected ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before + 8,
//            rsi_after == rsi_before + 8 ? "PASS" : "FAIL");

    printf("Testing LODSQ (aligned, backward):\n");
    CLEAR_FLAGS;
    asm volatile ("std");
    rsi_before = (uintptr_t)(buffer + 3);
    asm volatile (
        "mov %[addr], %%rsi\n\t"
        "lodsq\n\t"
        : "=a" (result), "=S" (rsi_after)
        : [addr] "r" (rsi_before)
        : "memory"
    );
    printf("  Value: 0x%016lX, Expected: 0x%016lX - %s\n", result, buffer[3], 
           result == buffer[3] ? "PASS" : "FAIL");
//     printf("  RSI before: 0x%lx, RSI after: 0x%lx (expected: 0x%lx) - %s\n", 
//            rsi_before, rsi_after, rsi_before - 8,
//            rsi_after == rsi_before - 8 ? "PASS" : "FAIL");
    asm volatile ("cld"); // Reset direction flag

    printf("\n");
}

int main() {
    printf("Starting LODS tests...\n\n");
    
    test_lodsb();
    test_lodsw();
    test_lodsd();
    test_lodsq();
    
    printf("All LODS tests completed.\n");
    return 0;
}
