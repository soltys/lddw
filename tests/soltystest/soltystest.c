#include "soltystest.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_context_free(test_context *test_context) {
    if (test_context->env != NULL) {
        for (int i = 0; test_context->env_size; i++) {
            if (test_context->env[i] != NULL) {
                free(test_context->env[i]->name);
                free(test_context->env[i]->value);
                free(test_context->env[i]);
                test_context->env[i] = NULL;
            }
        }
        free(test_context->env);
        test_context->env = NULL;
    }
    return 0;
}
int test_add_env(test_context *t, const char *name) {
    if (t->env == NULL) {
        t->env_size = 1;
        t->env = calloc(t->env_size, sizeof(test_env *));
    } else {
        t->env_size *= 2;
        t->env = realloc(t->env, t->env_size * sizeof(test_env *));
    }

    if (t->env == NULL) {
        return test_error(t, "allocation for new enviroment variables failed");
    }
    char enviroment_value[256];
    size_t enviroment_value_length;
    errno_t err = getenv_s(&enviroment_value_length, enviroment_value, sizeof(enviroment_value), name);
    if (err != 0) {
        return test_error(t, "error while getting enviroment variable (errno: %d)", err);
    }

    if (strncmp(enviroment_value, "", sizeof(enviroment_value)) == 0) {
        return test_error(t, "enviroment variable value cannot be empty");
    }

    test_env *new_entry = malloc(sizeof(test_env));
    if (new_entry == NULL) {
        return test_error(t, "allocation for new enviroment variable failed");
    }
    new_entry->name = _strdup(name);
    new_entry->value = _strdup(enviroment_value);
    t->env[t->env_size - 1] = new_entry;

    return 0;
}

const char *test_read_env(const test_context* t, const char *name) {
    for (int i = 0; i < t->env_size; i++) {
        if (strcmp(t->env[i]->name, name) == 0) {
            return t->env[i]->value;
        }
    }
    return NULL;
}

int _test_error(const test_context* t, const char *file, int line, const char *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    fprintf(stderr, "ERROR: [%s][%s]\n", t->suit_name, t->test_name);
    fprintf(stderr, "    "); // adding indent for next message
    vfprintf(stderr, format, argptr);
    fprintf(stderr, "\n>>  %s at line: %d\n\n", file, line);

    va_end(argptr);
    return EXIT_FAILURE;
}

int _run_test_suite(const char *test_suite_name, const test_func_reg *test_functions, int test_size) {
    printf("SUITE: %s\n", test_suite_name);
    for (int i = 0; i < test_size; i++) {

        test_func_reg test_func_reg = test_functions[i];
        printf("TEST: %s \n", test_func_reg.test_name);

        test_context test_context = {
            .suit_name = test_suite_name, .test_name = test_func_reg.test_name, .env = NULL, .env_size = 0};
        int ret = test_func_reg.test_function(&test_context);
        test_context_free(&test_context);
        if (ret != 0) {
            return ret;
        }
    }
    return 0;
}
