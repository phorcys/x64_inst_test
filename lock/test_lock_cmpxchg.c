#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#define LOCK_PREFIX "lock "

// 测试8位CMPXCHG
void test_cmpxchg8() {
    printf("Testing 8-bit CMPXCHG:\n");
    uint8_t mem_val, expected, new_val, result;
    uint8_t aligned_mem[16] __attribute__((aligned(16)));
    uint8_t *unaligned_mem = aligned_mem + 1; // 非对齐地址

    // 测试1: 寄存器目标 - 匹配成功
    asm volatile (
        "movb $0x12, %%al\n"
        "movb $0x12, %1\n"
        "movb $0x34, %%cl\n"
        LOCK_PREFIX "cmpxchgb %%cl, %1\n"
        : "=a"(result), "+m"(mem_val)
        : 
        : "cl"
    );
    printf("Reg target [match]: mem=%02X (expected 0x34), result=%02X (expected 0x12) - %s\n",
           mem_val, result, (mem_val == 0x34 && result == 0x12) ? "PASS" : "FAIL");

    // 测试2: 内存目标(对齐) - 匹配失败
    aligned_mem[0] = 0x56;
    expected = 0x55;
    new_val = 0x78;
    asm volatile (
        "movb %2, %%al\n"
        LOCK_PREFIX "cmpxchgb %3, %0\n"
        : "+m"(aligned_mem[0]), "=a"(result)
        : "r"(expected), "r"(new_val)
        : 
    );
    printf("Aligned mem [fail]: mem=%02X (expected 0x56), result=%02X (expected 0x56) - %s\n",
           aligned_mem[0], result, (aligned_mem[0] == 0x56 && result == 0x56) ? "PASS" : "FAIL");

    // 测试3: 内存目标(非对齐) - 匹配成功
    *unaligned_mem = 0xAB;
    expected = 0xAB;
    new_val = 0xCD;
    asm volatile (
        "movb %2, %%al\n"
        LOCK_PREFIX "cmpxchgb %3, %0\n"
        : "+m"(*unaligned_mem), "=a"(result)
        : "r"(expected), "r"(new_val)
        : 
    );
    printf("Unaligned mem [match]: mem=%02X (expected 0xCD), result=%02X (expected 0xAB) - %s\n",
           *unaligned_mem, result, (*unaligned_mem == 0xCD && result == 0xAB) ? "PASS" : "FAIL");
}

// 16位CMPXCHG测试
void test_cmpxchg16() {
    printf("\nTesting 16-bit CMPXCHG:\n");
    uint16_t mem_val, expected, new_val, result;
    uint16_t aligned_mem[8] __attribute__((aligned(16)));
    uint16_t *unaligned_mem = (uint16_t*)((char*)aligned_mem + 1); // 非对齐地址

    // 寄存器目标 - 匹配成功
    asm volatile (
        "movw $0x1234, %%ax\n"
        "movw $0x1234, %1\n"
        "movw $0x5678, %%cx\n"
        LOCK_PREFIX "cmpxchgw %%cx, %1\n"
        : "=a"(result), "+m"(mem_val)
        : 
        : "cx"
    );
    printf("Reg target [match]: mem=%04X (expected 0x5678), result=%04X (expected 0x1234) - %s\n",
           mem_val, result, (mem_val == 0x5678 && result == 0x1234) ? "PASS" : "FAIL");

    // 内存目标(对齐) - 匹配失败
    aligned_mem[0] = 0x9ABC;
    expected = 0xDEAD;
    new_val = 0xBEEF;
    asm volatile (
        "movw %2, %%ax\n"
        LOCK_PREFIX "cmpxchgw %3, %0\n"
        : "+m"(aligned_mem[0]), "=a"(result)
        : "r"(expected), "r"(new_val)
        : 
    );
    printf("Aligned mem [fail]: mem=%04X (expected 0x9ABC), result=%04X (expected 0x9ABC) - %s\n",
           aligned_mem[0], result, (aligned_mem[0] == 0x9ABC && result == 0x9ABC) ? "PASS" : "FAIL");

    // 内存目标(非对齐) - 匹配成功
    *unaligned_mem = 0x1111;
    expected = 0x1111;
    new_val = 0x2222;
    asm volatile (
        "movw %2, %%ax\n"
        LOCK_PREFIX "cmpxchgw %3, %0\n"
        : "+m"(*unaligned_mem), "=a"(result)
        : "r"(expected), "r"(new_val)
        : 
    );
    printf("Unaligned mem [match]: mem=%04X (expected 0x2222), result=%04X (expected 0x1111) - %s\n",
           *unaligned_mem, result, (*unaligned_mem == 0x2222 && result == 0x1111) ? "PASS" : "FAIL");
}

