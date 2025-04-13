#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>  // 添加exit函数声明

// 测试配置
#define PADDING_SIZE 64
#define PATTERN_8  0xCC
#define PATTERN_16 0xCDCD
#define PATTERN_32 0xDEADBEEF
#define PATTERN_64 0xDEADBEEFCAFEBABEULL

// 多线程测试配置
#define NUM_THREADS  15    
#define NUM_ITERATIONS 100000

// 内存对齐测试偏移量
const size_t test_offsets[] = {0, 2, 4, 6, 8, 10, 12, 14}; // 仅测试对齐偏移量
#define NUM_OFFSETS (sizeof(test_offsets)/sizeof(test_offsets[0]))

#ifdef __x86_64__
#define PUSHF "pushfq"
#define POPF "pop %q1"
#else
#define PUSHF "pushfl"
#define POPF "pop %1"
#endif

// OR指令内联汇编宏
#define LOCK_OR_SUFFIX(suffix, type, ptr, val, flags) ({ \
    type __val = (val); \
    __asm__ volatile ( \
        "lock or" #suffix " %2, %0\n\t" \
        PUSHF "\n\t" \
        POPF \
        : "+m" (*ptr), "=r" (flags), "+r" (__val) \
        : \
        : "cc", "memory" \
    ); \
    __val; \
})

#define LOCK_OR8(ptr, val, flags)  LOCK_OR_SUFFIX(b, uint8_t, ptr, val, flags)
#define LOCK_OR16(ptr, val, flags) LOCK_OR_SUFFIX(w, uint16_t, ptr, val, flags)
#define LOCK_OR32(ptr, val, flags) LOCK_OR_SUFFIX(l, uint32_t, ptr, val, flags)
#define LOCK_OR64(ptr, val, flags) LOCK_OR_SUFFIX(q, uint64_t, ptr, val, flags)

// 标志位解析
#define FLAG_CF (1 << 0)
#define FLAG_ZF (1 << 6)

static inline bool get_cf(unsigned long flags) { return (flags & FLAG_CF) != 0; }
static inline bool get_zf(unsigned long flags) { return (flags & FLAG_ZF) != 0; }

// 内存初始化（复用xadd代码）
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

// 测试模板宏（修改为OR逻辑）
#define TEST_OR(suffix, type, init, val) do { \
    uint8_t buffer[PADDING_SIZE * 2]; \
    uint8_t expected[PADDING_SIZE * 2]; \
    fill_pattern(buffer, sizeof(buffer)); \
    for (size_t offset = 0; offset < PADDING_SIZE - sizeof(type); offset++) { \
        memcpy(expected, buffer, sizeof(buffer)); \
        type *ptr = (type*)(buffer + offset); \
        type *exp_ptr = (type*)(expected + offset); \
        *ptr = (type)(init); \
        type operand = val; \
        *exp_ptr = *ptr | operand; \
        \
        unsigned long flags = 0; \
        type ret = LOCK_OR##suffix(ptr, operand, flags); \
        \
        bool expect_cf = false; /* OR指令总是清除CF */ \
        bool expect_zf = (*exp_ptr == 0); \
        \
        bool mem_ok = verify_memory(buffer, expected, sizeof(buffer), offset, sizeof(type)); \
        bool flag_ok = (get_cf(flags) == expect_cf) && (get_zf(flags) == expect_zf); \
        \
        printf("%2zu-bit[%2zu] OLD:%0*jx NEW:%0*jx CF:%d/%d ZF:%d/%d %s%s\n", \
               sizeof(type)*8, offset, \
               (int)(sizeof(type)*2), (uintmax_t)ret, \
               (int)(sizeof(type)*2), (uintmax_t)*ptr, \
               get_cf(flags), expect_cf, \
               get_zf(flags), expect_zf, \
               mem_ok ? "\e[32m[MEM]\e[0m" : "\e[31m[MEM_FAILED]\e[0m", \
               flag_ok ? "\e[32m[FLAGS]\e[0m" : "\e[31m[FLAGS_FAILED]\e[0m"); \
    } \
} while(0)

// 多线程测试（修改OR操作）
typedef struct {
    uint8_t buffer[PADDING_SIZE] __attribute__((aligned(64)));
    size_t offset;
    int width;
    volatile uint64_t expected;
    pthread_barrier_t barrier;
} SharedData;

SharedData shared;

#define DEFINE_THREAD_FUNC_OR(width) \
void *thread_func_or_##width(void *arg) { \
    (void)arg; \
    uint##width##_t *ptr = (uint##width##_t*)(shared.buffer + shared.offset); \
    pthread_barrier_wait(&shared.barrier); \
    for (int i = 0; i < NUM_ITERATIONS; ++i) { \
        uint##width##_t val = (i % 2) ? (uint##width##_t)(0xF0F0F0F0F0F0F0F0ULL >> (64 - width)) : (uint##width##_t)(0x0F0F0F0F0F0F0F0FULL >> (64 - width)); \
        unsigned long flags; \
        LOCK_OR##width(ptr, val, flags); \
    } \
    return NULL; \
}

