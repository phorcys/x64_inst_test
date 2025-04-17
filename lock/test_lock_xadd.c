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
#define PADDING_SIZE 4
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

// 内联汇编宏（修正版）
#define LOCK_SUFFIX(suffix, type, ptr, add, flags) ({ \
    type __add = (add); \
    __asm__ volatile ( \
	"lock " #suffix " %2, %0\n\t" \
	PUSHF "\n\t" \
	POPF \
        : "+m" (*ptr), "=r" (flags), "+r" (__add) \
        : \
        : "cc", "memory" \
    ); \
    __add; \
})

// 各宽度专用宏
#define LOCK_XADD8(ptr, add, flags)  LOCK_SUFFIX(xaddb, uint8_t, ptr, add, flags)
#define LOCK_XADD16(ptr, add, flags) LOCK_SUFFIX(xaddw, uint16_t, ptr, add, flags)
#define LOCK_XADD32(ptr, add, flags) LOCK_SUFFIX(xaddl, uint32_t, ptr, add, flags)
#define LOCK_XADD64(ptr, add, flags) LOCK_SUFFIX(xaddq, uint64_t, ptr, add, flags)

// 标志位解析
#define FLAG_CF (1 << 0)
#define FLAG_ZF (1 << 6)

static inline bool get_cf(unsigned long flags) { return (flags & FLAG_CF) != 0; }
static inline bool get_zf(unsigned long flags) { return (flags & FLAG_ZF) != 0; }

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
#define TEST_XADD(suffix, type, init, delta) do { \
    uint8_t buffer[PADDING_SIZE * 8 ]; \
    uint8_t expected[PADDING_SIZE * 8]; \
    fill_pattern(buffer, sizeof(buffer)); \
    for (size_t offset = 0; offset < 16; offset++) { \
        memcpy(expected, buffer, sizeof(buffer)); \
        type *ptr = (type*)(buffer + offset); \
        type *exp_ptr = (type*)(expected + offset); \
        *ptr = (type)(init); \
        *exp_ptr = *ptr + (type)(delta); \
        \
        type add = delta; \
        unsigned long flags = 0; \
        type old = LOCK_XADD##suffix(ptr, add, flags); \
        \
        bool expect_cf = (type)(*ptr) < (type)old; \
        bool expect_zf = (*ptr == 0); \
        \
        bool mem_ok = verify_memory(buffer, expected, sizeof(buffer), offset, sizeof(type)); \
        bool flag_ok = (get_cf(flags) == expect_cf) && (get_zf(flags) == expect_zf); \
        \
        printf("%2zu-bit[%2zu] OLD:%0*jx NEW:%0*jx CF:%d/%d ZF:%d/%d %s%s\n", \
               sizeof(type)*8, offset, \
               (int)(sizeof(type)*2), (uintmax_t)old, \
               (int)(sizeof(type)*2), (uintmax_t)*ptr, \
               get_cf(flags), expect_cf, \
               get_zf(flags), expect_zf, \
               mem_ok ? "\e[32m[MEM]\e[0m" : "\e[31m[MEM_FAILED]\e[0m", \
               flag_ok ? "\e[32m[FLAGS]\e[0m" : "\e[31m[FLAGS_FAILED]\e[0m"); \
    } \
} while(0)

// 信号处理相关
static jmp_buf env;
static volatile sig_atomic_t got_signal = 0;

void handler(int sig) {
    got_signal = sig;
    longjmp(env, 1);
}
/*
struct sigaction sa = {
    .sa_handler = handler,
    .sa_flags = SA_RESTART
};
*/
// multi-threaded
// 测试配置
#define NUM_THREADS  15    // 并发线程数
#define NUM_ITERATIONS 100000

// 内存对齐测试偏移量
const size_t test_offsets[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
#define NUM_OFFSETS (sizeof(test_offsets)/sizeof(test_offsets[0]))

// 共享内存结构
typedef struct {
    uint8_t buffer[PADDING_SIZE * 32] __attribute__((aligned(64)));  // 带保护区的内存块
    size_t offset;
    int width;
    volatile uint64_t expected;
    pthread_barrier_t barrier;
} SharedData;

SharedData shared;

// 线程函数宏（直接使用宏拼接类型）
#define DEFINE_THREAD_FUNC(width) \
void *thread_func_##width(void *arg) { \
    (void)arg; \
    uint##width##_t *ptr = (uint##width##_t*)(shared.buffer + shared.offset); \
    /* 等待所有线程就绪 */ \
    pthread_barrier_wait(&shared.barrier); \
    for (int i = 0; i < NUM_ITERATIONS; ++i) { \
        uint##width##_t add = 1; \
        unsigned long flags; \
        uint##width##_t old = LOCK_XADD##width(ptr, add, flags); \
        (void)old; \
    } \
    return NULL; \
}

