# Makefile for compiling and generating reference outputs

# Target directories 
DIRS := mmx lock sse sse2 sse3 sse4 avx crypt arith x87

# Default CFLAGS
CC := gcc-14
CFLAGS := -Wall -Wextra -O0 -march=core2 -pthread -D_GNU_SOURCE
MMX_CFLAGS := -Wall -Wextra -O0 -mmmx -pthread -D_GNU_SOURCE -lm
SSE_CFLAGS := -Wall -Wextra -O0 -msse -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -mlzcnt \
             -pthread -D_GNU_SOURCE
AVX_CFLAGS := -Wall -Wextra -O0 -march=core2 -mavx -pthread -D_GNU_SOURCE -lm
X87_CFLAGS := -Wall -Wextra -O0 -march=core2 -pthread -D_GNU_SOURCE

# Get all .c files
SRCS := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
EXES := $(SRCS:.c=)

# Default target
all: $(EXES)
	@echo "Build completed for: $(EXES)"

# Pattern rule to build executables
%: %.c
	@echo "Building $@ from $<"
	@if echo "$@" | grep -q '^mmx/'; then \
		$(CC) $(MMX_CFLAGS) -o $@ $<; \
	elif echo "$@" | grep -q '^sse/'; then \
		$(CC) $(SSE_CFLAGS) -o $@ $<; \
	elif echo "$@" | grep -q '^avx/'; then \
		$(CC) $(AVX_CFLAGS) -o $@ $<; \
	elif [ "$@" = "x87/test_x87_ficom" ]; then \
		$(CC) $(X87_CFLAGS) -c x87/ficom_asm.s -o x87/ficom_asm.o; \
		$(CC) $(X87_CFLAGS) -o $@ $< x87/ficom_asm.o -lm; \
	elif [ "$@" = "x87/test_x87_ficomp" ]; then \
		$(CC) $(X87_CFLAGS) -c x87/ficomp_asm.s -o x87/ficomp_asm.o; \
		$(CC) $(X87_CFLAGS) -o $@ $< x87/ficomp_asm.o -lm; \
	elif echo "$@" | grep -q '^x87/'; then \
		$(CC) $(X87_CFLAGS) -o $@ $< -lm; \
	else \
		$(CC) $(CFLAGS) -o $@ $<; \
	fi
	@if [ ! -f $@ ]; then echo "Error: Failed to build $@"; exit 1; fi

# Target to generate reference outputs
genref: $(if $(filter-out genref,$(MAKECMDGOALS)),genref-$(firstword $(filter mmx sse lock avx crypt arith x87,$(MAKECMDGOALS))),genref-all)

genref-all: all
	@for exe in $(EXES); do \
		echo "Generating reference for $$exe"; \
		./$$exe > $$exe.ref.txt; \
	done

genref-%: all
	@for exe in $(filter $*/%,$(EXES)); do \
		echo "Generating reference for $$exe"; \
		./$$exe > $$exe.ref.txt; \
	done

# Clean target
clean:
	@for exe in $(EXES); do \
		echo "Removing $$exe and $$exe.ref.txt"; \
		rm -f $$exe $$exe.ref.txt  $$exe.o; \
	done

# Test target to run all tests with translator
test: $(if $(filter-out test,$(MAKECMDGOALS)),test-$(firstword $(filter mmx sse lock avx crypt arith x87,$(MAKECMDGOALS))),test-all)

test-all:
	@for exe in $(EXES); do \
		$$TRANSLATOR ./$$exe > test_res.txt; \
		if diff -q test_res.txt $$exe.ref.txt >/dev/null; then \
			echo "\033[32m$$exe: PASSED\033[0m"; \
		else \
			echo "\033[31m$$exe: FAILED\033[0m"; \
			diff -u $$exe.ref.txt test_res.txt; \
		fi; \
		rm -f test_res.txt; \
	done

test-%:
	@for exe in $(filter $*/%,$(EXES)); do \
		$$TRANSLATOR ./$$exe > test_res.txt; \
		if diff -q test_res.txt $$exe.ref.txt >/dev/null; then \
			echo "\033[32m$$exe: PASSED\033[0m"; \
		else \
			echo "\033[31m$$exe: FAILED\033[0m"; \
			diff -u $$exe.ref.txt test_res.txt; \
		fi; \
		rm -f test_res.txt; \
	done

testn-%:
	@for exe in $(filter $*/%,$(EXES)); do \
		$$TRANSLATOR ./$$exe > test_res.txt; \
		if diff -q test_res.txt $$exe.ref.txt >/dev/null; then \
			echo "\033[32m$$exe: PASSED\033[0m"; \
		else \
			echo "\033[31m$$exe: FAILED\033[0m"; \
		fi; \
		rm -f test_res.txt; \
	done

%:
	@:

.PHONY: all genref clean test
