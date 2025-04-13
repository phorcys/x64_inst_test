#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdatomic.h>

#define TEST_COUNT 10000

// Test lock sub with different operand sizes
static void test_lock_sub_size() {
    uint8_t byte1 = 0xFF, byte2 = 0x55;
    uint16_t word1 = 0xFFFF, word2 = 0x5555; 
    uint32_t dword1 = 0xFFFFFFFF, dword2 = 0x55555555;
    uint64_t qword1 = 0xFFFFFFFFFFFFFFFF, qword2 = 0x5555555555555555;

    printf("Testing LOCK SUB with different operand sizes:\n");

    // Test byte size
    asm volatile("lock subb %1, %0" : "+m"(byte1) : "ri"(byte2));
    printf("BYTE: 0x%02X - 0x%02X = 0x%02X\n", 0xFF, 0x55, byte1);

    // Test word size
    asm volatile("lock subw %1, %0" : "+m"(word1) : "ri"(word2));
    printf("WORD: 0x%04X - 0x%04X = 0x%04X\n", 0xFFFF, 0x5555, word1);

    // Test dword size  
    asm volatile("lock subl %1, %0" : "+m"(dword1) : "ri"(dword2));
    printf("DWORD: 0x%08X - 0x%08X = 0x%08X\n", 0xFFFFFFFF, 0x55555555, dword1);

    // Test qword size
    asm volatile("lock subq %1, %0" : "+m"(qword1) : "ri"(qword2));
    printf("QWORD: 0x%016lX - 0x%016lX = 0x%016lX\n", 
           0xFFFFFFFFFFFFFFFF, 0x5555555555555555, qword1);
}

// Test lock sub with different addressing modes
static void test_lock_sub_addressing() {
    uint32_t mem = 0x99999999;
    uint32_t reg = 0x87654321;

    printf("\nTesting LOCK SUB with different addressing modes:\n");

    // Register to memory
    asm volatile("lock subl %1, %0" : "+m"(mem) : "r"(reg));
    printf("REG->MEM: 0x99999999 - 0x87654321 = 0x%08X\n", mem);

    // Immediate to memory
    mem = 0x99999999;
    asm volatile("lock subl $0x87654321, %0" : "+m"(mem));
    printf("IMM->MEM: 0x99999999 - 0x87654321 = 0x%08X\n", mem);
}

// Test lock sub in multithreaded environment
static atomic_uint shared = 20000;
static void* thread_func(void* arg) {
    (void)arg;
    for (int i = 0; i < TEST_COUNT; ++i) {
        asm volatile("lock subl $1, %0" : "+m"(shared));
    }
    return NULL;
}

static void test_lock_sub_threading() {
    pthread_t t1, t2;
    
    printf("\nTesting LOCK SUB in multithreaded environment:\n");
    printf("Initial value: %u\n", shared);

    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final value: %u (expected: 0)\n", shared);
}

int main() {
    printf("=== Testing LOCK SUB instruction ===\n\n");
    
    test_lock_sub_size();
    test_lock_sub_addressing();
    test_lock_sub_threading();

    printf("\n=== LOCK SUB test complete ===\n");
    return 0;
}