DEFINE_THREAD_FUNC(8)
DEFINE_THREAD_FUNC(16)
DEFINE_THREAD_FUNC(32)
DEFINE_THREAD_FUNC(64)

// 执行多线程测试
void run_mt_test(int width) {
    pthread_t threads[NUM_THREADS];
    const size_t type_size = width/8;

    for (size_t i = 0; i < NUM_OFFSETS; i++) {
        size_t offset = test_offsets[i];
        if (offset + type_size > (size_t)8*width) continue;

        // 初始化共享内存
        memset(shared.buffer, 0xCC, PADDING_SIZE * 32);
        shared.offset = offset;
        shared.width = width;

        // 根据宽度初始化指针
	uint64_t maxval;
        switch(width) {
            case 8:  *(uint8_t*)(shared.buffer+offset) = 0; maxval=0x100ULL; break;
            case 16: *(uint16_t*)(shared.buffer+offset) = 0; maxval=0x10000ULL; break;
            case 32: *(uint32_t*)(shared.buffer+offset) = 0; maxval=0x100000000ULL; break;
            case 64: *(uint64_t*)(shared.buffer+offset) = 0; maxval = 0xFFFFFFFFFFFFFFFFLL; break;
        }

        shared.expected = NUM_THREADS * NUM_ITERATIONS % maxval;

        // 初始化屏障
        pthread_barrier_init(&shared.barrier, NULL, NUM_THREADS + 1);

        // 创建线程
        for (int t = 0; t < NUM_THREADS; t++) {
            switch(width) {
                case 8:  pthread_create(&threads[t], NULL, thread_func_8, NULL); break;
                case 16: pthread_create(&threads[t], NULL, thread_func_16, NULL); break;
                case 32: pthread_create(&threads[t], NULL, thread_func_32, NULL); break;
                case 64: pthread_create(&threads[t], NULL, thread_func_64, NULL); break;
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
        switch(width) {
            case 8:  actual = *(uint8_t*)(shared.buffer+offset); break;
            case 16: actual = *(uint16_t*)(shared.buffer+offset); break;
            case 32: actual = *(uint32_t*)(shared.buffer+offset); break;
            case 64: actual = *(uint64_t*)(shared.buffer+offset); break;
        }

        bool mem_ok = true;
        for (size_t j = 0; j < PADDING_SIZE; j++) {
            if (j >= offset && j < offset + type_size) continue;
            if (shared.buffer[j] != 0xCC) {
                printf("  Memory corruption @%zu: 0x%02X\n", j, shared.buffer[j]);
                mem_ok = false;
            }
        }

        printf("MT %2d-bit[%2zu] ACT:%10lu EXP:%10lu %s\n",
               width, offset, actual, shared.expected,
               (actual == shared.expected && mem_ok) ?
               "\e[32mPASS\e[0m" : "\e[31mFAIL\e[0m");
    }
}

int main() {
    struct sigaction sa = { .sa_handler = SIG_IGN };
    sigaction(SIGBUS, &sa, NULL);

    printf("========= 8-bit Tests =========\n");
    TEST_XADD(8, uint8_t, 0x00, 0x01);  // 最小值
    TEST_XADD(8, uint8_t, 0xFE, 0x03);  // 边界值
    TEST_XADD(8, uint8_t, 0xFF, 0x00);  // 最大值
    TEST_XADD(8, uint8_t, 0xCD, 0x32);  // 随机值

    printf("\n========= 16-bit Tests =========\n");
    TEST_XADD(16, uint16_t, 0x0000, 0x0001);  // 最小值
    TEST_XADD(16, uint16_t, 0xFFFE, 0x0003);  // 边界值
    TEST_XADD(16, uint16_t, 0xFFFF, 0x0000);  // 最大值
    TEST_XADD(16, uint16_t, 0xCDCD, 0x1234);  // 随机值

    printf("\n========= 32-bit Tests =========\n");
    TEST_XADD(32, uint32_t, 0x00000000, 0x00000001);  // 最小值
    TEST_XADD(32, uint32_t, 0xFFFFFFFE, 0x00000003);  // 边界值
    TEST_XADD(32, uint32_t, 0xFFFFFFFF, 0x00000000);  // 最大值
    TEST_XADD(32, uint32_t, 0xDEADBEEF, 0x12345678);  // 随机值

    printf("\n========= 64-bit Tests =========\n");
    TEST_XADD(64, uint64_t, 0x0000000000000000ULL, 0x0000000000000001ULL);  // 最小值
    TEST_XADD(64, uint64_t, 0xFFFFFFFFFFFFFFFEULL, 0x0000000000000003ULL);  // 边界值
    TEST_XADD(64, uint64_t, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL);  // 最大值
    TEST_XADD(64, uint64_t, 0xDEADBEEFCAFEBABEULL, 0x123456789ABCDEF0ULL);  // 随机值

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