// 32位CMPXCHG测试
void test_cmpxchg32() {
    printf("\nTesting 32-bit CMPXCHG:\n");
    uint32_t mem_val, expected, new_val, result;
    uint32_t aligned_mem[4] __attribute__((aligned(16)));
    uint32_t *unaligned_mem = (uint32_t*)((char*)aligned_mem + 1); // 非对齐地址

    // 寄存器目标 - 匹配成功
    asm volatile (
        "movl $0x12345678, %%eax\n"
        "movl $0x12345678, %1\n"
        "movl $0x9ABCDEF0, %%ecx\n"
        LOCK_PREFIX "cmpxchgl %%ecx, %1\n"
        : "=a"(result), "+m"(mem_val)
        : 
        : "ecx"
    );
    printf("Reg target [match]: mem=%08X (expected 0x9ABCDEF0), result=%08X (expected 0x12345678) - %s\n",
           mem_val, result, (mem_val == 0x9ABCDEF0 && result == 0x12345678) ? "PASS" : "FAIL");

    // 内存目标(对齐) - 匹配失败
    aligned_mem[0] = 0x11223344;
    expected = 0x55667788;
    new_val = 0x99AABBCC;
    asm volatile (
        "movl %2, %%eax\n"
        LOCK_PREFIX "cmpxchgl %3, %0\n"
        : "+m"(aligned_mem[0]), "=a"(result)
        : "r"(expected), "r"(new_val)
        : 
    );
    printf("Aligned mem [fail]: mem=%08X (expected 0x11223344), result=%08X (expected 0x11223344) - %s\n",
           aligned_mem[0], result, (aligned_mem[0] == 0x11223344 && result == 0x11223344) ? "PASS" : "FAIL");

    // 内存目标(非对齐) - 匹配成功
    *unaligned_mem = 0xDEADBEEF;
    expected = 0xDEADBEEF;
    new_val = 0xCAFEBABE;
    asm volatile (
        "movl %2, %%eax\n"
        LOCK_PREFIX "cmpxchgl %3, %0\n"
        : "+m"(*unaligned_mem), "=a"(result)
        : "r"(expected), "r"(new_val)
        : 
    );
    printf("Unaligned mem [match]: mem=%08X (expected 0xCAFEBABE), result=%08X (expected 0xDEADBEEF) - %s\n",
           *unaligned_mem, result, (*unaligned_mem == 0xCAFEBABE && result == 0xDEADBEEF) ? "PASS" : "FAIL");
}

// 64位CMPXCHG测试
void test_cmpxchg64() {
    printf("\nTesting 64-bit CMPXCHG:\n");
    uint64_t mem_val, expected, new_val, result;
    uint64_t aligned_mem[2] __attribute__((aligned(16)));
    uint64_t *unaligned_mem = (uint64_t*)((char*)aligned_mem + 1); // 非对齐地址

    // 寄存器目标 - 匹配成功
    uint64_t expected_val = 0x123456789ABCDEF0;
    uint64_t new_val1 = 0x1122334455667788;
    asm volatile (
        "movq %[expected], %%rax\n"
        "movq %[expected], %[mem]\n"
        "movq %[new_val], %%rcx\n"
        LOCK_PREFIX "cmpxchgq %%rcx, %[mem]\n"
        : [mem] "+m" (mem_val), "=a" (result)
        : [expected] "r" (expected_val), [new_val] "r" (new_val1)
        : "rcx", "memory"
    );
    printf("Reg target [match]: mem=%016lX (expected 0x1122334455667788), result=%016lX (expected 0x123456789ABCDEF0) - %s\n",
           mem_val, result, (mem_val == 0x1122334455667788 && result == 0x123456789ABCDEF0) ? "PASS" : "FAIL");

    // 内存目标(对齐) - 匹配失败
    aligned_mem[0] = 0xDEADBEEFCAFEBABE;
    expected = 0x0123456789ABCDEF;
    new_val = 0x8877665544332211;
    asm volatile (
        "movq %[exp], %%rax\n"
        LOCK_PREFIX "cmpxchgq %[new], %[mem]\n"
        : [mem] "+m" (aligned_mem[0]), "=a" (result)
        : [exp] "r" (expected), [new] "r" (new_val)
        : "memory"
    );
    printf("Aligned mem [fail]: mem=%016lX (expected 0xDEADBEEFCAFEBABE), result=%016lX (expected 0xDEADBEEFCAFEBABE) - %s\n",
           aligned_mem[0], result, (aligned_mem[0] == 0xDEADBEEFCAFEBABE && result == 0xDEADBEEFCAFEBABE) ? "PASS" : "FAIL");

    // 内存目标(非对齐) - 匹配成功
    *unaligned_mem = 0xAABBCCDDEEFF0011;
    expected = 0xAABBCCDDEEFF0011;
    new_val = 0x2233445566778899;
    asm volatile (
        "movq %[exp], %%rax\n"
        LOCK_PREFIX "cmpxchgq %[new], %[mem]\n"
        : [mem] "+m" (*unaligned_mem), "=a" (result)
        : [exp] "r" (expected), [new] "r" (new_val)
        : "memory"
    );
    printf("Unaligned mem [match]: mem=%016lX (expected 0x2233445566778899), result=%016lX (expected 0xAABBCCDDEEFF0011) - %s\n",
           *unaligned_mem, result, (*unaligned_mem == 0x2233445566778899 && result == 0xAABBCCDDEEFF0011) ? "PASS" : "FAIL");
}

// 多线程测试
#define NUM_THREADS 4
volatile uint32_t shared_val = 0;

void *thread_func(void *arg) {
    for (int i = 0; i < 1000; i++) {
        uint32_t expected, new_val, result;
        do {
            expected = shared_val;
            new_val = expected + 1;
            result = expected;
            asm volatile (
                LOCK_PREFIX "cmpxchgl %[new_val], %[shared_val]\n"
                : [shared_val] "+m" (shared_val), "=a" (result)
                : [new_val] "r" (new_val), "a" (expected)
                : 
            );
        } while (result != expected); // 重试直到成功
    }
    return NULL;
}

void test_multithread() {
    printf("\nTesting multithreaded 32-bit CMPXCHG:\n");
    pthread_t threads[NUM_THREADS];
    shared_val = 0;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    uint32_t expected_total = NUM_THREADS * 1000;
    printf("Final value: %u (expected %u) - %s\n", 
           shared_val, expected_total, 
           (shared_val == expected_total) ? "PASS" : "FAIL");
}

int main() {
    test_cmpxchg8();
    test_cmpxchg16();
    test_cmpxchg32();
    test_cmpxchg64();
    test_multithread();
    return 0;
}
