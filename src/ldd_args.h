#pragma once
#include <stdbool.h>
#include <windows.h>
typedef  enum {
    OUTPUT_LIST,
    OUTPUT_CMAKE,

}output_format;
typedef struct {
    bool bFlatten;
    bool bViewAll;
    output_format output_format;
} ldd_args;

typedef enum {
    FAILURE = -1,
    DO_LDD = 1,
    PRINT_HELP = 2,
    PRINT_VERSION = 3,
    MISSING_ARGS = 4,
} parse_args_code;

parse_args_code ldd_parse_args(int argc, LPWSTR *argv, ldd_args *ldd_args, int *argbase);
