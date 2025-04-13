#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>

// 测试配置
#define PADDING_SIZE 64
#define PATTERN_8  0xCC
#define PATTERN_16 0xCDCD
#define PATTERN_32 0xDEADBEEF
#define PATTERN_64 0xDEADBEEFCAFEBABEULL

#ifdef __x86_64__
#define PUSHF "pushfq"
#define POPF "pop %q1"  // 使用64位寄存器
#else
#define PUSHF "pushfl"
#define POPF "pop %1"   // 使用32位寄存器
#endif

// 内联汇编宏
#define LOCK_SUFFIX(suffix, type, ptr, flags) ({ \
    __asm__ volatile ( \
        "lock " #suffix " %0\n\t" \
        PUSHF "\n\t" \
        POPF \
        : "+m" (*ptr), "=r" (flags) \
        : \
        : "cc", "memory" \
    ); \
})

// 各宽度专用宏
#define LOCK_NOT8(ptr, flags)  LOCK_SUFFIX(notb, uint8_t, ptr, flags)
#define LOCK_NOT16(ptr, flags) LOCK_SUFFIX(notw, uint16_t, ptr, flags)
#define LOCK_NOT32(ptr, flags) LOCK_SUFFIX(notl, uint32_t, ptr, flags)
#define LOCK_NOT64(ptr, flags) LOCK_SUFFIX(notq, uint64_t, ptr, flags)

// 内存初始化
void fill_pattern(void *buf, size_t size) {
    uint8_t *p = buf;
    for (size_t i = 0; i < size; i++) {
        switch (i % 4) {
            case 0: p[i] = 0xCC; break;
            case 1: p[i] = 0xCD; break;
            case 2: p[i] = 0xDE; break;
            case 3: p[i] = 0xAD; break;
        }
    }
}

// 内存验证
bool verify_memory(const void *actual, const void *expected,
                  size_t size, size_t offset, int width) {
    const uint8_t *a = actual, *e = expected;
    for (size_t i = 0; i < size; i++) {
        if (i >= (size_t)offset && i < (size_t)(offset + width)) continue;
        if (a[i] != e[i]) {
            fprintf(stderr, "  Memory corruption @%zu: 0x%02X vs 0x%02X\n",
                    i, a[i], e[i]);
            return false;
        }
    }
    return true;
}

// 测试模板宏
#define TEST_NOT(suffix, type, init) do { \
    uint8_t buffer[PADDING_SIZE * 2]; \
    uint8_t expected[PADDING_SIZE * 2]; \
    fill_pattern(buffer, sizeof(buffer)); \
    for (size_t offset = 0; offset < PADDING_SIZE - sizeof(type); offset++) { \
        memcpy(expected, buffer, sizeof(buffer)); \
        type *ptr = (type*)(buffer + offset); \
        type *exp_ptr = (type*)(expected + offset); \
        *ptr = (type)(init); \
        *exp_ptr = ~(*ptr); \
        \
        unsigned long flags = 0; \
        LOCK_NOT##suffix(ptr, flags); \
        \
        bool mem_ok = verify_memory(buffer, expected, sizeof(buffer), offset, sizeof(type)); \
        \
        printf("%2zu-bit[%2zu] RES:%0*jx EXP:%0*jx %s\n", \
               sizeof(type)*8, offset, \
               (int)(sizeof(type)*2), (uintmax_t)*ptr, \
               (int)(sizeof(type)*2), (uintmax_t)*exp_ptr, \
               mem_ok ? "\e[32m[MEM]\e[0m" : "\e[31m[MEM_FAILED]\e[0m"); \
    } \
} while(0)

// 多线程测试相关
#define NUM_THREADS  15
#define NUM_ITERATIONS 100000
const size_t test_offsets[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
#define NUM_OFFSETS (sizeof(test_offsets)/sizeof(test_offsets[0]))

typedef struct {
    uint8_t buffer[PADDING_SIZE] __attribute__((aligned(64)));
    size_t offset;
    int width;
    pthread_barrier_t barrier;
} SharedData;

SharedData shared;

#define DEFINE_THREAD_FUNC(width) \
void *thread_func_##width(void *arg) { \
    (void)arg; \
    uint##width##_t *ptr = (uint##width##_t*)(shared.buffer + shared.offset); \
    pthread_barrier_wait(&shared.barrier); \
    for (int i = 0; i < NUM_ITERATIONS; ++i) { \
        unsigned long flags; \
        LOCK_NOT##width(ptr, flags); \
    } \
    return NULL; \
}

DEFINE_THREAD_FUNC(8)
DEFINE_THREAD_FUNC(16)
DEFINE_THREAD_FUNC(32)
DEFINE_THREAD_FUNC(64)

