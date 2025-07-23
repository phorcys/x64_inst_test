#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <x86intrin.h>

// 测试参数
#define NUM_ITERATIONS 10000000 // 循环迭代次数
#define INS_PER_ITER 50         // 每次迭代执行的指令数
#define NUM_TESTS 10            // 重复测试次数

// 精确计时函数
static inline unsigned long long rdtsc() {
  unsigned int lo, hi;
  asm volatile("mfence\n\t"
               "rdtsc\n\t"
               : "=a"(lo), "=d"(hi));
  return ((unsigned long long)hi << 32) | lo;
}

#define TEST()                                                              \
  do {                                                                         \
    min_cycles = 1e20;                                                         \
    for (int test = 0; test < NUM_TESTS; test++) {                             \
      start = rdtsc();                                                         \
      asm volatile("mov %0, %%ecx\n\t"                                         \
                   "vpcmpeqd %%ymm0, %%ymm0, %%ymm0\n\t"                       \
                   "vpxor %%ymm1, %%ymm1, %%ymm1\n\t"                          \
                   "1:\n\t"                                                    \
                   "vpackuswb %%ymm1,%%ymm0,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "vpackuswb %%ymm1,%%ymm2,%%ymm2\n\t"                        \
                   "dec %%ecx\n\t"                                             \
                   "jnz 1b\n\t"                                                \
                   :                                                           \
                   : "i"(NUM_ITERATIONS)                             \
                   : "ecx", "ymm0", "ymm1", "ymm2", "cc");                     \
      end = rdtsc();                                                           \
      cycles_per_ins =                                                         \
          (double)(end - start) / (NUM_ITERATIONS * (INS_PER_ITER));           \
      if (cycles_per_ins < min_cycles) {                                       \
        min_cycles = cycles_per_ins;                                           \
      }                                                                        \
      cycles = cycles_per_ins;                                                 \
    }                                                                          \
  } while (0)

int main() {

  printf("VPACKSSWB Microbenchmark (x64)\n");
  printf("Iterations: %d, Instructions per iteration: %d\n\n", NUM_ITERATIONS,
         INS_PER_ITER);

  unsigned long long start, end;
  double min_cycles, cycles_per_ins, cycles;

  printf("Testing ");
  TEST();
  printf(" %.4f cycles/instruction\n", cycles);

  printf("Testing ");
  TEST();
  printf(" %.4f cycles/instruction\n", cycles);
  printf("Testing ");
  TEST();
  printf(" %.4f cycles/instruction\n", cycles);
  printf("Testing ");
  TEST();
  printf(" %.4f cycles/instruction\n", cycles);
  printf("Testing ");
  TEST();
  printf(" %.4f cycles/instruction\n", cycles);
  printf("Testing ");
  TEST();
  printf(" %.4f cycles/instruction\n", cycles);

  return 0;
}
