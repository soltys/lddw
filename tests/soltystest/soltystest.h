#pragma once
#include <stddef.h>

typedef struct {
    char *name;
    char *value;
} test_env;

typedef struct {
    const char *suit_name;
    const char *test_name;
    test_env **env;
    size_t env_size;
} test_context;

typedef int (*test_func)(const test_context* test_context);

typedef struct test_func_reg {
    const char *test_name;
    test_func test_function;
} test_func_reg;

#define ADD_TEST(test) {#test, test}
int _test_error(const test_context *t, const char *file, int line, const char *format, ...);
#define test_error(t, format, ...) _test_error(t, __FILE__, __LINE__, format, ##__VA_ARGS__)

int test_add_env(test_context *t, const char *name);
const char *test_read_env(const test_context *t, const char *name);
int test_context_free(test_context *test_context);

#define run_test_suite(name, test_funcs) _run_test_suite(name, test_funcs, sizeof(test_funcs) / sizeof(test_funcs[0]))
int _run_test_suite(const char *test_suite_name, const test_func_reg *test_functions, int test_size);