void run_mt_test(int width) {
    pthread_t threads[NUM_THREADS];
    const size_t type_size = width/8;

    for (size_t i = 0; i < NUM_OFFSETS; i++) {
        size_t offset = test_offsets[i];
        if (offset + type_size > PADDING_SIZE) continue;

        memset(shared.buffer, 0x55, PADDING_SIZE);
        shared.offset = offset;
        shared.width = width;

        pthread_barrier_init(&shared.barrier, NULL, NUM_THREADS + 1);

        for (int t = 0; t < NUM_THREADS; t++) {
            switch(width) {
                case 8:  pthread_create(&threads[t], NULL, thread_func_8, NULL); break;
                case 16: pthread_create(&threads[t], NULL, thread_func_16, NULL); break;
                case 32: pthread_create(&threads[t], NULL, thread_func_32, NULL); break;
                case 64: pthread_create(&threads[t], NULL, thread_func_64, NULL); break;
            }
        }

        pthread_barrier_wait(&shared.barrier);

        for (int t = 0; t < NUM_THREADS; t++) {
            pthread_join(threads[t], NULL);
        }

        pthread_barrier_destroy(&shared.barrier);

        uint64_t actual;
        switch(width) {
            case 8:  actual = *(uint8_t*)(shared.buffer+offset); break;
            case 16: actual = *(uint16_t*)(shared.buffer+offset); break;
            case 32: actual = *(uint32_t*)(shared.buffer+offset); break;
            case 64: actual = *(uint64_t*)(shared.buffer+offset); break;
        }

        bool mem_ok = true;
        for (size_t j = 0; j < PADDING_SIZE; j++) {
            if (j >= offset && j < offset + type_size) continue;
            if (shared.buffer[j] != 0x55) {
                printf("  Memory corruption @%zu: 0x%02X\n", j, shared.buffer[j]);
                mem_ok = false;
            }
        }

        uint64_t expected;
        if (width == 8) {
            expected = ~0x55ULL & 0xFF;
        } else if (width == 16) {
            expected = ~0x5555ULL & 0xFFFF;
        } else if (width == 32) {
            expected = ~0x55555555ULL & 0xFFFFFFFF;
        } else {
            expected = ~0x5555555555555555ULL;
        }
        // For multi-thread test, the final value should be the initial value
        // since each thread flips all bits multiple times (odd number of times)
        if (NUM_THREADS % 2 == 1) {
            if (width == 8) {
                expected = 0x55;
            } else if (width == 16) {
                expected = 0x5555;
            } else if (width == 32) {
                expected = 0x55555555;
            } else {
                expected = 0x5555555555555555ULL;
            }
        }
        printf("MT %2d-bit[%2zu] ACT:%10lx EXP:0x%016lx %s\n",
               width, offset, actual, expected,
               (actual == expected && mem_ok) ?
               "\e[32mPASS\e[0m" : "\e[31mFAIL\e[0m");
    }
}

int main() {
    struct sigaction sa = { .sa_handler = SIG_IGN };
    sigaction(SIGBUS, &sa, NULL);

    printf("========= 8-bit Tests =========\n");
    TEST_NOT(8, uint8_t, 0x00);  // 全0
    TEST_NOT(8, uint8_t, 0xFF);  // 全1
    TEST_NOT(8, uint8_t, 0x55);  // 交替模式
    TEST_NOT(8, uint8_t, 0xAA);  // 交替模式

    printf("\n========= 16-bit Tests =========\n");
    TEST_NOT(16, uint16_t, 0x0000);  // 全0
    TEST_NOT(16, uint16_t, 0xFFFF);  // 全1
    TEST_NOT(16, uint16_t, 0x5555);  // 交替模式
    TEST_NOT(16, uint16_t, 0xAAAA);  // 交替模式

    printf("\n========= 32-bit Tests =========\n");
    TEST_NOT(32, uint32_t, 0x00000000);  // 全0
    TEST_NOT(32, uint32_t, 0xFFFFFFFF);  // 全1
    TEST_NOT(32, uint32_t, 0x55555555);  // 交替模式
    TEST_NOT(32, uint32_t, 0xAAAAAAAA);  // 交替模式

    printf("\n========= 64-bit Tests =========\n");
    TEST_NOT(64, uint64_t, 0x0000000000000000ULL);  // 全0
    TEST_NOT(64, uint64_t, 0xFFFFFFFFFFFFFFFFULL);  // 全1
    TEST_NOT(64, uint64_t, 0x5555555555555555ULL);  // 交替模式
    TEST_NOT(64, uint64_t, 0xAAAAAAAAAAAAAAAAULL);  // 交替模式

    printf("========= 8-bit Multi-thread Test =========\n");
    run_mt_test(8);
    
    printf("\n========= 16-bit Multi-thread Test =========\n");
    run_mt_test(16);
    
    printf("\n========= 32-bit Multi-thread Test =========\n");
    run_mt_test(32);
    
    printf("\n========= 64-bit Multi-thread Test =========\n");
    run_mt_test(64);

    return 0;
}