DEFINE_THREAD_FUNC_OR(8)
DEFINE_THREAD_FUNC_OR(16)
DEFINE_THREAD_FUNC_OR(32)
DEFINE_THREAD_FUNC_OR(64)

void run_mt_test_or(int width) {
    pthread_t threads[NUM_THREADS];
    const size_t type_size = width/8;

    for (size_t i = 0; i < NUM_OFFSETS; i++) {
        size_t offset = test_offsets[i];
        if (offset + type_size > PADDING_SIZE) continue;

        // 初始化共享内存
        memset(shared.buffer, 0xCC, PADDING_SIZE);
        shared.offset = offset;
        shared.width = width;

        // 根据宽度初始化指针
        switch(width) {
            case 8:  *(volatile uint8_t*)(shared.buffer+offset) = 0; break;
            case 16: *(volatile uint16_t*)(shared.buffer+offset) = 0; break;
            case 32: *(volatile uint32_t*)(shared.buffer+offset) = 0; break;
            case 64: *(volatile uint64_t*)(shared.buffer+offset) = 0; break;
        }

        shared.expected = (width == 64) ? 0xFFFFFFFFFFFFFFFFULL : (1ULL << width) - 1;

        // 初始化屏障
        pthread_barrier_init(&shared.barrier, NULL, NUM_THREADS + 1);

        // 创建并启动线程
        for (int t = 0; t < NUM_THREADS; t++) {
            if (pthread_create(&threads[t], NULL, 
                width == 8 ? thread_func_or_8 :
                width == 16 ? thread_func_or_16 :
                width == 32 ? thread_func_or_32 : thread_func_or_64, NULL)) {
                perror("pthread_create failed");
                exit(1);
            }
        }

        // 等待所有线程就绪
        pthread_barrier_wait(&shared.barrier);

        // 等待线程完成
        for (int t = 0; t < NUM_THREADS; t++) {
            pthread_join(threads[t], NULL);
        }

        // 销毁屏障
        pthread_barrier_destroy(&shared.barrier);

        // 验证结果
        uint64_t actual;
        // 使用volatile读取确保获取最新值
        switch(width) {
            case 8:  actual = *(volatile uint8_t*)(shared.buffer+offset); break;
            case 16: actual = *(volatile uint16_t*)(shared.buffer+offset); break;
            case 32: actual = *(volatile uint32_t*)(shared.buffer+offset); break;
            case 64: actual = *(volatile uint64_t*)(shared.buffer+offset); break;
        }

        bool mem_ok = true;
        for (size_t j = 0; j < PADDING_SIZE; j++) {
            if (j >= offset && j < offset + type_size) continue;
            if (shared.buffer[j] != 0xCC) {
                printf("  Memory corruption @%zu: 0x%02X\n", j, shared.buffer[j]);
                mem_ok = false;
            }
        }

        printf("MT OR %2d-bit[%2zu] ACT:%10lu EXP:%10lu %s\n",
               width, offset, actual, shared.expected,
               (actual == shared.expected && mem_ok) ?
               "\e[32mPASS\e[0m" : "\e[31mFAIL\e[0m");
    }
}

int main() {
    struct sigaction sa = { .sa_handler = SIG_IGN };
    sigaction(SIGBUS, &sa, NULL);

    printf("========= 8-bit Tests =========\n");
    TEST_OR(8, uint8_t, 0x00, 0x00);  // 全0
    TEST_OR(8, uint8_t, 0xFF, 0x00);  // 全1
    TEST_OR(8, uint8_t, 0xAA, 0x55);  // 模式交替

    printf("\n========= 16-bit Tests =========\n");
    TEST_OR(16, uint16_t, 0x0000, 0x0000);
    TEST_OR(16, uint16_t, 0xFFFF, 0x1234);
    TEST_OR(16, uint16_t, 0xAAAA, 0x5555);

    printf("\n========= 32-bit Tests =========\n");
    TEST_OR(32, uint32_t, 0x00000000, 0x00000000);
    TEST_OR(32, uint32_t, 0xFFFFFFFF, 0x12345678);
    TEST_OR(32, uint32_t, 0xAAAAAAAA, 0x55555555);

    printf("\n========= 64-bit Tests =========\n");
    TEST_OR(64, uint64_t, 0x0000000000000000ULL, 0x0000000000000000ULL);
    TEST_OR(64, uint64_t, 0xFFFFFFFFFFFFFFFFULL, 0x123456789ABCDEF0ULL);
    TEST_OR(64, uint64_t, 0xAAAAAAAAAAAAAAAAULL, 0x5555555555555555ULL);

    // 多线程测试
    printf("========= 8-bit Multi-thread Test =========\n");
    run_mt_test_or(8);
    
    printf("\n========= 16-bit Multi-thread Test =========\n");
    run_mt_test_or(16);
    
    printf("\n========= 32-bit Multi-thread Test =========\n");
    run_mt_test_or(32);
    
    printf("\n========= 64-bit Multi-thread Test =========\n");
    run_mt_test_or(64);

    return 0;
}
